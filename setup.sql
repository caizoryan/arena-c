CREATE TABLE channel (
    id INTEGER PRIMARY KEY NOT NULL UNIQUE,
    title TEXT NOT NULL,
    slug TEXT NOT NULL,

    length INTEGER NOT NULL,
    user_id INTEGER NOT NULL,

    status TEXT NOT NULL,

    created_at TEXT NOT NULL,
    updated_at TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS block (
    id INTEGER PRIMARY KEY NOT NULL UNIQUE,
    title TEXT,
    class TEXT NOT NULL,
    content TEXT NOT NULL,
    base_class TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS connections (
    id INTEGER PRIMARY KEY NOT NULL UNIQUE,
    position INTEGER NOT NULL,
    parent_id INTEGER NOT NULL,
    channel_id INTEGER NOT NULL,
    block_id INTEGER NOT NULL
);

insert into channel values(9932, 'funky title', 'funky-title-sevas8s9', 0, 2, 'private', '', '');
