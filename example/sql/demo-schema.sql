-- Create PostgreSQL database schema for demo project (demo1+demo2 modules)
--
-- This is how to use this with psql:
--      psql -h 127.0.0.1 -p 5432 -f demo-schema.sql <db> <user>
-- ex.:
--      psql -h 127.0.0.1 -p 5432 -f demo-schema.sql vidte vidte
--
-- authors: ifroml[at]fit.vutbr.cz; ivolf[at]fit.vutbr.cz

SET statement_timeout = 0;
SET lock_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SET check_function_bodies = false;
SET client_min_messages = warning;
SET search_path = demo, pg_catalog;

-------------------------------------
-- DROP and CREATE demo schema
-------------------------------------

DROP SCHEMA IF EXISTS demo CASCADE;
CREATE SCHEMA demo;
GRANT ALL ON SCHEMA demo TO postgres;

-------------------------------------
-- CREATE tables
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

-- intervals table for demo1 results
CREATE TABLE demo1out (
    id serial NOT NULL,
    seqid integer NOT NULL,
    prsid integer NOT NULL,
    t1 integer NOT NULL,
    t2 integer NOT NULL,
    features_array real[],
    features_mat public.cvmat,
    userid name,
    created timestamp without time zone DEFAULT now(),
    notes text,
    CONSTRAINT demo1out_pk PRIMARY KEY (id),
    CONSTRAINT seqid_fk FOREIGN KEY (seqid)
      REFERENCES sequences(id) ON UPDATE CASCADE ON DELETE RESTRICT,
    CONSTRAINT prsid_fk FOREIGN KEY (prsid)
      REFERENCES processes(id) ON UPDATE CASCADE ON DELETE RESTRICT
);
CREATE INDEX demo1out_seqid_idx ON demo1out(seqid);
CREATE INDEX demo1out_prsid_idx ON demo1out(prsid);

-- intervals table for demo2 results
CREATE TABLE demo2out (
    id serial NOT NULL,
    seqid integer NOT NULL,
    prsid integer NOT NULL,
    t1 integer NOT NULL,
    t2 integer NOT NULL,
    event public.vtevent NOT NULL,
    userid name,
    created timestamp without time zone DEFAULT now(),
    notes text,
    CONSTRAINT demo2out_pk PRIMARY KEY (id),
    CONSTRAINT seqid_fk FOREIGN KEY (seqid)
      REFERENCES sequences(id) ON UPDATE CASCADE ON DELETE RESTRICT,
    CONSTRAINT prsid_fk FOREIGN KEY (prsid)
      REFERENCES processes(id) ON UPDATE CASCADE ON DELETE RESTRICT
);
CREATE INDEX demo2out_seqid_idx ON demo2out(seqid);
CREATE INDEX demo2out_prsid_idx ON demo2out(prsid);

-------------------------------------
-- DROP and INSERT new schema metadata
-------------------------------------

-- insert into dataset list
DELETE FROM public.datasets WHERE dsname = 'demo';
INSERT INTO public.datasets (dsname, dslocation, userid, notes) VALUES
    ('demo', 'demo/', 'demouser', 'auto-generated demo');

-- insert demo1 modules metadata into public schema
DELETE FROM public.methods WHERE mtname = 'demo1';
WITH mid AS
(
    INSERT INTO public.methods (mtname, userid, notes) VALUES
        ('demo1', 'demouser', 'auto-generated')
    RETURNING id
)
INSERT INTO public.methods_keys (mtid, keyname, typname, inout, default_num, default_str) VALUES
    ((SELECT id FROM mid), 'param1', 'integer', 'in_param', '{50}', NULL),
    ((SELECT id FROM mid), 'param2', 'real', 'in_param', '{720.4}', NULL),
    ((SELECT id FROM mid), 'features_array', 'real[]', 'out', NULL, NULL),
    ((SELECT id FROM mid), 'features_mat', 'public.cvmat', 'out', NULL, NULL);

-- insert demo2 modules metadata into public schema
DELETE FROM public.methods WHERE mtname = 'demo2';
WITH mid AS
(
    INSERT INTO public.methods (mtname, userid, notes) VALUES
        ('demo2', 'demouser', 'auto-generated')
    RETURNING id
)
INSERT INTO public.methods_keys (mtid, keyname, typname, inout, default_num, default_str) VALUES
    ((SELECT id FROM mid), 'video', 'varchar', 'in_param', NULL, NULL),
    ((SELECT id FROM mid), 'event', 'public.vtevent', 'out', NULL, NULL);

