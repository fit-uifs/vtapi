--
-- Create schema test
--

-- for example see example/sql/demo-schema.sql

-- replace all instances of "test" with the schema_name!
-- specify your own intervals table
-- specify the dataset location within your datasets (last line)

-- work as the database user created by createdb

--
-- PostgreSQL database schema test
--


SET statement_timeout = 0;
SET lock_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SET check_function_bodies = false;
SET client_min_messages = warning;
SET default_with_oids = false;

SET search_path = test, public, pg_catalog;
      

-- create custom schema "test"
CREATE schema test;

GRANT ALL ON SCHEMA public TO vidte;
GRANT ALL ON SCHEMA public TO postgres;

-- sequence number
CREATE SEQUENCE seqnum_sequence
  INCREMENT 1
  MINVALUE 1
  MAXVALUE 9223372036854775807
  START 1
  CACHE 1;

-- create table for sequences (video/imagefolders)
CREATE TABLE sequences (
    seqname name NOT NULL,
    seqnum integer DEFAULT nextval('seqnum_sequence'::regclass),
    seqlocation character varying,
    seqtyp public.seqtype DEFAULT 'data',
    vid_length integer,
    vid_fps float,
    userid name,
    created timestamp without time zone DEFAULT now(),
    notes text,
    CONSTRAINT sequences_pk PRIMARY KEY (seqname),
    CONSTRAINT seqnum_unq UNIQUE (seqnum)
);
CREATE INDEX sequences_seqtyp_idx ON sequences(seqtyp);

-- create table for processes
CREATE TABLE processes (
    mtname name,
    prsname name NOT NULL,
    outputs regclass,
    inputs character varying,
    params character varying,
    userid name,
    created timestamp without time zone DEFAULT now(),
    notes text,
    CONSTRAINT processes_pk PRIMARY KEY (prsname),
    CONSTRAINT mtname_fk FOREIGN KEY (mtname)
      REFERENCES public.methods(mtname) ON UPDATE CASCADE ON DELETE RESTRICT
);
ALTER TABLE ONLY processes
    ADD CONSTRAINT inputs_fk FOREIGN KEY (inputs)
        REFERENCES processes(prsname) ON UPDATE CASCADE ON DELETE RESTRICT;
CREATE INDEX processes_mtname_idx ON processes(mtname);
CREATE INDEX processes_inputs_idx ON processes(inputs);

-- interval number
CREATE SEQUENCE interval_sequence
  INCREMENT 1
  MINVALUE 1
  MAXVALUE 9223372036854775807
  START 1
  CACHE 1;

-- create table for intervals (images/frames)
-- replace column "event" with 0-N columns with result data or leave as is
CREATE TABLE intervals (
    id integer NOT NULL DEFAULT nextval('interval_sequence'::regclass),
    seqname character varying NOT NULL,
    process character varying,
    t1 integer NOT NULL,
    t2 integer NOT NULL,
    imglocation character varying,
    event public.vtevent,
    userid name,
    created timestamp without time zone DEFAULT now(),
    notes text,
    CONSTRAINT intervals_pk PRIMARY KEY (id),
    CONSTRAINT seqname_fk FOREIGN KEY (seqname)
      REFERENCES sequences(seqname) ON UPDATE CASCADE ON DELETE RESTRICT,
    CONSTRAINT process_fk FOREIGN KEY (process)
      REFERENCES processes(prsname) ON UPDATE CASCADE ON DELETE RESTRICT
);
CREATE INDEX intervals_seqname_idx ON intervals(seqname);
CREATE INDEX intervals_process_idx ON intervals(process);

-- insert into dataset list
INSERT INTO public.datasets(dsname, dslocation, userid, groupid, notes)
    VALUES ('test', 'data/test/', 'testuser', 'testgroup', 'testovaci dataset');



