
DROP TABLE IF EXISTS [intervals];
DROP TABLE IF EXISTS [sequences];

ATTACH DATABASE 'vtapi_public.db' AS public;

-- created, changed = timestamp
-- tabulka sekvencí
CREATE TABLE [sequences] (
    [seqname] TEXT NOT NULL,
    [seqnum] INTEGER,
    [seqlocation] TEXT,
    [seqtyp] TEXT,
    [userid] TEXT,
    [groupid] TEXT,
    [created] TIMESTAMP,
    [changed] TIMESTAMP,
    [notes] TEXT,
    CONSTRAINT [sequences_pk] PRIMARY KEY ([seqname]),
    CONSTRAINT [sequences_unq] UNIQUE ([seqnum])  
);


-- tags = array of INTEGERs
-- svm = array of REALs
-- Process_name = array of REALs
-- created = timestamp
-- tabulka intervalù
CREATE TABLE [intervals] (
    [seqname] TEXT NOT NULL,
    [t1] INTEGER NOT NULL,
    [t2] INTEGER NOT NULL,
    [imglocation] TEXT,
    [userid] TEXT,
    [created] TIMESTAMP,
    [notes] TEXT,
    CONSTRAINT [intervals_pk] PRIMARY KEY ([seqname], [t1], [t2]),
    CONSTRAINT [sequences_fk] FOREIGN KEY ([seqname])
			REFERENCES [sequences]([seqname]) ON UPDATE CASCADE ON DELETE RESTRICT
);

-- vlo¾ení do seznamu datasetù
INSERT INTO [public].[selections]([selname],[dataset])
		VALUES ('vidte.intervals','vidte');
-- definice tabulky intervals jako selection
INSERT INTO [public].[datasets]([dsname],[dslocation])
		VALUES ('vidte','vidte/');


CREATE TABLE [seqnum_seq] (
    [id] INTEGER,
    [value] INTEGER,
    CONSTRAINT [seqnum_seq_pk] PRIMARY KEY (id)
);

INSERT INTO [seqnum_seq]([id],[value]) VALUES (0,0);


