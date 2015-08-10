-- Create database as postgresql user and assign ownership to newly created user
-- database:    dbname
-- user:        username
--
-- sudo -u postgres createuser -S -D -R -P username
--  = CREATE ROLE username PASSWORD 'md50000000000000000000000000000000' NOSUPERUSER NOCREATEDB NOCREATEROLE INHERIT LOGIN;
--
-- sudo -u postgres createdb -e -O username dbname
--  = CREATE DATABASE dbname OWNER username;
--
-- authors: ifroml[at]fit.vutbr.cz; ivolf[at]fit.vutbr.cz

SET statement_timeout = 0;
SET lock_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SET check_function_bodies = false;
SET client_min_messages = warning;
SET default_with_oids = false;
SET search_path = public, pg_catalog;


-------------------------------------
-- CREATE public schema
-------------------------------------

CREATE SCHEMA IF NOT EXISTS public;
-- GRANT ALL ON SCHEMA public TO postgres;

-------------------------------------
-- DROP all VTApi objects
-------------------------------------

-- TODO: what if this doesn't exist??
SELECT VT_dataset_drop_all();


DROP TABLE IF EXISTS public.methods_params CASCADE;
DROP TABLE IF EXISTS public.methods_keys CASCADE;
DROP TABLE IF EXISTS public.methods CASCADE;
DROP TABLE IF EXISTS public.datasets CASCADE;

DROP TYPE IF EXISTS public.seqtype CASCADE;
DROP TYPE IF EXISTS public.inouttype CASCADE;
DROP TYPE IF EXISTS public.paramtype CASCADE;
DROP TYPE IF EXISTS public.methodkeytype CASCADE;
DROP TYPE IF EXISTS public.methodparamtype CASCADE;
DROP TYPE IF EXISTS public.pstatus CASCADE;
DROP TYPE IF EXISTS public.cvmat CASCADE;
DROP TYPE IF EXISTS public.vtevent CASCADE;
DROP TYPE IF EXISTS public.pstate CASCADE;

DROP FUNCTION IF EXISTS public.VT_dataset_create(VARCHAR, VARCHAR, VARCHAR, TEXT) CASCADE;
DROP FUNCTION IF EXISTS public.VT_dataset_drop(VARCHAR) CASCADE;
DROP FUNCTION IF EXISTS public.VT_dataset_truncate(VARCHAR) CASCADE;
DROP FUNCTION IF EXISTS public.VT_dataset_support_create(VARCHAR) CASCADE;

DROP FUNCTION IF EXISTS public.VT_method_add(VARCHAR, methodkeytype[], methodparamtype[], BOOLEAN, VARCHAR, TEXT) CASCADE;
DROP FUNCTION IF EXISTS public.VT_method_delete(VARCHAR) CASCADE;

DROP FUNCTION IF EXISTS public.VT_process_output_create(VARCHAR, VARCHAR, VARCHAR) CASCADE;
DROP FUNCTION IF EXISTS public.VT_process_output_drop(VARCHAR, BOOLEAN, VARCHAR) CASCADE;
DROP FUNCTION IF EXISTS public.VT_process_output_idxquery(VARCHAR, NAME, REGTYPE, INT, VARCHAR) CASCADE;


DROP FUNCTION IF EXISTS public.tsrange(TIMESTAMP WITHOUT TIME ZONE, REAL) CASCADE;
DROP FUNCTION IF EXISTS public.trg_interval_provide_realtime() CASCADE;


-------------------------------------
-- CREATE user-defined data types
-------------------------------------

-- sequence type
CREATE TYPE seqtype AS ENUM (
    'video',    -- sequence is video
    'images',   -- sequence is image folder
    'data'      -- unspecified
);    

-- method key type - does column contain input or output data?
CREATE TYPE inouttype AS ENUM (
    'in',           -- input = column from other method's processes' output table
    'out'           -- output = column from this method's processes' output table
);

-- supported data types for method params
CREATE TYPE paramtype AS ENUM (
    'string',       -- characeter string
    'int',          -- 4-byte integer
    'double',       -- double precision float
    'int[]',        -- integer vector
    'double[]'      -- double vector
);

-- definition of input/output column for method's processes (used when creating method)
CREATE TYPE methodkeytype AS (
    keyname        NAME,      -- column name
    typname        REGTYPE,   -- column data type
    inout          INOUTTYPE, -- is column a process input/output?
    required       BOOLEAN,   -- is value in column required (must not be NULL) or not?
    indexedkey     BOOLEAN,   -- is column indexed?
    indexedparts   INT[],     -- which parts of composite type is indexed?
    description    VARCHAR    -- key description
);

-- definition of input params for method's processes (used when creating method)
CREATE TYPE methodparamtype AS (
    paramname     NAME,      -- param name
    type          PARAMTYPE, -- param type (enum)
    required      BOOLEAN,   -- is param required?
    default_val   VARCHAR,   -- param default value (used when param is required and not given)
    valid_range   VARCHAR,   -- range definition for numeric types (custom format)
    description   VARCHAR    -- param description
);


-- process state enum
CREATE TYPE pstatus AS ENUM (
    'created',  -- process has been newly registered but not yet started
    'running',  -- process is currently working
    'suspended',-- process is currently in paused state
    'finished', -- process has finished succesfully
    'error'     -- process has finished with an error
);

-- OpenCV matrix type
CREATE TYPE cvmat AS (
    type integer,       -- type of elements (see OpenCV matrix types)
    dims integer[],     -- dimensions sizes
    data bytea          -- matrix data
);

-- VTApi event type
CREATE TYPE vtevent AS (
    group_id integer,   -- groups associate events together
    class_id integer,   -- event class (user-defined)
    is_root boolean,    -- is this event a meta-event (eg. trajectory envelope)
    region box,         -- event region in video
    score double precision, -- event score (user-defined)
    data bytea          -- additional custom user-defined data
);

-- process state type
CREATE TYPE pstate AS (
    status public.pstatus,  -- process status
    progress real,          -- process progress (0-100)
    current_item varchar,   -- currently processed item
    last_error varchar      -- error message
);


-------------------------------------
-- CREATE tables
-------------------------------------