-- insert videos
INSERT INTO sequences (seqname, seqlocation, seqtyp, userid, notes) VALUES
    ('video1', 'video1.mpg', 'video', 'demouser', 'pre-generated'),
    ('video2', 'video2.mpg', 'video', 'demouser', 'pre-generated'),
    ('video3', 'video3.mpg', 'video', 'demouser', 'pre-generated');

-- insert processes (3 processes for both modules)
WITH mid1 AS
(
    SELECT id FROM public.methods WHERE mtname = 'demo1'
),
mid2 AS
(
    SELECT id FROM public.methods WHERE mtname = 'demo2'
)
INSERT INTO processes (mtid, prsname, prsid_inputs, outputs, params, userid, notes) VALUES
    ((SELECT id FROM mid1), 'demo1p_5000_25', NULL, 'demo1out', '{param1:5000,param2:25}', 'demouser', 'pre-generated'),
    ((SELECT id FROM mid1), 'demo1p_0_10', NULL, 'demo1out', '{param1:0,param2:10}', 'demouser', 'pre-generated'),
    ((SELECT id FROM mid1), 'demo1p_11_50', NULL, 'demo1out', '{param1:11,param2:50}', 'demouser', 'pre-generated'),
    ((SELECT id FROM mid2), 'demo2p_video3_demo1p_5000_25', 1, 'demo2out', '{video:video3}', 'demouser', 'pre-generated'),
    ((SELECT id FROM mid2), 'demo2p_video1_demo1p_11_50', 3, 'demo2out', '{video:video1}', 'demouser', 'pre-generated'),
    ((SELECT id FROM mid2), 'demo2p_video3_demo1p_11_50', 3, 'demo2out', '{video:video3}', 'demouser', 'pre-generated');

-- insert results for demo1 processes (only process 1)
INSERT INTO demo1out (seqid, prsid, t1, t2, features_array, features_mat, userid, notes) VALUES
    (1, 1, 1, 1, '{1.8,2.3,2.5}', '(5,"{3,1}","\\x00000000cdcccc3dcdcc4c3e")', 'demouser', 'pre-generated'),
    (2, 1, 2, 1, '{1.6}', '(5,"{2,2}","\\x00000000cdcccc3dcdcc4c3e4c3ecdcc")', 'demouser', 'pre-generated'),
    (3, 1, 1, 1, '{1.8}', '(5,"{2,1}","\\xcdcccc3dcdcccc")', 'demouser', 'pre-generated'),
    (3, 1, 5, 32, '{7.9,2.9,0.01}', '(5,"{1,1}","\\x00000000")', 'demouser', 'pre-generated'),
    (3, 1, 7, 15, '{2.9,0.07,1.5}', '(5,"{3,1}","\\x00000000cdcccc3dcdcc4c3e")', 'demouser', 'pre-generated');

-- insert results for demo2 processes (processes 4,5,6)
INSERT INTO demo2out (seqid, prsid, t1, t2, event, userid, notes) VALUES
    (3, 4, 1, 1, '(1,20,0,"((5,9),(16,36))",0.8,NULL)', 'demouser', 'pre-generated'),
    (3, 4, 2, 2, '(2,15,0,"((14,19),(22,63))",0.74,NULL)', 'demouser', 'pre-generated'),
    (1, 5, 1, 15, '(1,55,0,"((5,63),(17,19))",0.23,NULL)', 'demouser', 'pre-generated'),
    (1, 5, 16, 79, '(2,17,0,"((1,3),(12,86))",0.11,NULL)', 'demouser', 'pre-generated'),
    (3, 6, 1, 2, '(1,12,1,"((10,20),(10,20))",0.42,"\\x41484f4a")', 'demouser', 'pre-generated'),
    (3, 6, 1, 1, '(1,12,0,"((10,20),(10,10))",0.63,NULL)', 'demouser', 'pre-generated'),
    (3, 6, 2, 2, '(1,12,0,"((10,10),(10,20))",0.11,NULL)', 'demouser', 'pre-generated');
