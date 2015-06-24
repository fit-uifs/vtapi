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


SELECT public.VT_dataset_drop('demo');
SELECT public.VT_dataset_create('demo', 'demo/', 'auto-generated demo');

SET search_path = demo, pg_catalog;

-------------------------------------
-- CREATE tables
-------------------------------------

-- intervals table for demo1 results
CREATE TABLE demo1out (
    id serial NOT NULL,
    seqname name NOT NULL,
    prsname name,
    t1 integer NOT NULL,
    t2 integer NOT NULL,
    rt_start     timestamp without time zone   DEFAULT NULL, -- trigger supplied
    sec_length   real, -- trigger supplied
    imglocation character varying,
    features_array real[],
    features_mat public.cvmat,
    userid name,
    created timestamp without time zone DEFAULT now(),
    notes text,
    CONSTRAINT demo1out_pk PRIMARY KEY (id),
    CONSTRAINT seqname_fk FOREIGN KEY (seqname)
      REFERENCES sequences(seqname) ON UPDATE CASCADE ON DELETE RESTRICT,
    CONSTRAINT prsname_fk FOREIGN KEY (prsname)
      REFERENCES processes(prsname) ON UPDATE CASCADE ON DELETE RESTRICT
);
CREATE INDEX demo1out_seqname_idx ON demo1out(seqname);
CREATE INDEX demo1out_prsname_idx ON demo1out(prsname);
CREATE INDEX demo1out_sec_length_idx ON demo1out(sec_length);
CREATE INDEX demo1out_imglocation_idx ON demo1out(imglocation);
CREATE INDEX demo1out_tsrange_idx ON demo1out USING GIST ( public.tsrange(rt_start, sec_length) );

CREATE TRIGGER demo1out_provide_realtime
  BEFORE INSERT OR UPDATE
  ON demo1out
  FOR EACH ROW
  EXECUTE PROCEDURE public.trg_interval_provide_realtime();


-- intervals table for demo2 results
CREATE TABLE demo2out (
    id serial NOT NULL,
    seqname name NOT NULL,
    prsname name,
    t1 integer NOT NULL,
    t2 integer NOT NULL,
    rt_start     timestamp without time zone   DEFAULT NULL, -- trigger supplied
    sec_length   real, -- trigger supplied
    imglocation character varying,
    event public.vtevent NOT NULL,
    userid name,
    created timestamp without time zone DEFAULT now(),
    notes text,
    CONSTRAINT demo2out_pk PRIMARY KEY (id),
    CONSTRAINT seqname_fk FOREIGN KEY (seqname)
      REFERENCES sequences(seqname) ON UPDATE CASCADE ON DELETE RESTRICT,
    CONSTRAINT prsname_fk FOREIGN KEY (prsname)
      REFERENCES processes(prsname) ON UPDATE CASCADE ON DELETE RESTRICT
);
CREATE INDEX demo2out_seqname_idx ON demo2out(seqname);
CREATE INDEX demo2out_prsname_idx ON demo2out(prsname);
CREATE INDEX demo2out_sec_length_idx ON demo2out(sec_length);
CREATE INDEX demo2out_imglocation_idx ON demo2out(imglocation);
CREATE INDEX demo2out_tsrange_idx ON demo2out USING GIST ( public.tsrange(rt_start, sec_length) );
CREATE INDEX demo2out_event_region_idx ON demo2out USING GIST (( (event).region ));

CREATE TRIGGER demo2out_provide_realtime
  BEFORE INSERT OR UPDATE
  ON demo2out
  FOR EACH ROW
  EXECUTE PROCEDURE public.trg_interval_provide_realtime();

-------------------------------------
-- DROP and INSERT new schema metadata
-------------------------------------

-- insert demo1 modules metadata into public schema
SELECT public.VT_method_delete('demo1');
SELECT public.VT_method_add('demo1', '{"(param1,int,in_param,{50},)", "(param2,real,in_param,{720.4},)", "(features_array,real[],out,,)", "(features_mat,public.cvmat,out,,)"}', 'auto-generated');

-- insert demo2 modules metadata into public schema
SELECT public.VT_method_delete('demo2');
SELECT public.VT_method_add('demo2', '{"(video,varchar,in_param,,)", "(features_array,real[],in,,)", "(features_mat,public.cvmat,in,,)", "(event,public.vtevent,out,,)"}', 'auto-generated');

INSERT INTO public.methods_keys (mtname, keyname, typname, inout, default_num, default_str) VALUES
    ('demo2', 'video', 'varchar', 'in_param', NULL, NULL),
    ('demo2', 'features_array', 'real[]', 'in', NULL, NULL),
    ('demo2', 'features_mat', 'public.cvmat', 'in', NULL, NULL),
    ('demo2', 'event', 'public.vtevent', 'out', NULL, NULL);

