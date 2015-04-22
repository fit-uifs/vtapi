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
GRANT ALL ON SCHEMA public TO postgres;

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

DROP FUNCTION IF EXISTS public.tsrange(timestamp without time zone, real) CASCADE;
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
    'init',     -- process has been created
    'started',  -- process has been started in the past, hasn't finished and is not running
    'running',  -- process is currently working
    'done',     -- process has finished succesfully
    'error'     -- process has finished with error
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
    error_msg varchar       -- error message
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
-- CREATE functions
-------------------------------------
CREATE OR REPLACE FUNCTION tsrange(IN rt_start timestamp without time zone, IN sec_length real)
  RETURNS tsrange AS
  $tsrange$
    SELECT CASE rt_start
      WHEN NULL THEN NULL
      ELSE tsrange(rt_start, rt_start + sec_length * '1 second'::interval, '[]')
    END
  $tsrange$
  LANGUAGE SQL;


CREATE OR REPLACE FUNCTION trg_interval_provide_realtime()
  RETURNS TRIGGER AS
  $trg_interval_provide_realtime$
    DECLARE
      fps real := NULL;
      speed real := NULL;
      rt_start timestamp without time zone := NULL;
      tabname name := NULL;
    BEGIN
      IF TG_OP = 'INSERT' OR (TG_OP = 'UPDATE' AND (OLD.t1 <> NEW.t1 OR OLD.t2 <> NEW.t2)) THEN  
        tabname := quote_ident(TG_TABLE_SCHEMA) || '.sequences';
        EXECUTE 'SELECT vid_fps, vid_speed, vid_time
                   FROM '
                   || tabname::regclass
                   || ' WHERE seqname = $1.seqname;'
          INTO fps, speed, rt_start
          USING NEW;
        IF fps = 0 OR speed = 0 THEN
          fps = NULL;
        ELSE
          fps = fps / speed;
        END IF;

        NEW.sec_length := (NEW.t2 - NEW.t1) / fps;
        NEW.rt_start := rt_start + (NEW.t1 / fps) * '1 second'::interval;
      END IF;
      RETURN NEW;
    END;
  $trg_interval_provide_realtime$
  LANGUAGE PLPGSQL;

