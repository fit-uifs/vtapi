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
DROP TABLE IF EXISTS public.methods_keys CASCADE;
DROP TABLE IF EXISTS public.methods CASCADE;
DROP TABLE IF EXISTS public.datasets CASCADE;

DROP TYPE IF EXISTS public.seqtype CASCADE;
DROP TYPE IF EXISTS public.inouttype CASCADE;
DROP TYPE IF EXISTS public.pstatus CASCADE;
DROP TYPE IF EXISTS public.cvmat CASCADE;
DROP TYPE IF EXISTS public.vtevent CASCADE;
DROP TYPE IF EXISTS public.pstate CASCADE;

DROP FUNCTION IF EXISTS public.VT_dataset_create(VARCHAR, VARCHAR, TEXT) CASCADE;
DROP FUNCTION IF EXISTS public.VT_dataset_drop(VARCHAR) CASCADE;
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
    userid name,
    created timestamp without time zone DEFAULT now(),
    notes text,
    CONSTRAINT dataset_pk PRIMARY KEY (dsname)
);                    

-- method list
CREATE TABLE methods (
    mtname name NOT NULL,
    userid name,
    created timestamp without time zone DEFAULT now(),
    notes text,
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
-- Function behavior (proposed, partially implemented):
--   * Successful creation of a dataset => returns TRUE
--   * Whole dataset structures already exist => returns FALSE
--   * There already exist some parts of dataset and some parts not exist => INTERRUPTED with INCONSISTENCY EXCEPTION
--   * Some error in CREATE statement OR INSERT statement => INTERRUPTED with statement ERROR/EXCEPTION
CREATE OR REPLACE FUNCTION VT_dataset_create (_dsname VARCHAR, _dslocation VARCHAR, _dsnotes TEXT DEFAULT NULL) 
  RETURNS BOOLEAN AS 
  $VT_dataset_create$
  BEGIN
--    IF $1 IS NULL THEN  
--      RAISE EXCEPTION 'Dataset name can not be empty.'; 
--    END IF;
--    IF $2 IS NULL THEN  
--      RAISE EXCEPTION 'Dataset location can not be empty.'; 
--    END IF;

    -- FUTURE TODO: check if exists schema && tables & datasets => FALSE
    -- TODO: only some of schema && tables & datasets exists => INCONSISTENCY EXCEPTION (like in drop function)

    -- create schema (dataset)
    EXECUTE 'CREATE SCHEMA ' || quote_ident(_dsname);
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
      userid name,
      created timestamp without time zone DEFAULT now(),
      notes text,
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
      userid name,
      created timestamp without time zone DEFAULT now(),
      notes text,
      CONSTRAINT processes_pk PRIMARY KEY (prsname),
      CONSTRAINT mtname_fk FOREIGN KEY (mtname)
          REFERENCES public.methods(mtname) ON UPDATE CASCADE ON DELETE RESTRICT,
      CONSTRAINT inputs_fk FOREIGN KEY (inputs)
          REFERENCES processes(prsname) ON UPDATE CASCADE ON DELETE RESTRICT
    );
    CREATE INDEX processes_mtname_idx ON processes(mtname);
    CREATE INDEX processes_inputs_idx ON processes(inputs);
    CREATE INDEX processes_status_idx ON processes(( (state).status ));

    -- register dataset
    IF _dsnotes IS NOT NULL THEN
      INSERT INTO public.datasets(dsname, dslocation, notes) VALUES (_dsname, _dslocation, _dsnotes);
    ELSE 
      INSERT INTO public.datasets(dsname, dslocation) VALUES (_dsname, _dslocation);
    END IF;
    
    RETURN TRUE;
  END;
  $VT_dataset_create$
  LANGUAGE plpgsql CALLED ON NULL INPUT;


-- DATASET: drop (delete)
-- Function behavior:
--   * Successful removal of a dataset => returns TRUE
--   * Whole dataset structures are no longer available => returns FALSE
--   * There already exist some parts of dataset and some parts not exist => raises notice of INCONSISTENCY and continue
--   * Some error in CREATE statement OR INSERT statement => INTERRUPTED with statement ERROR/EXCEPTION
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

    IF _dsnamecount <> _schemacount THEN
      RAISE NOTICE 'Inconsistency was detected in datasets.';
    ELSIF _dsnamecount = 0 THEN
      RETURN FALSE;
    END IF;

    EXECUTE 'DELETE FROM public.datasets WHERE dsname = ' || quote_literal(_dsname);
    EXECUTE 'DROP SCHEMA ' || quote_ident(_dsname) || ' CASCADE';
    RETURN TRUE;
  EXCEPTION WHEN OTHERS THEN
    RAISE EXCEPTION 'Some problem occured during the removal of the desired dataset. (Details: ERROR %: %)', SQLSTATE, SQLERRM;
  END
  $VT_dataset_drop$
  LANGUAGE plpgsql STRICT;



-------------------------------------
-- Functions to work with real time
-------------------------------------
CREATE OR REPLACE FUNCTION tsrange(_rt_start TIMESTAMP WITHOUT TIME ZONE, _sec_length REAL)
  RETURNS TSRANGE AS
  $tsrange$
    SELECT CASE _rt_start
      WHEN NULL THEN NULL
      ELSE tsrange(_rt_start, _rt_start + _sec_length * '1 second'::interval, '[]')
    END
  $tsrange$
  LANGUAGE SQL;


CREATE OR REPLACE FUNCTION trg_interval_provide_realtime()
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