-- dataset list
CREATE TABLE datasets (
    dsname NAME NOT NULL,
    dslocation VARCHAR NOT NULL,
    friendly_name VARCHAR,
    description TEXT,
    created TIMESTAMP WITHOUT TIME ZONE DEFAULT now(),
    CONSTRAINT dataset_pk PRIMARY KEY (dsname)
);

-- method list
CREATE TABLE methods (
    mtname name NOT NULL,
    usert   BOOLEAN   DEFAULT FALSE,
    friendly_name VARCHAR,
    description TEXT,
    created TIMESTAMP WITHOUT TIME ZONE DEFAULT now(),
    CONSTRAINT methods_pk PRIMARY KEY (mtname)
);

-- methods in/out columns definitions
CREATE TABLE methods_keys (
    mtname     NAME        NOT NULL,
    keyname    NAME        NOT NULL,
    typname    REGTYPE     NOT NULL,
    inout      INOUTTYPE   NOT NULL,
    required   BOOLEAN     DEFAULT FALSE,
    indexedkey     BOOLEAN   DEFAULT FALSE,
    indexedparts   INT[]     DEFAULT NULL,
    description    VARCHAR   DEFAULT NULL,
    CONSTRAINT methods_keys_pk PRIMARY KEY (mtname, keyname),
    CONSTRAINT mtname_fk FOREIGN KEY (mtname)
      REFERENCES methods(mtname) ON UPDATE CASCADE ON DELETE CASCADE
);

-- methods input parameters definitions
CREATE TABLE methods_params (
    mtname        NAME        NOT NULL,
    paramname     NAME        NOT NULL,
    type          PARAMTYPE   NOT NULL,
    required      BOOLEAN     DEFAULT FALSE,
    default_val   VARCHAR     DEFAULT NULL,
    valid_range   VARCHAR     DEFAULT NULL,
    description   VARCHAR     DEFAULT NULL,
    CONSTRAINT methods_params_pk PRIMARY KEY (mtname, paramname),
    CONSTRAINT mtname_fk FOREIGN KEY (mtname)
      REFERENCES methods(mtname) ON UPDATE CASCADE ON DELETE CASCADE
);



-------------------------------------
-- VTApi CORE UNDERLYING functions for DATASETS
-------------------------------------

