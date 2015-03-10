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
DROP TABLE IF EXISTS public.methods_keys;
DROP TABLE IF EXISTS public.methods;
DROP TABLE IF EXISTS public.datasets;
DROP TYPE IF EXISTS public.seqtype;
DROP TYPE IF EXISTS public.inouttype;
DROP TYPE IF EXISTS public.cvmat;
DROP TYPE IF EXISTS public.vtevent;

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
    id serial NOT NULL,
    mtname name NOT NULL,
    userid name,
    created timestamp without time zone DEFAULT now(),
    notes text,
    CONSTRAINT methods_pk PRIMARY KEY (id),
    CONSTRAINT mtname_unq UNIQUE (mtname)
);

-- methods parameters definition
CREATE TABLE methods_keys (
    mtid integer NOT NULL,
    keyname name NOT NULL,
    typname regtype NOT NULL,
    inout inouttype NOT NULL,
    default_num numeric[],
    default_str varchar[],
    CONSTRAINT methods_keys_pk PRIMARY KEY (mtid, keyname),
    CONSTRAINT mtid_fk FOREIGN KEY (mtid)
      REFERENCES methods(id) ON UPDATE CASCADE ON DELETE CASCADE
);
