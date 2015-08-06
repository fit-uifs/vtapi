
DROP TABLE IF EXISTS [seqnum_seq];
DROP TABLE IF EXISTS [intervals];
DROP TABLE IF EXISTS [sequences];

ATTACH DATABASE 'vtapi_public.db' AS public;

-- seqtyp = enum z public.seqtype
-- created, changed = timestamp
CREATE TABLE [sequences] (
    [seqname] TEXT NOT NULL,
    [seqnum] INTEGER,
    [seqlocation] TEXT,
    [seqtyp] INTEGER DEFAULT 2,
    [userid] TEXT,
    [groupid] TEXT,
    [created] TEXT,
    [changed] TEXT,
    [notes] TEXT,
    CONSTRAINT [sequences_pk] PRIMARY KEY ([seqname]),
    CONSTRAINT [sequences_unq] UNIQUE ([seqnum]),
    CONSTRAINT [seqtyp_fk] FOREIGN KEY ([seqtyp]) REFERENCES [public.seqtype](id) ON UPDATE CASCADE ON DELETE RESTRICT    
);


-- tags = array of INTEGERs
-- svm = array of REALs
-- Process_name = array of REALs
-- created = timestamp
CREATE TABLE [intervals] (
    [seqname] TEXT NOT NULL,
    [t1] INTEGER NOT NULL,
    [t2] INTEGER NOT NULL,
    [imglocation] TEXT,
    [tags] BLOB,
    [svm] BLOB,
    [userid] TEXT,
    [created] TEXT,
    [notes] TEXT,
    [Process_name] BLOB,
    CONSTRAINT [intervals_pk] PRIMARY KEY ([seqname], [t1], [t2]),
    CONSTRAINT [sequences_fk] FOREIGN KEY ([seqname]) REFERENCES [sequences]([seqname]) ON UPDATE CASCADE ON DELETE RESTRICT
);


CREATE TABLE [seqnum_seq] (
    [id] INTEGER,
    [value] INTEGER,
    CONSTRAINT [seqnum_seq_pk] PRIMARY KEY (id)
);

BEGIN TRANSACTION;

INSERT INTO [seqnum_seq]([id],[value]) VALUES (0,0);
INSERT INTO [public].[selections]([selname],[dataset]) VALUES ('vidte.intervals','vidte');
INSERT INTO [public].[datasets]([dsname],[dslocation]) VALUES ('vidte','vidte/');

END TRANSACTION;