-- DATASET: create
-- Function behavior:
--   * Successful creation of a dataset => returns 1
--   * Whole dataset structure already exists => returns 0
--   * Dataset is already registered, but _dslocation, _friendly_name or _description is different than in DB => returns -1
--   * There already exist some parts of dataset and some parts not exist => INTERRUPTED with INCONSISTENCY EXCEPTION
--   * Some error in statement (ie. CREATE, DROP, INSERT, ..) => INTERRUPTED with statement ERROR/EXCEPTION
CREATE OR REPLACE FUNCTION VT_dataset_create (_dsname VARCHAR, _dslocation VARCHAR, _friendly_name VARCHAR DEFAULT NULL, _description TEXT DEFAULT NULL) 
  RETURNS INT AS 
  $VT_dataset_create$
  DECLARE
    _dsnamecount    INT;
    _dsidentical    INT;
    _schemacount    INT;
    _classescount   INT;
    
    __tmp1 VARCHAR;
    __tmp2 VARCHAR;
  BEGIN
    IF _dsname IN ('public', 'pg_catalog') THEN
      RAISE EXCEPTION 'Usage of the name "%" for dataset name is not allowed!', _dsname; 
    END IF;

    EXECUTE 'SELECT COUNT(*)
             FROM public.datasets
             WHERE dsname = ' || quote_literal(_dsname)
      INTO _dsnamecount;
    EXECUTE 'SELECT COUNT(*)
             FROM pg_catalog.pg_namespace
             WHERE nspname = ' || quote_literal(_dsname)
      INTO _schemacount;


    IF _friendly_name IS NULL THEN
      __tmp1 := 'IS NULL';
    ELSE 
      __tmp1 := '= ' || quote_literal(_friendly_name);
    END IF;
    IF _description IS NULL THEN
      __tmp2 := 'IS NULL';
    ELSE 
      __tmp2 := '= ' || quote_literal(_description);
    END IF;

    EXECUTE 'SELECT COUNT(*)
             FROM public.datasets
             WHERE dsname = ' || quote_literal(_dsname)
             || ' AND dslocation = ' || quote_literal(_dslocation)
             || ' AND friendly_name ' || __tmp1
             || ' AND description ' || __tmp2
      INTO _dsidentical;
    
    IF _dsnamecount <> _dsidentical THEN
      RAISE WARNING 'Dataset "%" already exists, but has different value in "dslocation" or/and in "friendly_name", "description" property.', _dsname;
      RETURN -1;
    END IF;

    -- check if schema already exists (only dataset core relations; we assume, that the user did not modify other database objects (like table columns, primary and foreign key constraints, indexes. etc.)
    IF _schemacount = 1 THEN
      EXECUTE 'SELECT COUNT(*)
               FROM pg_catalog.pg_class
               WHERE relname IN (
                   ''sequences'', ''tasks'', ''processes'', 
                   ''rel_processes_sequences_assigned'', ''rel_tasks_tasks_prerequisities'', ''rel_tasks_sequences_done''
                 )
                 AND relnamespace = (
                   SELECT oid
                   FROM pg_catalog.pg_namespace
                   WHERE nspname = ' || quote_literal(_dsname) || '
                 );'
        INTO _classescount;

      -- check inconsistence || already existing dataset
      IF _dsnamecount <> 1 OR _classescount <> 6 THEN
        -- inconsistence
        RAISE EXCEPTION 'Inconsistency was detected in dataset "%".', _dsname;
      ELSE
        -- already existing dataset
        RAISE NOTICE 'Dataset "%" can not be created due to it already exists.', _dsname;
        RETURN 0;
      END IF;
      
    END IF;

    -- create schema (dataset)
    PERFORM public.VT_dataset_support_create(_dsname);

    IF _dsnamecount = 0 THEN
      -- register dataset
        EXECUTE 'INSERT INTO public.datasets(dsname, dslocation, friendly_name, description) VALUES ('
                || quote_literal(_dsname) || ', '
                || quote_literal(_dslocation) || ', '
                || quote_nullable(_friendly_name) || ', '
                || quote_nullable(_description) || ');';
    ELSE
      RAISE NOTICE 'Dataset inconsistency was repaired by creating dataset "%".', _dsname;
    END IF;
    
    RETURN 1;
    
    EXCEPTION WHEN OTHERS THEN
      RAISE EXCEPTION 'Some problem occured during the creation of the dataset "%". (Details: ERROR %: %)', _dsname, SQLSTATE, SQLERRM;
  END;
  $VT_dataset_create$
  LANGUAGE plpgsql CALLED ON NULL INPUT;



-- DATASET: drop (delete)
-- Function behavior:
--   * Successful removal of a dataset => returns TRUE
--   * Whole dataset structure is no longer available => returns FALSE
--   * Dataset is not registered, but schema exists => INTERRUPTED with INCONSISTENCY EXCEPTION
--   * Some error in statement (ie. CREATE, DROP, INSERT, ..) => INTERRUPTED with statement ERROR/EXCEPTION
CREATE OR REPLACE FUNCTION VT_dataset_drop (_dsname VARCHAR) 
  RETURNS BOOLEAN AS 
  $VT_dataset_drop$
  DECLARE
    _dsnamecount   INT;
    _schemacount   INT;
  BEGIN
    IF _dsname IN ('public', 'pg_catalog') THEN
      RAISE EXCEPTION 'Can not drop dataset "%" due to it is not valid VTApi dataset (reserved name whose use for dataset name is not allowed!).', _dsname; 
    END IF;

    EXECUTE 'SELECT COUNT(*)
             FROM public.datasets
             WHERE dsname = ' || quote_literal(_dsname)
      INTO _dsnamecount;
    EXECUTE 'SELECT COUNT(*)
             FROM pg_catalog.pg_namespace
             WHERE nspname = ' || quote_literal(_dsname)
      INTO _schemacount;

    IF _dsnamecount = 0 THEN
      IF _schemacount = 0 THEN
        RAISE NOTICE 'Dataset "%" can not be dropped due to it is no longer available.', _dsname;
        RETURN FALSE;
      ELSE
        RAISE EXCEPTION 'Inconsistency was detected in dataset "%".', _dsname;
      END IF;
    END IF;

    EXECUTE 'DELETE FROM public.datasets WHERE dsname = ' || quote_literal(_dsname);
    IF _schemacount <> 0 THEN
      EXECUTE 'DROP SCHEMA ' || quote_ident(_dsname) || ' CASCADE';
    ELSE
      RAISE NOTICE 'Dataset inconsistency was repaired by dropping dataset "%".', _dsname;
    END IF;
    
    RETURN TRUE;
    
    EXCEPTION WHEN OTHERS THEN
      RAISE EXCEPTION 'Some problem occured during the removal of the dataset "%". (Details: ERROR %: %)', _dsname, SQLSTATE, SQLERRM;
  END;
  $VT_dataset_drop$
  LANGUAGE plpgsql STRICT;



CREATE OR REPLACE FUNCTION VT_dataset_drop_all ()
  RETURNS BOOLEAN AS
  $VT_dataset_drop_all$
  DECLARE
    _dsname   public.datasets.dsname%TYPE;
  BEGIN
    FOR _dsname IN SELECT dsname FROM public.datasets LOOP
      EXECUTE 'DROP SCHEMA IF EXISTS ' || quote_ident(_dsname) || ' CASCADE;';
    END LOOP;
    
    TRUNCATE TABLE public.datasets;
    
    RETURN TRUE;
    
    EXCEPTION WHEN OTHERS THEN
      RAISE EXCEPTION 'Some problem occured during the removal of all datasets. (Details: ERROR %: %)', SQLSTATE, SQLERRM;
  END;
  $VT_dataset_drop_all$
  LANGUAGE plpgsql STRICT;



-- DATASET: truncate
-- Function behavior:
--   * Successful truncation of a dataset => returns TRUE
--   * Whole dataset structure is no longer available => returns FALSE
--   * Dataset is not registered, but schema exists => INTERRUPTED with INCONSISTENCY EXCEPTION
--   * Some error in statement (ie. CREATE, DROP, INSERT, ..) => INTERRUPTED with statement ERROR/EXCEPTION
CREATE OR REPLACE FUNCTION VT_dataset_truncate (_dsname VARCHAR)
  RETURNS BOOLEAN AS
  $VT_dataset_truncate$
  DECLARE
    _dsnamecount   INT;
    _schemacount   INT;
  BEGIN
    EXECUTE 'SELECT COUNT(*)
             FROM public.datasets
             WHERE dsname = ' || quote_literal(_dsname)
      INTO _dsnamecount;
    EXECUTE 'SELECT COUNT(*)
             FROM pg_catalog.pg_namespace
             WHERE nspname = ' || quote_literal(_dsname)
      INTO _schemacount;
    
    IF _dsnamecount = 0 THEN
      IF _schemacount = 0 THEN
        RAISE WARNING 'Dataset "%" can not be truncated due to it is no longer available. You need to call VT_dataset_create() function instead.', _dsname;
        RETURN FALSE;
      ELSE
        RAISE EXCEPTION 'Inconsistency was detected in dataset "%".', _dsname;
      END IF;
    END IF;
    
    IF _schemacount = 1 THEN
      EXECUTE 'DROP SCHEMA ' || quote_ident(_dsname) || ' CASCADE';
    ELSE
      RAISE NOTICE 'Dataset inconsistency was repaired by truncating dataset "%".', _dsname;
    END IF;
    
    PERFORM public.VT_dataset_support_create(_dsname);
    
    RETURN TRUE;

    EXCEPTION WHEN OTHERS THEN
      RAISE EXCEPTION 'Some problem occured during the truncation of the dataset "%". (Details: ERROR %: %)', _dsname, SQLSTATE, SQLERRM;
  END;
  $VT_dataset_truncate$
  LANGUAGE plpgsql STRICT;



-- Warning: DO NOT USE this function as standalone function!
--          It is a support function for VT_dataset_create and VT_dataset_truncate only.
CREATE OR REPLACE FUNCTION VT_dataset_support_create (_dsname VARCHAR)
  RETURNS VOID AS
  $VT_dataset_create_support$
  DECLARE
    _schemacount   INT;
  BEGIN
    EXECUTE 'SELECT COUNT(*)
             FROM pg_catalog.pg_namespace
             WHERE nspname = ' || quote_literal(_dsname)
      INTO _schemacount;

    IF _schemacount = 0 THEN
      EXECUTE 'CREATE SCHEMA ' || quote_ident(_dsname);
    END IF;
    EXECUTE 'SET search_path=' || quote_ident(_dsname);
    
    -- create sequences in schema (dataset)
    CREATE TABLE sequences (
      seqname name NOT NULL,
      seqlocation character varying,
      seqtyp public.seqtype,
      vid_length integer,
      vid_fps real,
      vid_speed  real  DEFAULT 1,
      vid_time timestamp without time zone,
      created timestamp without time zone DEFAULT now(),
      comment text DEFAULT NULL,
      CONSTRAINT sequences_pk PRIMARY KEY (seqname)
    );
    CREATE INDEX sequences_seqtyp_idx ON sequences(seqtyp);

    -- table for processing tasks
    CREATE TABLE tasks (
      taskname name NOT NULL,
      mtname name NOT NULL,
      params varchar,
      outputs regclass,
      created timestamp without time zone DEFAULT now(),
      CONSTRAINT tasks_pk PRIMARY KEY (taskname),
      CONSTRAINT mtname_fk FOREIGN KEY (mtname)
        REFERENCES public.methods(mtname) ON UPDATE CASCADE ON DELETE CASCADE
    );
    CREATE INDEX tasks_mtname_idx ON tasks(mtname);
    CREATE INDEX tasks_created_idx ON tasks(created);

    -- table for processes
    CREATE TABLE processes (
      prsid serial NOT NULL,
      taskname name NOT NULL,
      state public.pstate DEFAULT '(created,0,,)',
      ipc_port int DEFAULT 0,
      created timestamp without time zone DEFAULT now(),
      CONSTRAINT processes_pk PRIMARY KEY (prsid)
    );
    CREATE INDEX processes_taskname_idx ON processes(taskname);
    CREATE INDEX processes_status_idx ON processes(( (state).status ));

    -- table for assigning sequences to be processed by process
    CREATE TABLE rel_processes_sequences_assigned (
      prsid int NOT NULL,
      seqname name NOT NULL,
      CONSTRAINT rel_processes_sequences_pk PRIMARY KEY (prsid, seqname),
      CONSTRAINT prsid_fk FOREIGN KEY (prsid)
        REFERENCES processes(prsid) ON UPDATE CASCADE ON DELETE CASCADE,
      CONSTRAINT seqname_fk FOREIGN KEY (seqname)
        REFERENCES sequences(seqname) ON UPDATE CASCADE ON DELETE CASCADE
    );

    -- table for defining prerequisite tasks for other tasks
    CREATE TABLE rel_tasks_tasks_prerequisities (
      taskname name NOT NULL,
      taskprereq name NOT NULL,
      CONSTRAINT rel_tasks_prerequisities_pk PRIMARY KEY (taskname, taskprereq),
      CONSTRAINT taskname_fk FOREIGN KEY (taskname)
        REFERENCES tasks(taskname) ON UPDATE CASCADE ON DELETE CASCADE,
      CONSTRAINT taskprereq_fk FOREIGN KEY (taskprereq)
        REFERENCES tasks(taskname) ON UPDATE CASCADE ON DELETE CASCADE
    );

    -- table for recording which sequences has been processed for task
    CREATE TABLE rel_tasks_sequences_done (
      taskname name NOT NULL,
      seqname name NOT NULL,
      is_done boolean default false,
      CONSTRAINT rel_tasks_sequences_done_pk PRIMARY KEY (taskname, seqname),
      CONSTRAINT taskname_fk FOREIGN KEY (taskname)
        REFERENCES tasks(taskname) ON UPDATE CASCADE ON DELETE CASCADE,
      CONSTRAINT seqname_fk FOREIGN KEY (seqname)
        REFERENCES sequences(seqname) ON UPDATE CASCADE ON DELETE CASCADE
    );
    CREATE INDEX rel_tasks_sequences_done_is_done_idx ON rel_tasks_sequences_done(is_done);

  END;
  $VT_dataset_create_support$
  LANGUAGE plpgsql STRICT;





-------------------------------------
-- VTApi CORE UNDERLYING functions for METHODS
-------------------------------------

-- METHOD: add
-- Function behavior:
--   * Successful addition of a method => returns TRUE
--   * Method with the same name is already available => returns FALSE
--   * Some error in statement => INTERRUPTED with statement ERROR/EXCEPTION
CREATE OR REPLACE FUNCTION VT_method_add (_mtname VARCHAR, _mkeys METHODKEYTYPE[], _mparams METHODPARAMTYPE[], _usert BOOLEAN DEFAULT FALSE, _mfriendly_name VARCHAR DEFAULT NULL, _description TEXT DEFAULT NULL)
  RETURNS BOOLEAN AS
  $VT_method_add$
  DECLARE
    _mtcount   INT;
    
    _stmt      VARCHAR;
    _inscols   VARCHAR   DEFAULT '';
    _insvals   VARCHAR   DEFAULT '';
  BEGIN
    EXECUTE 'SELECT COUNT(*) FROM public.methods WHERE mtname = ' || quote_literal(_mtname) INTO _mtcount;
    IF _mtcount <> 0 THEN
      RETURN FALSE;
    END IF;

    IF _usert = TRUE THEN
      _inscols := ', usert';
      _insvals := ', TRUE';
    END IF;
    
    IF _description IS NOT NULL THEN
      _inscols := _inscols || ', description';
      _insvals := _insvals || ', ' || quote_literal(_description);
    END IF;

    IF _mfriendly_name IS NOT NULL THEN
      _inscols := _inscols || ', friendly_name';
      _insvals := _insvals || ', ' || quote_literal(_mfriendly_name);
    END IF;
    
    _stmt := 'INSERT INTO public.methods (mtname' || _inscols || ') VALUES (' || quote_literal(_mtname) || _insvals || ');';
    EXECUTE _stmt;

    IF _mkeys IS NOT NULL THEN
      _insvals := '';
      FOR _i IN 1 .. array_upper(_mkeys, 1) LOOP
        IF _mkeys[_i].keyname IS NULL THEN
          RAISE EXCEPTION 'Method key name ("keyname" property) can not be NULL!';
        END IF;

        IF _mkeys[_i].typname IS NULL THEN
          RAISE EXCEPTION 'Method key type ("typname" property) can not be NULL!';
        END IF;

        IF _mkeys[_i].inout IS NULL THEN
          RAISE EXCEPTION 'Method scope ("inout" property) can not be NULL!';
        END IF;      

        _insvals := _insvals || '(' || quote_literal(_mtname) || ', ' || quote_literal(_mkeys[_i].keyname) || ', ' || quote_literal(_mkeys[_i].typname) || ', ' || quote_literal(_mkeys[_i].inout) || ', ';

        IF _mkeys[_i].inout = 'out' THEN
          _insvals := _insvals || quote_nullable(_mkeys[_i].required) || ', ' || quote_nullable(_mkeys[_i].indexedkey) || ', ' || quote_nullable(_mkeys[_i].indexedparts) || ', ';
        ELSE
          _insvals := _insvals || 'NULL, NULL, NULL, ';
        END IF;

        _insvals := _insvals || quote_nullable(_mkeys[_i].description) || '), ';
      END LOOP;

      _stmt := 'INSERT INTO public.methods_keys(mtname, keyname, typname, inout, required, indexedkey, indexedparts, description) VALUES ' || rtrim(_insvals, ', ');
      EXECUTE _stmt;
    END IF;

    
    IF _mparams IS NOT NULL THEN
      _insvals := '';
      FOR _i IN 1 .. array_upper(_mparams, 1) LOOP
        IF _mparams[_i].paramname IS NULL THEN
          RAISE EXCEPTION 'Method parameter name ("paramname" property) can not be NULL!';
        END IF;

        IF _mparams[_i].type IS NULL THEN
          RAISE EXCEPTION 'Method parameter type ("type" property) can not be NULL!';
        END IF;


        _insvals := _insvals || '(' || quote_literal(_mtname)                   || ', ' 
                                    || quote_literal(_mparams[_i].paramname)    || ', ' 
                                    || quote_literal(_mparams[_i].type)         || ', '
                                    -- this values can be NULL
                                    || quote_nullable(_mparams[_i].required)    || ', '
                                    || quote_nullable(_mparams[_i].default_val) || ', '
                                    || quote_nullable(_mparams[_i].valid_range) || ', '
                                    || quote_nullable(_mparams[_i].description)
                             || '), ';
      END LOOP;

      _stmt := 'INSERT INTO public.methods_params(mtname, paramname, type, required, default_val, valid_range, description) VALUES ' || rtrim(_insvals, ', ');
      EXECUTE _stmt;
    END IF;
    
    RETURN TRUE;

    EXCEPTION WHEN OTHERS THEN
      RAISE EXCEPTION 'Some problem occured during the addition of the method "%". (Details: ERROR %: %)', _mtname, SQLSTATE, SQLERRM;
  END;
  $VT_method_add$
  LANGUAGE plpgsql CALLED ON NULL INPUT;



-- METHOD: delete
-- Function behavior:
--   * Successful deletion of a method => returns TRUE
--   * Method with the same name is no longer available => returns FALSE
--   * If deletion is not forced and methods' dependencies exist => INTERRUPTED with EXCEPTION
--   * Some error in statement => INTERRUPTED with statement ERROR/EXCEPTION
CREATE OR REPLACE FUNCTION VT_method_delete (_mtname VARCHAR, _force BOOLEAN DEFAULT FALSE)
  RETURNS BOOLEAN AS
  $VT_method_delete$
  DECLARE
    _controlcount   INT;
    _tblname        VARCHAR;
  BEGIN
    EXECUTE 'SELECT COUNT(*) FROM public.methods WHERE mtname = ' || quote_literal(_mtname) INTO _controlcount;
    IF _controlcount <> 1 THEN
      RETURN FALSE;
    END IF;
    
    -- pokud je _force false, SELECT procesů kde je method name - IF TRUE - exception 
    IF _force = FALSE THEN
      FOR _tblname IN SELECT tgconstrrelid::regclass::varchar FROM pg_catalog.pg_trigger WHERE tgrelid = 'public.methods'::regclass AND tgconstrrelid::regclass::varchar LIKE '%.processes' AND tgfoid::regproc::varchar = '"RI_FKey_cascade_del"' LOOP
        EXECUTE 'SELECT COUNT(*) FROM ' || _tblname || ' WHERE mtname = ' || quote_literal(_mtname) INTO _controlcount;
        IF _controlcount > 0 THEN
          RAISE EXCEPTION 'Can not delete method "%" due to dependent processes in dataset "%".', _mtname, regexp_replace(_tblname, '.processes$', '');
        END IF;
      END LOOP;
    END IF;
    
    EXECUTE 'DELETE FROM public.methods WHERE mtname = ' || quote_literal(_mtname);
    RETURN TRUE;
    
    EXCEPTION WHEN OTHERS THEN
      RAISE EXCEPTION 'Some problem occured during the deletion of the method "%". (Details: ERROR %: %)', _mtname, SQLSTATE, SQLERRM;
  END;
  $VT_method_delete$
  LANGUAGE plpgsql CALLED ON NULL INPUT;





-------------------------------------
-- VTApi CORE UNDERLYING functions for PROCESS' OUTPUT of given method
-------------------------------------

-- PROCESS OUTPUT: CREATE
-- Function args:
--   * _mtname - name of method for which will be created process' output table
--   * _dsname - name of dataset where will be created process' output table (optional)
--   * _reqoutname - process' ouput table according to user requirements (optional)
-- Function behavior:
--   * Process' output table succesfully created => returns TRUE
--   * Some error in statement => INTERRUPTED with statement ERROR/EXCEPTION
CREATE OR REPLACE FUNCTION public.VT_process_output_create (_mtname VARCHAR, _dsname VARCHAR DEFAULT NULL, _reqoutname VARCHAR DEFAULT NULL)
  RETURNS BOOLEAN AS
  $VT_method_output_create$
  DECLARE
    _keyname        public.methods_keys.keyname%TYPE;
    _typname        public.methods_keys.typname%TYPE;
    _required       public.methods_keys.required%TYPE;
    _indexedkey     public.methods_keys.indexedkey%TYPE;
    _indexedparts   public.methods_keys.indexedparts%TYPE;
    _typname2       pg_catalog.pg_attribute.atttypid%TYPE;
    _required2      pg_catalog.pg_attribute.attnotnull%TYPE;
  
    _stmt           VARCHAR   DEFAULT '';
    _tmpstmt        VARCHAR   DEFAULT '';
    _idxstmt        VARCHAR   DEFAULT '';
    
    _controlcount   INT;
    _i              INT;
    _usert          BOOLEAN;
    _keyio          VARCHAR;
    _namespaceoid   OID;
    _idxnames       VARCHAR[];
    _idxprefix      VARCHAR   DEFAULT '';

    __outname       VARCHAR   DEFAULT '';
  BEGIN
    IF _dsname IN ('public', 'pg_catalog') THEN
      RAISE EXCEPTION 'Usage of the name "%" for dataset name is not allowed!', _dsname; 
    END IF;

    IF _reqoutname IS NULL THEN
      _reqoutname := _mtname || '_out';
    ELSE
      -- check if requested process' output name is not reserved to other infrastructure tables
      IF _reqoutname IN ('processes', 'sequences') THEN
        RAISE EXCEPTION 'Usage of the name "%" for process output is not allowed!', _reqoutname;
      END IF;
    END IF;

    -- check if method exists
    EXECUTE 'SELECT COUNT(*) FROM public.methods WHERE mtname = ' || quote_literal(_mtname) INTO _controlcount;
    IF _controlcount <> 1 THEN
      RAISE EXCEPTION 'Method "%" does not exist.', _mtname;
    END IF;
    
    IF _dsname IS NULL THEN
      _dsname := current_schema();
    ELSE
      _idxprefix := _dsname || '.';
    END IF;

    __outname := quote_ident(_dsname) || '.' || quote_ident(_reqoutname);

    -- check if dataset (schema) exists
    EXECUTE 'SELECT oid
             FROM pg_catalog.pg_namespace
             WHERE nspname = ' || quote_literal(_dsname)
        INTO _namespaceoid;
    
    IF _namespaceoid IS NULL THEN
      RAISE EXCEPTION 'Dataset "%" does not exist.', _dsname;
    END IF;
    
    -- check if process' output table exists
    EXECUTE 'SELECT COUNT(*)
             FROM pg_catalog.pg_class
             WHERE relname = ' || quote_literal(_reqoutname) || '
               AND relnamespace = ' || _namespaceoid || '
               AND relkind = ''r'';'
        INTO _controlcount;
        
    EXECUTE 'SELECT usert FROM public.methods WHERE mtname = ' || quote_literal(_mtname) INTO _usert;


    IF _controlcount = 0 THEN
      _stmt := 'id        SERIAL   NOT NULL,
                seqname   NAME     NOT NULL,
                prsname   NAME,
                t1        INT      NOT NULL,
                t2        INT      NOT NULL,';

      IF _usert = TRUE THEN
        _stmt := _stmt || 'rt_start      TIMESTAMP WITHOUT TIME ZONE   DEFAULT NULL,'; -- trigger supplied
      END IF;

      _stmt := _stmt || 'sec_length    REAL,   -- trigger supplied
                         imglocation   VARCHAR, ';

      FOR _keyname, _typname, _required, _indexedkey, _indexedparts IN EXECUTE 'SELECT keyname, typname, required, indexedkey, indexedparts FROM public.methods_keys WHERE mtname = ' || quote_literal(_mtname) || ' AND inout = ''out''' LOOP
        _stmt := _stmt || ' ' || quote_ident(_keyname) || ' ' || _typname::regtype;
        IF _required = TRUE THEN
          _stmt := _stmt || ' NOT NULL';
        END IF;
        _stmt := _stmt || ', ';

        IF _indexedkey THEN
          _idxstmt := _idxstmt || public.VT_process_output_idxquery(_reqoutname, _keyname, _typname, NULL, _dsname);
        END IF;

        IF _indexedparts IS NOT NULL THEN
          FOREACH _i IN ARRAY _indexedparts LOOP
            _idxstmt := _idxstmt || public.VT_process_output_idxquery(_reqoutname, _keyname, _typname, _i, _dsname);
          END LOOP;
        END IF;

      END LOOP;

      _stmt := 'CREATE TABLE ' || __outname || '('
                  || _stmt ||
               '  created       TIMESTAMP WITHOUT TIME ZONE   DEFAULT now(),
                  CONSTRAINT ' || quote_ident(_reqoutname || '_pk') || ' PRIMARY KEY (id),
                  CONSTRAINT seqname_fk FOREIGN KEY (seqname)
                    REFERENCES sequences(seqname) ON UPDATE CASCADE ON DELETE RESTRICT,
                  CONSTRAINT prsname_fk FOREIGN KEY (prsname)
                    REFERENCES processes(prsname) ON UPDATE CASCADE ON DELETE CASCADE
                );
                CREATE INDEX ' || quote_ident(_reqoutname || '_seqname_idx') || ' ON ' || __outname || '(seqname);
                CREATE INDEX ' || quote_ident(_reqoutname || '_prsname_idx') || ' ON ' || __outname || '(prsname);
                CREATE INDEX ' || quote_ident(_reqoutname || '_sec_length_idx') || ' ON ' || __outname || '(sec_length);
                CREATE INDEX ' || quote_ident(_reqoutname || '_imglocation_idx') || ' ON ' || __outname || '(imglocation);';

      IF _usert = TRUE THEN
        _stmt := _stmt || 'CREATE INDEX ' || quote_ident(_reqoutname || '_tsrange_idx') || ' ON ' || __outname || ' USING GIST ( public.tsrange(rt_start, sec_length) );';
      END IF;
      
      _stmt := _stmt || 'CREATE TRIGGER ' || quote_ident(_reqoutname || '_provide_realtime') ||
               '  BEFORE INSERT OR UPDATE
                  ON ' || __outname ||
               '  FOR EACH ROW
                  EXECUTE PROCEDURE public.trg_interval_provide_realtime();';


    -- process output table doesn't exist
    ELSE
    
      -- check if sequence exists
      EXECUTE 'SELECT COUNT(*)
               FROM pg_catalog.pg_class
               WHERE relname = ' || quote_literal(_reqoutname || '_id_seq') ||
              '  AND relkind = ''S'';'
          INTO _controlcount;

      IF _controlcount = 0 THEN
        RAISE EXCEPTION 'Column named "id" also defined as a sequence is missing.';
      END IF;
    
      EXECUTE 'SELECT ''seqname'' AS attname, ''NAME''::regtype AS attypid
               UNION SELECT ''prsname'', ''NAME''::regtype
               UNION SELECT ''t1'', ''INT''::regtype
               UNION SELECT ''t2'', ''INT''::regtype
               UNION SELECT ''sec_length'', ''REAL''::regtype --trigger supplied
               UNION SELECT ''imglocation'', ''VARCHAR''::regtype
               UNION SELECT ''created'', ''TIMESTAMP WITHOUT TIME ZONE''::regtype
               EXCEPT
               SELECT attname, atttypid
               FROM pg_catalog.pg_attribute
               WHERE attrelid = ' || quote_literal(_dsname || '.' || _reqoutname) || '::regclass::oid
                 AND attstattarget = -1'
          INTO _keyname, _typname;
      
      IF _keyname IS NOT NULL THEN
        RAISE EXCEPTION 'It seems, that "%" is not a process'' output table (column named "%" of "%" data type is missing).', _dsname || '.' || _reqoutname, _keyname, _typname;
      END IF;
      
      EXECUTE 'SELECT array_agg(indexrelid::regclass::varchar)
               FROM pg_catalog.pg_index
               WHERE indrelid = ' || quote_literal(__outname) || '::regclass;'
          INTO _idxnames;
      
      IF _usert = TRUE THEN
        _tmpstmt := 'SELECT ''rt_start'', ''TIMESTAMP WITHOUT TIME ZONE'', FALSE, FALSE, NULL  -- trigger supplied
                     UNION ';
        
        IF EXECUTE 'SELECT TRUE WHERE ' || quote_literal(_idxprefix || _reqoutname || '_tsrange_idx') || ' = ANY(' || quote_literal(_idxnames) || ');' THEN
          _idxstmt := 'CREATE INDEX ' || quote_ident(_reqoutname || '_tsrange_idx') || ' ON ' || __outname || ' USING GIST ( public.tsrange(rt_start, sec_length) );';
        END IF;
      END IF;

      FOR _keyname, _typname, _required, _indexedkey, _indexedparts IN EXECUTE _tmpstmt || ' SELECT keyname, typname, required, indexedkey, indexedparts FROM public.methods_keys WHERE inout = ''out'' AND mtname = ' || quote_literal(_mtname) || ' ORDER BY 3 DESC;' LOOP
        EXECUTE 'SELECT atttypid, attnotnull
                 FROM pg_catalog.pg_attribute
                 WHERE attrelid = ' || quote_literal(_dsname || '.' || _reqoutname) || '::regclass::oid
                   AND attname = ' || quote_literal(_keyname) || '
                   AND attstattarget = -1'
            INTO _typname2, _required2;

        IF _typname2 IS NULL THEN
          _stmt := _stmt || ' ADD ' || quote_ident(_keyname) || '   ' || _typname;
          IF _required = TRUE THEN
            _stmt := _stmt || ' NOT NULL';
          END IF;
          _stmt := _stmt || ', ';
        ELSIF _typname <> _typname2 THEN
          RAISE EXCEPTION 'Column named "%" already exists, but is of different data type (existing: "%", requested: "%").', _keyname, _typname, _typname2;
        ELSIF _required <> required2 THEN
          _stmt := _stmt || ' ALTER COLUMN ' || quote_ident(_keyname) || ' SET NOT NULL, ';
        END IF;

        IF _indexedkey = TRUE THEN
          EXECUTE 'SELECT COUNT(*) WHERE ' || quote_literal(_idxprefix || _reqoutname || '_' || _keyname || '_idx') || ' = ANY(' || quote_literal(_idxnames) || ');' INTO _controlcount;
          IF _controlcount = 0 THEN
            _idxstmt := _idxstmt || public.VT_process_output_idxquery(_reqoutname, _keyname, _typname, NULL, _dsname);
          END IF;
        END IF;

        IF _indexedparts IS NOT NULL THEN
          FOREACH _i IN ARRAY _indexedparts LOOP
            EXECUTE 'SELECT attname FROM pg_catalog.pg_attribute WHERE attrelid = (SELECT oid FROM pg_catalog.pg_class WHERE reltype = ' || quote_literal(_typname) || '::regtype AND relkind = ''c'') AND attnum = ' || _i INTO _tmpstmt;
            EXECUTE 'SELECT COUNT(*) WHERE ' || quote_literal(_idxprefix || _reqoutname || '_' || _keyname || '_' || _tmpstmt || '_idx') || ' = ANY(' || quote_literal(_idxnames) || ');' INTO _controlcount;
            IF _controlcount = 0 THEN
              _idxstmt := _idxstmt || public.VT_process_output_idxquery(_reqoutname, _keyname, _typname, _i, _dsname);
            END IF;
          END LOOP;
        END IF;
      END LOOP;

      IF _stmt <> '' THEN
        _stmt := 'ALTER TABLE ' || __outname || rtrim(_stmt, ', ');
      END IF;

    END IF;

    EXECUTE _stmt;
    EXECUTE _idxstmt;
    
    RETURN TRUE;

    EXCEPTION WHEN OTHERS THEN
      RAISE EXCEPTION 'Some problem occured during the creation of the process'' output table "%". (Details: ERROR %: %)', __outname, SQLSTATE, SQLERRM;
  END;
  $VT_method_output_create$
  LANGUAGE plpgsql CALLED ON NULL INPUT;



