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

insert into channel values(9932, 'funky title', 'funky-title-sevas8s9', 0, 2, 'private', '', '');
