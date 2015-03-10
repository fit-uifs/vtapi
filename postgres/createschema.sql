-- Create schema test - always modify this appropriately for new dataset
-- For example see example/sql/demo-schema.sql
--
-- replace all instances of "test" with the schema_name!
-- specify your own intervals table
-- specify the dataset location within your datasets (last line)
--
-- authors: ifroml[at]fit.vutbr.cz; ivolf[at]fit.vutbr.cz

SET statement_timeout = 0;
SET lock_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SET check_function_bodies = false;
SET client_min_messages = warning;
SET default_with_oids = false;
SET search_path = test, pg_catalog;
      

-------------------------------------
-- DROP and CREATE test schema
-------------------------------------

DROP SCHEMA IF EXISTS test CASCADE;
CREATE schema test;
GRANT ALL ON SCHEMA test TO postgres;

-------------------------------------
-- CREATE common tables, don't change
-------------------------------------

-- table for sequences (video/imagefolders)
CREATE TABLE sequences (
    id serial NOT NULL,
    seqname name NOT NULL,
    seqlocation character varying,
    seqtyp public.seqtype,
    userid name,
    created timestamp without time zone DEFAULT now(),
    notes text,
    CONSTRAINT sequences_pk PRIMARY KEY (id),
    CONSTRAINT seqnum_unq UNIQUE (seqname)
);
CREATE INDEX sequences_seqtyp_idx ON sequences(seqtyp);

-- table for processes
CREATE TABLE processes (
    id serial NOT NULL,
    mtid integer NOT NULL,
    prsname name NOT NULL,
    prsid_inputs integer,
    outputs regclass,
    params character varying,
    userid name,
    created timestamp without time zone DEFAULT now(),
    notes text,
    CONSTRAINT processes_pk PRIMARY KEY (id),
    CONSTRAINT mtid_fk FOREIGN KEY (mtid)
        REFERENCES public.methods(id) ON UPDATE CASCADE ON DELETE RESTRICT,
    CONSTRAINT prsname_unq UNIQUE (prsname)
);
ALTER TABLE ONLY processes
    ADD CONSTRAINT inputs_fk FOREIGN KEY (prsid_inputs)
        REFERENCES processes(id) ON UPDATE CASCADE ON DELETE RESTRICT;
CREATE INDEX processes_mtid_idx ON processes(mtid);
CREATE INDEX processes_prsid_inputs_idx ON processes(prsid_inputs);

-------------------------------------
-- CREATE module-specific tables
--  create one outputs table for every module according to specifications
--  out_* columns are to be replaced here
-------------------------------------

-- intervals table for demo1 results
CREATE TABLE test1out (
    id serial NOT NULL,
    seqid integer NOT NULL,
    prsid integer NOT NULL,
    t1 integer NOT NULL,
    t2 integer NOT NULL,
    out_features_array real[],
    out_features_mat public.cvmat,
    out_event public.vtevent NOT NULL,
    userid name,
    created timestamp without time zone DEFAULT now(),
    notes text,
    CONSTRAINT test1out_pk PRIMARY KEY (id),
    CONSTRAINT seqid_fk FOREIGN KEY (seqid)
      REFERENCES sequences(id) ON UPDATE CASCADE ON DELETE RESTRICT,
    CONSTRAINT prsid_fk FOREIGN KEY (prsid)
      REFERENCES processes(id) ON UPDATE CASCADE ON DELETE RESTRICT
);
CREATE INDEX test1out_seqid_idx ON test1out(seqid);
CREATE INDEX test1out_prsid_idx ON test1out(prsid);

-------------------------------------
-- INSERT schema into dataset list
-------------------------------------

INSERT INTO public.datasets(dsname, dslocation, userid, groupid, notes)
    VALUES ('test', 'data/test/', 'testuser', 'testgroup', 'testovaci dataset');



