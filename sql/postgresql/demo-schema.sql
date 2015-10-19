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

-- create dataset
SELECT public.VT_dataset_drop('demo');
SELECT public.VT_dataset_create('demo', 'demo', 'Pre-generated dataset', 'This dataset is for demonstration purposes only');

SET search_path = demo, pg_catalog;



-- delete demo modules
SELECT public.VT_method_delete('demo1', TRUE);
SELECT public.VT_method_delete('demo2', TRUE);

-- insert demo modules
SELECT public.VT_method_add(
    'demo1',
    '{"(features_array,real[],out,,,,)", "(features_mat,public.cvmat,out,,,,)"}',
    '{"(param1,int,true,50,[-100\\,100],)", "(param2,double,true,0.5,[-1.0\\,1.0],)"}',
    FALSE,
    'auto-generated demo method #1',
    NULL);
SELECT public.VT_method_add(
    'demo2',
    '{"(features_array,real[],in,,,,)", "(features_mat,public.cvmat,in,,,,)",
    "(event,public.vtevent,out,true,,{4},)"}', '{"(video,string,true,,,)"}',
    TRUE,
    'auto-generated demo method #2',
    NULL);

-- delete tasks
SELECT public.VT_task_delete('task_demo2_1', TRUE, 'demo');
SELECT public.VT_task_delete('task_demo2_2', TRUE, 'demo');
SELECT public.VT_task_delete('task_demo1_1', TRUE, 'demo');
SELECT public.VT_task_delete('task_demo1_2', TRUE, 'demo');
SELECT public.VT_task_delete('task_demo1_3', TRUE, 'demo');
 -- not needed => returns FALSE because "task_demo1_3" was forced (in case, if it was not dropped whole dataset at the beggining of demo-schema.sql)
SELECT public.VT_task_delete('task_demo2_3', TRUE, 'demo');

-- create tasks with their output tables (3 tasks per module; 1st module is prerequisity for 2nd module)
SELECT public.VT_task_create('task_demo1_1', 'demo1', '{param1:-53,param2:-0.8}', NULL, NULL, 'demo');
SELECT public.VT_task_create('task_demo1_2', 'demo1', '{param1:0,param2:1.0}', NULL, NULL, 'demo');
SELECT public.VT_task_create('task_demo1_3', 'demo1', '{param1:11,param2:0.76}', NULL, NULL, 'demo');
SELECT public.VT_task_create('task_demo2_1', 'demo2', '{video:video3}', 'task_demo1_1', NULL, 'demo');
SELECT public.VT_task_create('task_demo2_2', 'demo2', '{video:video1}', 'task_demo1_2', NULL, 'demo');
SELECT public.VT_task_create('task_demo2_3', 'demo2', '{video:video3}', 'task_demo1_3', NULL, 'demo');

---------------------------------------------------

-- insert videos
INSERT INTO sequences (seqname, seqlocation, seqtyp, seqlength, vid_fps, vid_speed, vid_time, comment) VALUES
    ('video1', 'video1.mpg', 'video', 5207, 29.97, 1, '2015-04-01 04:05:06', 'pre-generated'),
    ('video2', 'video2.mpg', 'video', 6499, 29.97, 1, '2015-04-01 05:06:07', 'pre-generated'),
    ('video3', 'video3.mpg', 'video', 1648, 25, 1, '2015-04-01 06:07:08', 'pre-generated');

-- create some processes and assign tasks to them
-- 2 processes for first task (with different sequences to process)
INSERT INTO processes (taskname, state) VALUES
    ('task_demo1_1', '(finished,100,,)'),
    ('task_demo1_1', '(finished,100,,)'),
    ('task_demo1_2', '(running,0,"video1",)'), -- suspended does not exist anymore
    ('task_demo1_3', '(running,0,"video1",)'), -- suspended does not exist anymore
    ('task_demo2_1', '(error,33,"video2","cannot process video")'),
    ('task_demo2_2', '(created,0,,)'),
    ('task_demo2_3', '(created,0,,)');

-- set sequences to be processed by processes
-- if no sequences are set for process then process all sequences
INSERT INTO rel_processes_sequences_assigned (prsid, seqname) VALUES
    (1, 'video1'),
    (1, 'video2'),
    (2, 'video3');

-- insert information about already processed sequences with certain tasks
-- only first task for both methods has been done (partially)
INSERT INTO rel_tasks_sequences_done (taskname, prsid, seqname, is_done) VALUES
    ('task_demo1_1', 1, 'video1', true),
    ('task_demo1_1', 1, 'video2', true),
    ('task_demo1_1', 2, 'video3', true),
    ('task_demo2_1', 1, 'video1', true);

-- insert results for demo1 tasks (only for first task)
INSERT INTO demo1_out (taskname, seqname, t1, t2, features_array, features_mat) VALUES
    ('task_demo1_1', 'video1', 1, 1, '{1.8,2.3,2.5}', '(5,"{3,1}","\\x00000000cdcccc3dcdcc4c3e")'),
    ('task_demo1_1', 'video2', 2, 2, '{1.6}', '(5,"{2,2}","\\x00000000cdcccc3dcdcc4c3e4c3ecdcc")'),
    ('task_demo1_1', 'video3', 1, 1, '{1.8}', '(5,"{2,1}","\\xcdcccc3dcdcccc")'),
    ('task_demo1_1', 'video3', 5, 32, '{7.9,2.9,0.01}', '(5,"{1,1}","\\x00000000")'),
    ('task_demo1_1', 'video3', 7, 15, '{2.9,0.07,1.5}', '(5,"{3,1}","\\x00000000cdcccc3dcdcc4c3e")');

-- insert results for demo2 processes (only for first task and first video)
INSERT INTO demo2_out (taskname, seqname, t1, t2, event) VALUES
    ('task_demo2_1', 'video1', 1, 15, '(1,55,1,"((0.2,0.5),(0.11,0.13))",0.23,)'),
    ('task_demo2_1', 'video1', 16, 79, '(2,17,1,"((0,0.7),(1,1))",0.11,)');
