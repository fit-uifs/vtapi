--
-- Create DB ### and user $$$ owning the DB
--

-- sudo -u postgres createuser -S -D -R -P $$$
-- CREATE ROLE $$$ PASSWORD 'md50000000000000000000000000000000' NOSUPERUSER NOCREATEDB NOCREATEROLE INHERIT LOGIN;

-- sudo -u postgres createdb -e -O vidte vidte
-- CREATE DATABASE ### OWNER $$$;

-- logout and login as $$$


--
-- PostgreSQL database schema
--


SET statement_timeout = 0;
SET client_encoding = 'SQL_ASCII';
SET standard_conforming_strings = on;
SET check_function_bodies = false;
SET client_min_messages = warning;

SET search_path = public, pg_catalog;

SET default_with_oids = false;


-- create public schema
CREATE SCHEMA IF NOT EXISTS public;

GRANT ALL ON SCHEMA public TO postgres;
GRANT ALL ON SCHEMA public TO PUBLIC;


-- user-defined data types
CREATE TYPE seqtype AS ENUM
   ('video',
    'images',
    'data');

CREATE TYPE inouttype AS ENUM
   ('in',
    'out',
    'in_param',
    'out_param'
    );

CREATE TYPE cvmat AS
   (type integer,
    dims integer[],
    data bytea);

CREATE TYPE vtevent AS
   (group_id integer,
    class_id integer,
    is_root boolean,
    region box,
    score double precision,
    data bytea);

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
    default_num numeric[],
    default_str varchar[],
    CONSTRAINT methods_keys_pk PRIMARY KEY (mtname, keyname),
    CONSTRAINT methods_keys_mtname_fkey FOREIGN KEY (mtname)
      REFERENCES methods(mtname)
);

