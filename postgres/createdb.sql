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


CREATE SCHEMA public;

-- výètové datové typy
CREATE TYPE inouttype AS ENUM ('in', 'inout', 'out'); 
CREATE TYPE seqtype 	AS ENUM ('video', 'images', 'data');

-- tabulka se seznamem datasetù
CREATE TABLE datasets (
    dsname name NOT NULL,
    dslocation character varying,
    userid name,
    groupid name,
    created timestamp without time zone DEFAULT now(),
    changed timestamp without time zone,
    notes text,
    CONSTRAINT dataset_pk PRIMARY KEY (dsname)
);                    

-- tabulka s dostupnými metodami
CREATE TABLE methods (
    mtname name NOT NULL,
    userid name,
    created timestamp without time zone DEFAULT now(),
    notes text,
    CONSTRAINT methods_pk PRIMARY KEY (mtname)
);

-- tabulka s parametry metod
CREATE TABLE methods_keys (
    mtname name NOT NULL,
    keyname name NOT NULL,
    typname regtype NOT NULL,
    "inout" inouttype NOT NULL,
    CONSTRAINT methods_keys_pk PRIMARY KEY (mtname, keyname),
    CONSTRAINT methods_keys_mtname_fkey FOREIGN KEY (mtname)
			REFERENCES methods(mtname)
);

-- tabulka s definovanými procesy
CREATE TABLE processes (
    mtname name,
    prsname name NOT NULL,
    inputs regclass,
    outputs regclass,
    userid name,
    created timestamp without time zone,
    notes text,
    CONSTRAINT processes_pk PRIMARY KEY (prsname),
    CONSTRAINT method_fk FOREIGN KEY (mtname)
			REFERENCES methods(mtname) ON UPDATE CASCADE ON DELETE RESTRICT
);

-- tabulka selekcí
CREATE TABLE selections (
    selname regclass NOT NULL,
    dataset character varying NOT NULL,
    userid name,
    created timestamp without time zone DEFAULT now(),
    notes text,
    CONSTRAINT selections_pk PRIMARY KEY (selname)
);