-- PROCESS OUTPUT: DROP
-- Function args:
--   * _name - name of method or directly process' output table name
--   * _isnamedbymethod - determines if "_name" is method name or directly process' output table name (default TRUE => _name is method name)
--   * _dsname - name of dataset where will be dropped process' output table (optional)
-- Function behavior:
--   * Successful deletion of a process' output table => returns TRUE
--   * Process'output table with the given name (or derived from method name) is no longer available => returns FALSE
--   * Some error in statement => INTERRUPTED with statement ERROR/EXCEPTION
CREATE OR REPLACE FUNCTION VT_process_output_drop (_name VARCHAR, _isnamebymethod BOOLEAN DEFAULT TRUE, _dsname VARCHAR DEFAULT NULL)
  RETURNS BOOLEAN AS
  $VT_method_output_drop$
  DECLARE
    _controlcount   INT;
    __outname       VARCHAR;
  BEGIN
    IF _dsname IN ('public', 'pg_catalog') THEN
      RAISE EXCEPTION 'Usage of the name "%" for dataset name is not allowed!', _dsname; 
    END IF;

    IF _isnamebymethod = TRUE THEN
      _name := _name || '_out';
    END IF;
    
    IF _dsname IS NULL THEN
      _dsname := current_schema();
    END IF;
    
    __outname := quote_ident(_dsname) || '.' || quote_ident(_name);
    
    EXECUTE 'SELECT COUNT(*)
             FROM pg_catalog.pg_class
             WHERE relname = ' || quote_literal(_name) || '
               AND relnamespace = (
                 SELECT oid
                 FROM pg_catalog.pg_namespace
                 WHERE nspname = ' || quote_literal(_dsname) || '
               )
               AND relkind = ''r''; '
        INTO _controlcount; 
    IF _controlcount <> 1 THEN
      RAISE NOTICE 'Process'' output table "%" can not be dropped due to it is no longer available.', __outname;
      RETURN FALSE;
    END IF;
    
    EXECUTE 'DROP TABLE ' || __outname || ' CASCADE; ';
    RETURN TRUE;

    EXCEPTION WHEN OTHERS THEN
      RAISE EXCEPTION 'Some problem occured during the removal of the process'' output table "%.%". (Details: ERROR %: %)', __outname, SQLSTATE, SQLERRM;    
  END;
  $VT_method_output_drop$
  LANGUAGE plpgsql CALLED ON NULL INPUT;



