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
SELECT VT_dataset_drop_all();

DROP TABLE IF EXISTS public.methods_keys CASCADE;
DROP TABLE IF EXISTS public.methods CASCADE;
DROP TABLE IF EXISTS public.datasets CASCADE;

DROP TYPE IF EXISTS public.seqtype CASCADE;
DROP TYPE IF EXISTS public.inouttype CASCADE;
DROP TYPE IF EXISTS public.METHODKEY CASCADE;
DROP TYPE IF EXISTS public.pstatus CASCADE;
DROP TYPE IF EXISTS public.cvmat CASCADE;
DROP TYPE IF EXISTS public.vtevent CASCADE;
DROP TYPE IF EXISTS public.pstate CASCADE;

DROP FUNCTION IF EXISTS public.VT_dataset_create(VARCHAR, VARCHAR, TEXT) CASCADE;
DROP FUNCTION IF EXISTS public.VT_dataset_drop(VARCHAR) CASCADE;
DROP FUNCTION IF EXISTS public.VT_dataset_truncate(VARCHAR) CASCADE;
DROP FUNCTION IF EXISTS public.VT_dataset_support_create(VARCHAR) CASCADE;

DROP FUNCTION IF EXISTS public.VT_method_add(VARCHAR, METHODKEY, TEXT) CASCADE;
DROP FUNCTION IF EXISTS public.VT_method_delete(VARCHAR) CASCADE;

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

-- method parameter type
CREATE TYPE inouttype AS ENUM (
    'in_param',     -- input parameter (numeric/string)
    'in',           -- input = column from other method's processes' output table
    'out'           -- output = column from this method's processes' output table
);

CREATE TYPE METHODKEY AS (
    keyname       NAME,      -- variable name
    typname       REGTYPE,   -- variable data type
    inout         INOUTTYPE, -- variable scope - determines which kind variable is: common input, input from other process' output table or output in current process' output table
    default_num   DOUBLE PRECISION[], -- default numeric value
    default_str   VARCHAR[]  -- default character value
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
    dsname name NOT NULL,
    dslocation character varying NOT NULL,
    userid name DEFAULT NULL,
    created timestamp without time zone DEFAULT now(),
    notes text DEFAULT NULL,
    CONSTRAINT dataset_pk PRIMARY KEY (dsname)
);

-- method list
CREATE TABLE methods (
    mtname name NOT NULL,
    userid name DEFAULT NULL,
    created timestamp without time zone DEFAULT now(),
    notes text DEFAULT NULL,
    CONSTRAINT methods_pk PRIMARY KEY (mtname)
);

