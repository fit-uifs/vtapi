
DROP TABLE IF EXISTS [datasets];
DROP TABLE IF EXISTS [processes];
DROP TABLE IF EXISTS [methods_keys];
DROP TABLE IF EXISTS [methods];
DROP TABLE IF EXISTS [selections];

-- created, changed = timestamp
-- tabulka se seznamem datasetù
CREATE TABLE [datasets] (
    [dsname] TEXT NOT NULL,
    [dslocation] TEXT,
    [userid] TEXT,
    [groupid] TEXT,
    [created] TIMESTAMP,
    [changed] TIMESTAMP,
    [notes] TEXT,
    CONSTRAINT [dataset_pk] PRIMARY KEY ([dsname])
);

-- created = timestamp
-- tabulka s dostupnými metodami
CREATE TABLE [methods] (
    [mtname] TEXT NOT NULL,
    [userid] TEXT,
    [created] TEXT,
    [notes] TEXT,
    CONSTRAINT [methods_pk] PRIMARY KEY ([mtname])
);

-- typname = databazovy typ
-- tabulka s parametry metod
CREATE TABLE [methods_keys] (
    [mtname] TEXT NOT NULL,
    [keyname] TEXT NOT NULL,
    [typname] TEXT NOT NULL,
    [inout] TEXT NOT NULL,
    CONSTRAINT [methods_keys_pk] PRIMARY KEY ([mtname], [keyname]),
    CONSTRAINT [methods_keys_mtname_fk] FOREIGN KEY ([mtname])
			REFERENCES [methods]([mtname])
);

-- inputs, outputs = nazev sloupce nejake tabulky
-- created = timestamp
-- tabulka s definovanými procesy
CREATE TABLE [processes] (
    [mtname] TEXT,
    [prsname] TEXT NOT NULL,
    [inputs] TEXT,
    [outputs] TEXT,
    [userid] TIMESTAMP,
    [created] TIMESTAMP,
    [notes] TEXT,
    CONSTRAINT [processes_pk] PRIMARY KEY (prsname),
    CONSTRAINT [method_fk] FOREIGN KEY ([mtname])
			REFERENCES [methods]([mtname]) ON UPDATE CASCADE ON DELETE RESTRICT
);

-- selname = nazev sloupce nejake tabulky
-- created = timestamp
-- tabulka selekcí
CREATE TABLE [selections] (
    [selname] TEXT NOT NULL,
    [dataset] TEXT NOT NULL,
    [userid] TEXT,
    [created] TIMESTAMP,
    [notes] TEXT,
    CONSTRAINT [selections_pk] PRIMARY KEY ([selname])
);



