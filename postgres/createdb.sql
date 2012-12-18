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

--
-- Name: inouttype; Type: ENUM TYPE; Schema: public; Owner: -
-- 
CREATE TYPE inouttype AS ENUM
   ('in',
    'inout',
    'out');

--
-- Name: seqtype; Type: ENUM TYPE; Schema: public; Owner: -
-- 
CREATE TYPE seqtype AS ENUM
   ('video',
    'images',
    'data');

--
-- Name: datasets; Type: TABLE; Schema: public; Owner: -
--
CREATE TABLE datasets (
    dsname name NOT NULL,
    dslocation character varying,
    userid name,
    groupid name,
    created timestamp without time zone DEFAULT now(),
    changed timestamp without time zone,
    notes text
);


--
-- Name: methods; Type: TABLE; Schema: public; Owner: -
--
CREATE TABLE methods (
    mtname name NOT NULL,
    userid name,
    created timestamp without time zone DEFAULT now(),
    notes text
);


--
-- Name: methods_keys; Type: TABLE; Schema: public; Owner: -
--
CREATE TABLE methods_keys (
    mtname name NOT NULL,
    keyname name NOT NULL,
    typname regtype NOT NULL,
    "inout" inouttype NOT NULL
);


--
-- Name: processes; Type: TABLE; Schema: public; Owner: -
--
CREATE TABLE processes (
    mtname name,
    prsname name NOT NULL,
    inputs regclass,
    outputs regclass,
    userid name,
    created timestamp without time zone,
    notes text
);


--
-- Name: selections; Type: TABLE; Schema: public; Owner: -
--
CREATE TABLE selections (
    selname regclass NOT NULL,
    dataset character varying NOT NULL,
    userid name,
    created timestamp without time zone DEFAULT now(),
    notes text
);



--
-- Name: dataset_pk; Type: CONSTRAINT; Schema: public; Owner: -
--
ALTER TABLE ONLY datasets
    ADD CONSTRAINT dataset_pk PRIMARY KEY (dsname);


--
-- Name: methods_keys_pk; Type: CONSTRAINT; Schema: public; Owner: -
--
ALTER TABLE ONLY methods_keys
    ADD CONSTRAINT methods_keys_pk PRIMARY KEY (mtname, keyname);


--
-- Name: methods_pk; Type: CONSTRAINT; Schema: public; Owner: -
--
ALTER TABLE ONLY methods
    ADD CONSTRAINT methods_pk PRIMARY KEY (mtname);


--
-- Name: processes_pk; Type: CONSTRAINT; Schema: public; Owner: -
--
ALTER TABLE ONLY processes
    ADD CONSTRAINT processes_pk PRIMARY KEY (prsname);


--
-- Name: selections_pk; Type: CONSTRAINT; Schema: public; Owner: -
--
ALTER TABLE ONLY selections
    ADD CONSTRAINT selections_pk PRIMARY KEY (selname);


--
-- Name: method_fk; Type: FK CONSTRAINT; Schema: public; Owner: -
--
ALTER TABLE ONLY processes
    ADD CONSTRAINT method_fk FOREIGN KEY (mtname) REFERENCES methods(mtname) ON UPDATE CASCADE ON DELETE RESTRICT;


--
-- Name: methods_keys_mtname_fkey; Type: FK CONSTRAINT; Schema: public; Owner: -
--
ALTER TABLE ONLY methods_keys
    ADD CONSTRAINT methods_keys_mtname_fkey FOREIGN KEY (mtname) REFERENCES methods(mtname);

--
-- PostgreSQL database schema complete
--