-- insert videos
INSERT INTO sequences (seqname, seqlocation, seqtyp, vid_length, vid_fps, vid_speed, vid_time, userid, notes) VALUES
    ('video1', 'video1.mpg', 'video', 5207, 29.97, 1, '2015-04-01 04:05:06', 'demouser', 'pre-generated'),
    ('video2', 'video2.mpg', 'video', 6499, 29.97, 1, '2015-04-01 05:06:07', 'demouser', 'pre-generated'),
    ('video3', 'video3.mpg', 'video', 1648, 25, 1, '2015-04-01 06:07:08', 'demouser', 'pre-generated');

-- insert processes (3 processes for both modules)
INSERT INTO processes (prsname, mtname, inputs, outputs, state, params, userid, notes) VALUES
    ('demo1p_5000_25', 'demo1', NULL, 'demo1out', '(finished,100,,)', '{param1:5000,param2:25}', 'demouser', 'pre-generated'),
    ('demo1p_0_10', 'demo1', NULL, 'demo1out', '(error,32,"video1","cannot process video")', '{param1:0,param2:10}', 'demouser', 'pre-generated'),
    ('demo1p_11_50', 'demo2', NULL, 'demo1out', '(running,35,"video3",)','{param1:11,param2:50}', 'demouser', 'pre-generated'),
    ('demo2p_video3_demo1p_5000_25', 'demo2', 'demo1p_5000_25', 'demo2out', '(finished,100,,)','{video:video3}', 'demouser', 'pre-generated'),
    ('demo2p_video1_demo1p_11_50', 'demo2', 'demo1p_11_50', 'demo2out', '(suspended,10,"video1",)','{video:video1}', 'demouser', 'pre-generated'),
    ('demo2p_video3_demo1p_11_50', 'demo2', 'demo1p_11_50', 'demo2out', '(running,15,"video2",)','{video:video3}', 'demouser', 'pre-generated');

-- insert results for demo1 processes (only process 1)
INSERT INTO demo1out (seqname, prsname, t1, t2, features_array, features_mat, userid, notes) VALUES
    ('video1', 'demo1p_5000_25', 1, 1, '{1.8,2.3,2.5}', '(5,"{3,1}","\\x00000000cdcccc3dcdcc4c3e")', 'demouser', 'pre-generated'),
    ('video2', 'demo1p_5000_25', 2, 2, '{1.6}', '(5,"{2,2}","\\x00000000cdcccc3dcdcc4c3e4c3ecdcc")', 'demouser', 'pre-generated'),
    ('video3', 'demo1p_5000_25', 1, 1, '{1.8}', '(5,"{2,1}","\\xcdcccc3dcdcccc")', 'demouser', 'pre-generated'),
    ('video3', 'demo1p_5000_25', 5, 32, '{7.9,2.9,0.01}', '(5,"{1,1}","\\x00000000")', 'demouser', 'pre-generated'),
    ('video3', 'demo1p_5000_25', 7, 15, '{2.9,0.07,1.5}', '(5,"{3,1}","\\x00000000cdcccc3dcdcc4c3e")', 'demouser', 'pre-generated');

-- insert results for demo2 processes (processes 4,5,6)
INSERT INTO demo2out (seqname, prsname, t1, t2, event, userid, notes) VALUES
    ('video3', 'demo2p_video3_demo1p_5000_25', 1, 1, '(1,20,0,"((0.1,0.2),(0.4,0.75))",0.8,)', 'demouser', 'pre-generated'),
    ('video3', 'demo2p_video3_demo1p_5000_25', 2, 2, '(2,15,0,"((0,0),(1,1))",0.74,)', 'demouser', 'pre-generated'),
    ('video1', 'demo2p_video1_demo1p_11_50', 1, 15, '(1,55,0,"((0.2,0.5),(0.11,0.13))",0.23,)', 'demouser', 'pre-generated'),
    ('video1', 'demo2p_video1_demo1p_11_50', 16, 79, '(2,17,0,"((0,0.7),(1,1))",0.11,)', 'demouser', 'pre-generated'),
    ('video3', 'demo2p_video3_demo1p_11_50', 1, 2, '(1,12,1,"((0.7,0),(1,0.9))",0.42,"\\x41484f4a")', 'demouser', 'pre-generated'),
    ('video3', 'demo2p_video3_demo1p_11_50', 1, 1, '(1,12,0,"((0.1,0.2),(0.5,0.5))",0.63,)', 'demouser', 'pre-generated'),
    ('video3', 'demo2p_video3_demo1p_11_50', 2, 2, '(1,12,0,"((0.4,0.4),(0.6,0.6))",0.11,)', 'demouser', 'pre-generated');
