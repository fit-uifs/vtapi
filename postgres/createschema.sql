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

SET search_path = test, public, pg_catalog;
      
--vytvoøení schématu      
CREATE schema vidte;

-- tabulka sekvencí
CREATE TABLE sequences (
    seqname name NOT NULL,
    seqnum integer,
    seqlocation character varying,
    seqtyp public.seqtype DEFAULT 'data',
    svm float4[],
    userid name,
    groupid name,
    created timestamp without time zone,
    changed timestamp without time zone,
    notes text,
    CONSTRAINT sequences_pk PRIMARY KEY (seqname),
    CONSTRAINT sequences_unq UNIQUE (seqnum)
);

-- tabulka intervalù
CREATE TABLE intervals (
    seqname character varying NOT NULL,
    t1 integer NOT NULL,
    t2 integer NOT NULL,
    imglocation character varying,
    userid name,
    created timestamp without time zone DEFAULT now(),
    notes text,
    CONSTRAINT intervals_pk PRIMARY KEY (seqname, t1, t2),
    CONSTRAINT sequences_fk FOREIGN KEY (seqname)
			REFERENCES sequences(seqname) ON UPDATE CASCADE ON DELETE RESTRICT
);

-- vlo¾ení do seznamu datasetù
INSERT INTO public.datasets(dsname, dslocation)
	VALUES ('vidte', 'data/vidte/');
-- definice tabulky intervals jako selection
INSERT INTO public.selections(selname, dataset)
	VALUES ('vidte.intervals', 'vidte');
 
--
-- Generovani ID
--
CREATE SEQUENCE sequences_seqnum_seq;
ALTER TABLE ONLY sequences
    ALTER COLUMN seqnum
        SET DEFAULT NEXTVAL('sequences_seqnum_seq');


CREATE TYPE cvmat AS (
		type integer,
		dims integer,
		step_arr integer[],
		rows integer,
		cols integer,
		data_loc character varying
);

ATTACH DATABASE vtapi_vidte.db AS 'vidte';

-- vlo¾ení sekvence do datasetu vidte
INSERT INTO vidte.sequences (name, seqlocation, seqtyp)
		VALUES ($1, $2, $3);
