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

SELECT public.VT_dataset_drop('test');
SELECT public.VT_dataset_create('test', 'data/test/', 'testovaci dataset');

SET search_path = test, pg_catalog;

-------------------------------------
-- CREATE module-specific tables
--  create one outputs table for every module according to specifications
--  out_* columns are to be replaced here
-------------------------------------

-- intervals table for demo1 results
CREATE TABLE test1out (
    id serial NOT NULL,
    seqname name NOT NULL,
    prsname name,
    t1 integer NOT NULL,
    t2 integer NOT NULL,
    rt_start     timestamp without time zone   DEFAULT NULL, -- trigger supplied
    sec_length   real,   -- trigger supplied
    imglocation character varying,
    out_features_array real[],
    out_features_mat public.cvmat,
    out_event public.vtevent NOT NULL,
    created timestamp without time zone DEFAULT now(),
    notes text,
    CONSTRAINT test1out_pk PRIMARY KEY (id),
    CONSTRAINT seqname_fk FOREIGN KEY (seqname)
      REFERENCES sequences(seqname) ON UPDATE CASCADE ON DELETE RESTRICT,
    CONSTRAINT prsname_fk FOREIGN KEY (prsname)
      REFERENCES processes(prsname) ON UPDATE CASCADE ON DELETE RESTRICT
);
CREATE INDEX test1out_seqname_idx ON test1out(seqname);
CREATE INDEX test1out_prsname_idx ON test1out(prsname);
CREATE INDEX test1out_sec_length_idx ON test1out(sec_length);
CREATE INDEX test1out_imglocation_idx ON test1out(imglocation);
CREATE INDEX test1out_tsrange_idx ON test1out USING GIST ( public.tsrange(rt_start, sec_length) );
CREATE INDEX test1out_event_region_idx ON test1out USING GIST (( (out_event).region ));

CREATE TRIGGER test1out_provide_realtime
  BEFORE INSERT OR UPDATE
  ON test1out
  FOR EACH ROW
  EXECUTE PROCEDURE public.trg_interval_provide_realtime();