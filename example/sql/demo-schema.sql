--
-- Create schema demo
--

-- usage:
-- psql -h 127.0.0.1 -p 5432 -f demo-schema.sql <db> <user>
-- ex.:
-- psql -h 127.0.0.1 -p 5432 -f demo-schema.sql vidte vidte


SET statement_timeout = 0;
SET lock_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SET check_function_bodies = false;
SET client_min_messages = warning;

SET search_path = demo, public, pg_catalog;


-- recreate custom schema "demo"
DROP SCHEMA IF EXISTS demo CASCADE;
CREATE SCHEMA demo;

GRANT ALL ON SCHEMA demo TO postgres;

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
    seqtyp public.seqtype,
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

-- create intervals table for demo1 results
CREATE TABLE demo1out (
    seqname character varying NOT NULL,
    process character varying,
    t1 integer NOT NULL,
    t2 integer NOT NULL,
    vals real[],
    features public.cvmat,
    userid name,
    created timestamp without time zone DEFAULT now(),
    notes text,
    CONSTRAINT demo1out_pk PRIMARY KEY (seqname, process, t1, t2),
    CONSTRAINT seqname_fk FOREIGN KEY (seqname)
      REFERENCES sequences(seqname) ON UPDATE CASCADE ON DELETE RESTRICT,
    CONSTRAINT process_fk FOREIGN KEY (process)
      REFERENCES processes(prsname) ON UPDATE CASCADE ON DELETE RESTRICT
);
CREATE INDEX demo1out_seqname_idx ON demo1out(seqname);
CREATE INDEX demo1out_process_idx ON demo1out(process);

-- create intervals table for demo2 results
CREATE TABLE demo2out (
    seqname character varying NOT NULL,
    process character varying,
    t1 integer NOT NULL,
    t2 integer NOT NULL,
    event_name character varying,
    event_arg integer,
    userid name,
    created timestamp without time zone DEFAULT now(),
    notes text,
    CONSTRAINT demo2out_pk PRIMARY KEY (seqname, process, t1, t2),
    CONSTRAINT seqname_fk FOREIGN KEY (seqname)
      REFERENCES sequences(seqname) ON UPDATE CASCADE ON DELETE RESTRICT,
    CONSTRAINT process_fk FOREIGN KEY (process)
      REFERENCES processes(prsname) ON UPDATE CASCADE ON DELETE RESTRICT
);
CREATE INDEX demo2out_seqname_idx ON demo2out(seqname);
CREATE INDEX demo2out_process_idx ON demo2out(process);

-- insert into dataset list
DELETE FROM public.datasets WHERE dsname = 'demo';
\copy public.datasets (dsname, dslocation, userid, notes) FROM stdin ;
demo	demo/	demouser	auto-generated demo dataset
\.

-- insert methods
DELETE FROM public.methods_keys WHERE mtname IN ('demo1','demo2');
DELETE FROM public.methods WHERE mtname IN ('demo1','demo2');

\copy public.methods (mtname, userid, notes) FROM stdin ;
demo1	demouser	auto-generated
demo2	demouser	auto-generated
\.
\copy public.methods_keys (mtname, keyname, typname, inout, default_num, default_str) FROM stdin ;
demo1	param1	integer	in_param	{50}	\N
demo1	param2	integer	in_param	{720}	\N
demo1	vals	real[]	out	\N	\N
demo1	features	public.cvmat	out	\N	\N
demo2	event_arg	integer	out	\N	\N
demo2	event_name	varchar	out	\N	\N
demo2	vals	real[]	in	{0.0,0.0,0.0}	\N
demo2	features	public.cvmat	in	\N	\N
demo2	video	varchar	in_param	\N	\N
\.

-- copy demo data

\copy sequences (seqname, seqlocation, seqtyp, userid, notes) FROM stdin ;
video1	video1.mpg	video	demouser	pre-generated
video2	video2.mpg	video	demouser	pre-generated
video3	video3.mpg	video	demouser	pre-generated
\.

\copy processes (mtname, prsname, outputs, inputs, params, userid, notes) FROM stdin ;
demo1	demo1p_5000_25	demo1out	\N	{param1:5000,param2:25}	demouser	pre-generated
demo1	demo1p_0_10	demo1out	\N	{param1:0,param2:10}	demouser	pre-generated
demo1	demo1p_11_50	demo1out	\N	{param1:11,param2:50}	demouser	pre-generated
demo2	demo2p_video3_demo1p_5000_25	demo2out	demo1p_5000_25	{video:video3}	demouser	pre-generated
demo2	demo2p_video1_demo1p_11_50	demo2out	demo1p_11_50	{video:video1}	demouser	pre-generated
demo2	demo2p_video3_demo1p_11_50	demo2out	demo1p_11_50	{video:video3}	demouser	pre-generated
\.

\copy demo1out (seqname, process, t1, t2, vals, features, userid, notes) FROM stdin ;
video1	demo1p_5000_25	1	1	{1.8,2.3,2.5}	(5,"{3,1}","\\\\x00000000cdcccc3dcdcc4c3e")	demouser	pre-generated
video2	demo1p_5000_25	2	1	{1.6}	(5,"{2,2}","\\\\x00000000cdcccc3dcdcc4c3e4c3ecdcc")	demouser	pre-generated
video3	demo1p_5000_25	1	1	{1.8}	(5,"{2,1}","\\\\xcdcccc3dcdcccc")	demouser	pre-generated
video3	demo1p_5000_25	5	32	{7.9,2.9,0.01}	(5,"{1,1}","\\\\x00000000")	demouser	pre-generated
video3	demo1p_5000_25	7	15	{2.9,0.07,1.5}	(5,"{3,1}","\\\\x00000000cdcccc3dcdcc4c3e")	demouser	pre-generated
\.

\copy demo2out (seqname, process, t1, t2, event_name, event_arg, userid, notes) FROM stdin ;
video1	demo2p_video3_demo1p_5000_25	1	1	vid1event1	5	demouser	pre-generated
video2	demo2p_video3_demo1p_5000_25	2	1	vid2event1	7	demouser	pre-generated
video3	demo2p_video3_demo1p_5000_25	1	1	vid3event1	2	demouser	pre-generated
video3	demo2p_video3_demo1p_5000_25	5	32	vid3event2	8	demouser	pre-generated
video3	demo2p_video3_demo1p_5000_25	7	15	vid3event3	11	demouser	pre-generated
video3	demo2p_video3_demo1p_11_50	4	6	vid3event11	6	demouser	pre-generated
video3	demo2p_video3_demo1p_11_50	1	1	vid3event12	2	demouser	pre-generated
\.


