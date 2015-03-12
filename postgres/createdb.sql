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
DROP TYPE IF EXISTS public.cvmat CASCADE;
DROP TYPE IF EXISTS public.vtevent CASCADE;

-------------------------------------
-- CREATE user-defined data types
-------------------------------------

-- sequence type
CREATE TYPE seqtype AS ENUM
   ('video',
    'images',
    'data');

-- method parameter type
CREATE TYPE inouttype AS ENUM
   ('in',
    'out',
    'in_param',
    'out_param'
    );

-- OpenCV matrix type
CREATE TYPE cvmat AS
   (type integer,
    dims integer[],
    data bytea);

-- VTApi event type
CREATE TYPE vtevent AS
   (group_id integer,
    class_id integer,
    is_root boolean,
    region box,
    score double precision,
    data bytea);

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
