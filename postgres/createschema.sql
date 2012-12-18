--
-- Create schema test
--

-- replace test with the schema_name!:)
-- specify the dataset location within your datasets (last line)

-- work as the database user created by createdb

--
-- PostgreSQL database schema test
--
SET statement_timeout = 0;
SET client_encoding = 'SQL_ASCII';
SET standard_conforming_strings = on;
SET check_function_bodies = false;
SET client_min_messages = warning;
SET default_with_oids = false;


CREATE schema test;

SET search_path = test, public, pg_catalog;



--
-- Name: intervals; Type: TABLE; Schema: test; Owner: -
--
CREATE TABLE intervals (
    seqname character varying NOT NULL,
    t1 integer NOT NULL,
    t2 integer NOT NULL,
    imglocation character varying,
    tags integer[],
    svm real[],
    userid name,
    created timestamp without time zone DEFAULT now(),
    notes text,
    "Process_name" real[]
);


--
-- Name: sequences; Type: TABLE; Schema: test; Owner: -
--
CREATE TABLE sequences (
    seqname name NOT NULL,
    seqnum integer,
    seqlocation character varying,
    seqtyp seqtype DEFAULT 'data'::public.seqtype,
    userid name,
    groupid name,
    created timestamp without time zone,
    changed timestamp without time zone,
    notes text
);


--
-- Name: intervals_pk; Type: CONSTRAINT; Schema: test; Owner: -
--
ALTER TABLE ONLY intervals
    ADD CONSTRAINT intervals_pk PRIMARY KEY (seqname, t1, t2);



--
-- Name: sequences_pk; Type: CONSTRAINT; Schema: test; Owner: -
--
ALTER TABLE ONLY sequences
    ADD CONSTRAINT sequences_pk PRIMARY KEY (seqname);


--
-- Name: sequences_unq; Type: CONSTRAINT; Schema: test; Owner: -
--
ALTER TABLE ONLY sequences
    ADD CONSTRAINT sequences_unq UNIQUE (seqnum);



--
-- Name: sequences_fk; Type: FK CONSTRAINT; Schema: test; Owner: -
--
ALTER TABLE ONLY intervals
    ADD CONSTRAINT sequences_fk FOREIGN KEY (seqname) REFERENCES sequences(seqname) ON UPDATE CASCADE ON DELETE RESTRICT;

--
-- PostgreSQL database schema complete
--


--
-- PostgreSQL register schema
--

-- Insert initial data
INSERT INTO public.selections(selname, dataset) VALUES ('test.intervals', 'test');

-- Please, specify the dataset location within your datasets, include the trailing / (file separator) please
INSERT INTO public.datasets(dsname, dslocation) VALUES ('test', 'test/');