-- PROCESS OUTPUT: support for index creation
-- Function behavior:
--   * Returns SQL command for index creation
CREATE OR REPLACE FUNCTION VT_process_output_idxquery(_tblname VARCHAR, _keyname NAME, _typname REGTYPE, _keypart INT DEFAULT NULL, _dsname VARCHAR DEFAULT NULL)
  RETURNS VARCHAR AS
  $VT_process_output_idxquery$
  DECLARE
    _stmt      VARCHAR   DEFAULT '';
    _goid      OID;
    _idxname   VARCHAR   DEFAULT '';
    __idxcol    VARCHAR   DEFAULT '';
  BEGIN
    IF _dsname IS NULL THEN
      _dsname := current_schema();
    END IF;

    __idxcol := quote_ident(_keyname);
    
    IF _keypart IS NOT NULL THEN
      EXECUTE 'SELECT attname, atttypid::regtype FROM pg_catalog.pg_attribute WHERE attrelid = (SELECT oid FROM pg_catalog.pg_class WHERE reltype = ' || quote_literal(_typname) || '::regtype AND relkind = ''c'') AND attnum = ' || _keypart INTO __idxcol, _typname;
      _idxname := '_' || __idxcol;
      __idxcol := '( (' || quote_ident(_keyname) || ').' || quote_ident(__idxcol) || ' )';
    END IF;

    _idxname := _tblname || '_' || _keyname || _idxname || '_idx';
    
    -- TODO complex index
    _goid := oid FROM pg_catalog.pg_type WHERE typname = 'geometry';
    IF _typname = 'box'::regtype OR (_goid IS NOT NULL AND _typname = _goid) THEN
      _stmt := 'USING GIST';
    END IF;
    
    -- TODO GIST index 4 3D geometry?
    -- TODO _idxtype & _idxops - is it needed & useful?
    RETURN 'CREATE INDEX ' || quote_ident(_idxname) || ' ON ' || quote_ident(_dsname) || '.' || quote_ident(_tblname) || ' ' || _stmt || ' (' || __idxcol || ');';
    
    EXCEPTION WHEN OTHERS THEN
      RAISE WARNING 'It seems, that indexing of "%" type (above key "%") is not supported by VTApi at this moment. If you would like to index this key, try to contact VTApi team. (Details: ERROR %: %)', _typname, _keyname, SQLSTATE, SQLERRM;
      RETURN '';
  END;
  $VT_process_output_idxquery$
  LANGUAGE plpgsql CALLED ON NULL INPUT;



