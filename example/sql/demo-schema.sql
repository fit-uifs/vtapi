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



-- insert demo1 modules metadata into public schema
SELECT public.VT_method_delete('demo1');
SELECT public.VT_method_add('demo1', '{"(features_array,real[],out,,,,)", "(features_mat,public.cvmat,out,,,,)"}', '{"(param1,int,true,50,[-100\\,100],)", "(param2,double,true,0.5,[-1.0\\,1.0],)"}', TRUE, 'auto-generated');

-- insert demo2 modules metadata into public schema
SELECT public.VT_method_delete('demo2');
SELECT public.VT_method_add('demo2', '{"(features_array,real[],in,,,,)", "(features_mat,public.cvmat,in,,,,)", "(event,public.vtevent,out,true,,{4},)"}', '{"(video,string,true,,,)"}', TRUE, 'auto-generated');

-- insert videos
INSERT INTO sequences (seqname, seqlocation, seqtyp, vid_length, vid_fps, vid_speed, vid_time, notes) VALUES
    ('video1', 'video1.mpg', 'video', 5207, 29.97, 1, '2015-04-01 04:05:06', 'pre-generated'),
    ('video2', 'video2.mpg', 'video', 6499, 29.97, 1, '2015-04-01 05:06:07', 'pre-generated'),
    ('video3', 'video3.mpg', 'video', 1648, 25, 1, '2015-04-01 06:07:08', 'pre-generated');

-- create process' output tables
SELECT public.VT_process_output_create('demo1');
SELECT public.VT_process_output_create('demo2');

-- insert processes (3 processes for both modules)
INSERT INTO processes (prsname, mtname, inputs, outputs, state, params, notes) VALUES
    ('demo1p_5000_25', 'demo1', NULL, 'demo1_out', '(finished,100,,)', '{param1:5000,param2:25}', 'pre-generated'),
    ('demo1p_0_10', 'demo1', NULL, 'demo1_out', '(error,32,"video1","cannot process video")', '{param1:0,param2:10}', 'pre-generated'),
    ('demo1p_11_50', 'demo2', NULL, 'demo1_out', '(running,35,"video3",)','{param1:11,param2:50}', 'pre-generated'),
    ('demo2p_video3_demo1p_5000_25', 'demo2', 'demo1p_5000_25', 'demo2_out', '(finished,100,,)','{video:video3}', 'pre-generated'),
    ('demo2p_video1_demo1p_11_50', 'demo2', 'demo1p_11_50', 'demo2_out', '(suspended,10,"video1",)','{video:video1}', 'pre-generated'),
    ('demo2p_video3_demo1p_11_50', 'demo2', 'demo1p_11_50', 'demo2_out', '(running,15,"video2",)','{video:video3}', 'pre-generated');

-- insert results for demo1 processes (only process 1)
INSERT INTO demo1_out (seqname, prsname, t1, t2, features_array, features_mat) VALUES
    ('video1', 'demo1p_5000_25', 1, 1, '{1.8,2.3,2.5}', '(5,"{3,1}","\\x00000000cdcccc3dcdcc4c3e")'),
    ('video2', 'demo1p_5000_25', 2, 2, '{1.6}', '(5,"{2,2}","\\x00000000cdcccc3dcdcc4c3e4c3ecdcc")'),
    ('video3', 'demo1p_5000_25', 1, 1, '{1.8}', '(5,"{2,1}","\\xcdcccc3dcdcccc")'),
    ('video3', 'demo1p_5000_25', 5, 32, '{7.9,2.9,0.01}', '(5,"{1,1}","\\x00000000")'),
    ('video3', 'demo1p_5000_25', 7, 15, '{2.9,0.07,1.5}', '(5,"{3,1}","\\x00000000cdcccc3dcdcc4c3e")');

-- insert results for demo2 processes (processes 4,5,6)
INSERT INTO demo2_out (seqname, prsname, t1, t2, event) VALUES
    ('video3', 'demo2p_video3_demo1p_5000_25', 1, 1, '(1,20,0,"((0.1,0.2),(0.4,0.75))",0.8,)'),
    ('video3', 'demo2p_video3_demo1p_5000_25', 2, 2, '(2,15,0,"((0,0),(1,1))",0.74,)'),
    ('video1', 'demo2p_video1_demo1p_11_50', 1, 15, '(1,55,0,"((0.2,0.5),(0.11,0.13))",0.23,)'),
    ('video1', 'demo2p_video1_demo1p_11_50', 16, 79, '(2,17,0,"((0,0.7),(1,1))",0.11,)'),
    ('video3', 'demo2p_video3_demo1p_11_50', 1, 2, '(1,12,1,"((0.7,0),(1,0.9))",0.42,"\\x41484f4a")'),
    ('video3', 'demo2p_video3_demo1p_11_50', 1, 1, '(1,12,0,"((0.1,0.2),(0.5,0.5))",0.63,)'),
    ('video3', 'demo2p_video3_demo1p_11_50', 2, 2, '(1,12,0,"((0.4,0.4),(0.6,0.6))",0.11,)');
