--
-- PostgreSQL database dump
--

-- Dumped from database version 9.3.1
-- Dumped by pg_dump version 9.3.5
-- Started on 2015-01-30 08:57:25 CET

SET statement_timeout = 0;
SET lock_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SET check_function_bodies = false;
SET client_min_messages = warning;

SET search_path = demo, pg_catalog;

SET default_tablespace = '';

SET default_with_oids = false;


DROP SCHEMA IF EXISTS demo CASCADE;
CREATE SCHEMA demo
  AUTHORIZATION vidte;

GRANT ALL ON SCHEMA demo TO vidte;
GRANT ALL ON SCHEMA demo TO postgres;
GRANT ALL ON SCHEMA demo TO public;



CREATE TABLE demo1out (
    seqname character varying NOT NULL,
    process character varying,
    t1 integer NOT NULL,
    t2 integer NOT NULL,
    vals real[],
    userid name,
    created timestamp without time zone DEFAULT now(),
    notes text
);


ALTER TABLE demo.demo1out OWNER TO vidte;

CREATE TABLE demo2out (
    seqname character varying NOT NULL,
    process character varying,
    t1 integer NOT NULL,
    t2 integer NOT NULL,
    eventName character varying,
    eventArg integer,
    userid name,
    created timestamp without time zone DEFAULT now(),
    notes text
);


ALTER TABLE demo.demo2out OWNER TO vidte;


CREATE TABLE processes (
    mtname name,
    prsname name NOT NULL,
    outputs regclass,
    inputs character varying,
    params character varying,
    userid name,
    created timestamp without time zone,
    notes text
);


ALTER TABLE demo.processes OWNER TO vidte;

CREATE SEQUENCE seqnum_sequence
  INCREMENT 1
  MINVALUE 1
  MAXVALUE 9223372036854775807
  START 1
  CACHE 1;

ALTER TABLE demo.seqnum_sequence OWNER TO vidte;

CREATE TYPE demo.seqtype AS ENUM
   ('video',
    'images',
    'data');

ALTER TYPE demo.seqtype OWNER TO vidte;

CREATE TABLE sequences (
    seqname name NOT NULL,
    seqnum integer DEFAULT nextval('demo.seqnum_sequence'::regclass),
    seqlocation character varying,
    seqtyp demo.seqtype,
    userid name,
    groupid name,
    created timestamp without time zone,
    changed timestamp without time zone,
    notes text
);


ALTER TABLE demo.sequences OWNER TO vidte;


\copy sequences (seqname, seqlocation, seqtyp, userid, groupid, created, changed, notes) FROM stdin ;
video1	video1.mpg	video	\N	\N	\N	\N	\N
video2	video2.mpg	video	\N	\N	\N	\N	\N
video3	video3.mpg	video	\N	\N	\N	\N	\N
\.

\copy processes (mtname, prsname, outputs, inputs, params, userid, created, notes) FROM stdin ;
demo1	demo1p_5000_25	demo1out	\N	{param1:5000,param2:25}	\N	\N	\N
demo1	demo1p_0_10	demo1out	\N	{param1:0,param2:10}	\N	\N	\N
demo1	demo1p_11_50	demo1out	\N	{param1:11,param2:50}	\N	\N	\N
demo2	demo2p_video3_demo1p_5000_25	demo2out	demo1p_5000_25	{video:video3}	\N	\N	\N
demo2	demo2p_video1_demo1p_11_50	demo2out	demo1p_11_50	{video:video1}	\N	\N	\N
demo2	demo2p_video3_demo1p_11_50	demo2out	demo1p_11_50	{video:video3}	\N	\N	\N
\.

\copy demo1out (seqname, process, t1, t2, vals, userid, created, notes) FROM stdin ;
video1	demo1p_5000_25	1	1	{1.8,2.3,2.5}	\N	\N	\N
video2	demo1p_5000_25	2	1	{1.6}	\N	\N	\N
video3	demo1p_5000_25	1	1	{1.8}	\N	\N	\N
video3	demo1p_5000_25	5	32	{7.9,2.9,0.01}	\N	\N	\N
video3	demo1p_5000_25	7	15	{2.9,0.07,1.5}	\N	\N	\N
\.

\copy demo2out (seqname, process, t1, t2, eventName, eventArg, userid, created, notes) FROM stdin ;
video1	demo2p_video3_demo1p_5000_25	1	1	vid1event1	5	\N	\N	\N
video2	demo2p_video3_demo1p_5000_25	2	1	vid2event1	7	\N	\N	\N
video3	demo2p_video3_demo1p_5000_25	1	1	vid3event1	2	\N	\N	\N
video3	demo2p_video3_demo1p_5000_25	5	32	vid3event2	8	\N	\N	\N
video3	demo2p_video3_demo1p_5000_25	7	15	vid3event3	11	\N	\N	\N
video3	demo2p_video3_demo1p_11_50	4	6	vid3event11	6	\N	\N	\N
video3	demo2p_video3_demo1p_11_50	1	1	vid3event12	2	\N	\N	\N
\.






ALTER TABLE ONLY demo1out
    ADD CONSTRAINT demo1out_pk PRIMARY KEY (seqname, process, t1, t2);

ALTER TABLE ONLY demo2out
    ADD CONSTRAINT demo2out_pk PRIMARY KEY (seqname, process, t1, t2);

ALTER TABLE ONLY processes
    ADD CONSTRAINT processes_pk PRIMARY KEY (prsname);

ALTER TABLE ONLY sequences
    ADD CONSTRAINT sequences_pk PRIMARY KEY (seqname);

ALTER TABLE ONLY processes
    ADD CONSTRAINT inputs_fk FOREIGN KEY (inputs) REFERENCES processes(prsname) ON UPDATE CASCADE ON DELETE RESTRICT;

ALTER TABLE ONLY processes
    ADD CONSTRAINT method_fk FOREIGN KEY (mtname) REFERENCES public.methods(mtname) ON UPDATE CASCADE ON DELETE RESTRICT;

ALTER TABLE ONLY demo1out
    ADD CONSTRAINT demo1outseq_fk FOREIGN KEY (seqname) REFERENCES sequences(seqname) ON UPDATE CASCADE ON DELETE RESTRICT;

ALTER TABLE ONLY demo1out
    ADD CONSTRAINT demo1outprs_fk FOREIGN KEY (process) REFERENCES processes(prsname) ON UPDATE CASCADE ON DELETE RESTRICT;

ALTER TABLE ONLY demo2out
    ADD CONSTRAINT demo2outseq_fk FOREIGN KEY (seqname) REFERENCES sequences(seqname) ON UPDATE CASCADE ON DELETE RESTRICT;

ALTER TABLE ONLY demo2out
    ADD CONSTRAINT demo2outprs_fk FOREIGN KEY (process) REFERENCES processes(prsname) ON UPDATE CASCADE ON DELETE RESTRICT;