-- methods parameters definition
CREATE TABLE methods_keys (
    mtname name NOT NULL,
    keyname name NOT NULL,
    typname regtype NOT NULL,
    inout inouttype NOT NULL,
    default_num double precision[],
    default_str varchar[],
    CONSTRAINT methods_keys_pk PRIMARY KEY (mtname, keyname),
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
--   * Dataset is already registered, but _dslocation or _dsnotes is different than in DB => returns -1
--   * There already exist some parts of dataset and some parts not exist => INTERRUPTED with INCONSISTENCY EXCEPTION
--   * Some error in statement (ie. CREATE, DROP, INSERT, ..) => INTERRUPTED with statement ERROR/EXCEPTION
CREATE OR REPLACE FUNCTION VT_dataset_create (_dsname VARCHAR, _dslocation VARCHAR, _dsnotes TEXT DEFAULT NULL) 
  RETURNS INT AS 
  $VT_dataset_create$
  DECLARE
    _dsnamecount INT;
    _dsidentical INT;
    _schemacount INT;
    _classescount INT;
    
    _stmt VARCHAR;
  BEGIN
    EXECUTE 'SELECT COUNT(*)
             FROM public.datasets
             WHERE dsname = ' || quote_literal(_dsname)
      INTO _dsnamecount;
    EXECUTE 'SELECT COUNT(*)
             FROM pg_namespace
             WHERE nspname = ' || quote_literal(_dsname)
      INTO _schemacount;

    _stmt := 'SELECT COUNT(*)
               FROM public.datasets
               WHERE dsname = ' || quote_literal(_dsname) || '
                 AND dslocation = ' || quote_literal(_dslocation) || '
                 AND notes ';
    IF _dsnotes IS NULL THEN
      _stmt := _stmt || ' IS NULL;';
    ELSE 
      _stmt := _stmt || ' = ' || quote_literal(_dsnotes);
    END IF;
    EXECUTE _stmt INTO _dsidentical;
    
    IF _dsnamecount <> _dsidentical THEN
      RAISE WARNING 'Dataset "%" already exists, but has different value in "dslocation" or/and in "notes" property.', _dsname;
      RETURN -1;
    END IF;

    -- check if schema already exists
    IF _schemacount = 1 THEN
      EXECUTE 'SELECT COUNT(*)
               FROM pg_class
               WHERE relname IN (''sequences'', ''processes'', ''sequences_pk'', ''sequences_seqtyp_idx'', ''processes_pk'', ''processes_mtname_idx'', ''processes_inputs_idx'', ''processes_status_idx'')
               AND relnamespace = (
                 SELECT oid
                 FROM pg_namespace
                 WHERE nspname = ' || quote_literal(_dsname) || '
               );'
        INTO _classescount;

      -- check inconsistence || already existing dataset
      IF _dsnamecount <> 1 OR _classescount <> 8 THEN
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
      IF _dsnotes IS NOT NULL THEN
        EXECUTE 'INSERT INTO public.datasets(dsname, dslocation, notes)
                 VALUES (' || quote_literal(_dsname) || ', ' || quote_literal(_dslocation) || ', ' || quote_literal(_dsnotes) || ');';
      ELSE 
        EXECUTE 'INSERT INTO public.datasets(dsname, dslocation)
                 VALUES (' || quote_literal(_dsname) || ', ' || quote_literal(_dslocation) || ');';
      END IF;
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
    _dsnamecount INT;
    _schemacount INT;
  BEGIN
    EXECUTE 'SELECT COUNT(*)
             FROM public.datasets
             WHERE dsname = ' || quote_literal(_dsname)
      INTO _dsnamecount;
    EXECUTE 'SELECT COUNT(*)
             FROM pg_namespace
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
    _dsname public.datasets.dsname%TYPE;
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
    _dsnamecount INT;
    _schemacount INT;
  BEGIN
    EXECUTE 'SELECT COUNT(*)
             FROM public.datasets
             WHERE dsname = ' || quote_literal(_dsname)
      INTO _dsnamecount;
    EXECUTE 'SELECT COUNT(*)
             FROM pg_namespace
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
    _schemacount INT;
  BEGIN
    EXECUTE 'SELECT COUNT(*)
             FROM pg_namespace
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
      userid name DEFAULT NULL,
      created timestamp without time zone DEFAULT now(),
      notes text DEFAULT NULL,
      CONSTRAINT sequences_pk PRIMARY KEY (seqname)
    );
    CREATE INDEX sequences_seqtyp_idx ON sequences(seqtyp);

    -- table for processes
    CREATE TABLE processes (
      prsname name NOT NULL,
      mtname name NOT NULL,
      inputs name,
      outputs regclass,
      params character varying,
      state public.pstate DEFAULT '(created,0,,)',
      userid name DEFAULT NULL,
      created timestamp without time zone DEFAULT now(),
      notes text DEFAULT NULL,
      CONSTRAINT processes_pk PRIMARY KEY (prsname),
      CONSTRAINT mtname_fk FOREIGN KEY (mtname)
          REFERENCES public.methods(mtname) ON UPDATE CASCADE ON DELETE RESTRICT,
      CONSTRAINT inputs_fk FOREIGN KEY (inputs)
          REFERENCES processes(prsname) ON UPDATE CASCADE ON DELETE RESTRICT
    );
    CREATE INDEX processes_mtname_idx ON processes(mtname);
    CREATE INDEX processes_inputs_idx ON processes(inputs);
    CREATE INDEX processes_status_idx ON processes(( (state).status ));
    
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
CREATE OR REPLACE FUNCTION VT_method_add (_mtname VARCHAR, _mkeys METHODKEY[], _notes TEXT DEFAULT NULL)
  RETURNS BOOLEAN AS
  $VT_method_add$
  DECLARE
    _mtcount INT;
    
    _stmt VARCHAR;
    _inscols VARCHAR DEFAULT '';
    _insvals VARCHAR DEFAULT '';
  BEGIN
    EXECUTE 'SELECT COUNT(*) FROM public.methods WHERE mtname = ' || quote_literal(_mtname) INTO _mtcount;
    IF _mtcount <> 0 THEN
      RETURN FALSE;
    END IF;

    IF _notes IS NOT NULL THEN
      _inscols := ', notes';
      _insvals := ', ' || quote_literal(_notes);
    END IF;
    _stmt := 'INSERT INTO public.methods (mtname' || _inscols || ') VALUES (' || quote_literal(_mtname) || _insvals || ');';
    EXECUTE _stmt;
    
    _insvals := '';
    FOR i IN 1 .. array_upper(_mkeys, 1) LOOP
      IF _mkeys[i].keyname IS NULL
      THEN
        RAISE EXCEPTION 'Method key name ("keyname" property) can not be NULL!';
      END IF;
      
      IF _mkeys[i].typname IS NULL
      THEN
        RAISE EXCEPTION 'Method key type ("typname" property) can not be NULL!';
      END IF;
      
      IF _mkeys[i].inout IS NULL
      THEN
        RAISE EXCEPTION 'Method scope ("inout" property) can not be NULL!';
      END IF;
      
      
      _insvals := _insvals || '(' || quote_literal(_mtname) || ', ' || quote_literal(_mkeys[i].keyname) || ', ' || quote_literal(_mkeys[i].typname) || ', ' || quote_literal(_mkeys[i].inout) || ', ';
      
      IF _mkeys[i].default_num IS NULL THEN
        _insvals := _insvals || 'NULL';
      ELSE
        _insvals := _insvals || quote_literal(_mkeys[i].default_num);
      END IF;
      
      _insvals := _insvals || ', ';
      
      IF _mkeys[i].default_str IS NULL THEN
        _insvals := _insvals || 'NULL';
      ELSE
        _insvals := _insvals || quote_literal(_mkeys[i].default_str);
      END IF;
      
      _insvals := _insvals || '), ';
    END LOOP;
    
    _stmt := 'INSERT INTO public.methods_keys(mtname, keyname, typname, inout, default_num, default_str) VALUES ' || rtrim(_insvals, ', ');
    EXECUTE _stmt;
    
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
--   * Some error in statement => INTERRUPTED with statement ERROR/EXCEPTION
CREATE OR REPLACE FUNCTION VT_method_delete (_mtname VARCHAR)
  RETURNS BOOLEAN AS
  $VT_method_delete$
  DECLARE
    _mtcount INT;
  BEGIN
    -- TODO: what if processes block the deletion??? :(
  
    EXECUTE 'SELECT COUNT(*) FROM public.methods WHERE mtname = ' || quote_literal(_mtname) INTO _mtcount;
    IF _mtcount <> 1 THEN
      RETURN FALSE;
    END IF;
    EXECUTE 'DELETE FROM public.methods WHERE mtname = ' || quote_literal(_mtname);
--    EXECUTE 'DELETE FROM public.methods_keys WHERE mtname = ' || quote_literal(_mtname); -- is not necessary if the "ON DELETE CASCADE" is defined
    RETURN TRUE;
    
    EXCEPTION WHEN OTHERS THEN
      RAISE EXCEPTION 'Some problem occured during the deletion of the method "%". (Details: ERROR %: %)', _mtname, SQLSTATE, SQLERRM;
  END;
  $VT_method_delete$
  LANGUAGE plpgsql STRICT;





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
      IF TG_OP = 'INSERT' OR (TG_OP = 'UPDATE' AND (OLD.t1 <> NEW.t1 OR OLD.t2 <> NEW.t2)) THEN  
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