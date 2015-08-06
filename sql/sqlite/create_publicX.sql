
DROP TABLE IF EXISTS [datasets];
DROP TABLE IF EXISTS [processes];
DROP TABLE IF EXISTS [methods_keys];
DROP TABLE IF EXISTS [methods];
DROP TABLE IF EXISTS [selections];
DROP TABLE IF EXISTS [inouttype];
DROP TABLE IF EXISTS [seqtype];

-- created, changed = timestamp
CREATE TABLE [datasets] (
    [dsname] TEXT NOT NULL,
    [dslocation] TEXT,
    [userid] TEXT,
    [groupid] TEXT,
    [created] TEXT,
    [changed] TEXT,
    [notes] TEXT,
    CONSTRAINT [dataset_pk] PRIMARY KEY ([dsname])
);

-- created = timestamp
CREATE TABLE [methods] (
    [mtname] TEXT NOT NULL,
    [userid] TEXT,
    [created] TEXT,
    [notes] TEXT,
    CONSTRAINT [methods_pk] PRIMARY KEY ([mtname])
);

-- typname = databazovy typ
-- inout = enum z inouttype
CREATE TABLE [methods_keys] (
    [mtname] TEXT NOT NULL,
    [keyname] TEXT NOT NULL,
    [typname] TEXT NOT NULL,
    [inout] integer NOT NULL,
    CONSTRAINT [methods_keys_pk] PRIMARY KEY ([mtname], [keyname]),
    CONSTRAINT [methods_keys_mtname_fk] FOREIGN KEY ([mtname]) REFERENCES [methods]([mtname])
);

-- inputs, outputs = nazev sloupce nejake tabulky
-- created = timestamp
CREATE TABLE [processes] (
    [mtname] TEXT,
    [prsname] TEXT NOT NULL,
    [inputs] TEXT,
    [outputs] TEXT,
    [userid] TEXT,
    [created] TEXT,
    [notes] TEXT,
    CONSTRAINT [processes_pk] PRIMARY KEY (prsname),
    CONSTRAINT [method_fk] FOREIGN KEY ([mtname]) REFERENCES [methods]([mtname]) ON UPDATE CASCADE ON DELETE RESTRICT
);

-- selname = nazev sloupce nejake tabulky
-- created = timestamp
CREATE TABLE [selections] (
    [selname] TEXT NOT NULL,
    [dataset] TEXT NOT NULL,
    [userid] TEXT,
    [created] TEXT,
    [notes] TEXT,
    CONSTRAINT [selections_pk] PRIMARY KEY ([selname])
);


CREATE TABLE [inouttype] (
    [id] INTEGER,
    [value] TEXT,
    CONSTRAINT [inouttype_pk] PRIMARY KEY ([id])
);

CREATE TABLE [seqtype] (
    [id] INTEGER,
    [value] TEXT,
    CONSTRAINT [seqtype_pk] PRIMARY KEY ([id])
);

BEGIN TRANSACTION;

INSERT INTO [inouttype]([id],[value]) VALUES (0,'in');
INSERT INTO [inouttype]([id],[value]) VALUES (1,'inout');
INSERT INTO [inouttype]([id],[value]) VALUES (2,'out');

INSERT INTO [seqtype]([id],[value]) VALUES (0,'video');
INSERT INTO [seqtype]([id],[value]) VALUES (1,'images');
INSERT INTO [seqtype]([id],[value]) VALUES (2,'data');

END TRANSACTION;