-------------------------------------
-- Functions to work with real time
-------------------------------------
CREATE OR REPLACE FUNCTION tsrange (_rt_start TIMESTAMP WITHOUT TIME ZONE, _sec_length REAL)
  RETURNS TSRANGE AS
  $tsrange$
    SELECT CASE _rt_start
      WHEN NULL THEN NULL
      ELSE tsrange(_rt_start, _rt_start + _sec_length * '1 second'::interval, '[]')
    END;
  $tsrange$
  LANGUAGE SQL;


CREATE OR REPLACE FUNCTION trg_interval_provide_realtime ()
  RETURNS TRIGGER AS
  $trg_interval_provide_realtime$
    DECLARE
      _fps real := NULL;
      _speed real := NULL;
      _rt_start timestamp without time zone := NULL;
      _tabname name := NULL;
    BEGIN
      IF TG_OP = 'INSERT' OR (TG_OP = 'UPDATE' AND (OLD.t1 <> NEW.t1 OR OLD.t2 <> NEW.t2 OR OLD.rt_start <> NEW.rt_start OR OLD.sec_length <> NEW.sec_length)) THEN  
        _tabname := quote_ident(TG_TABLE_SCHEMA) || '.sequences';
        EXECUTE 'SELECT vid_fps, vid_speed, vid_time
                   FROM '
                   || _tabname::regclass
                   || ' WHERE seqname = $1.seqname;'
          INTO _fps, _speed, _rt_start
          USING NEW;
        IF _fps = 0 OR _speed = 0 THEN
          _fps = NULL;
        ELSE
          _fps = _fps / _speed;
        END IF;

        NEW.sec_length := (NEW.t2 - NEW.t1) / _fps;
        NEW.rt_start := _rt_start + (NEW.t1 / _fps) * '1 second'::interval;
      END IF;
      RETURN NEW;
    END;
  $trg_interval_provide_realtime$
  LANGUAGE PLPGSQL;