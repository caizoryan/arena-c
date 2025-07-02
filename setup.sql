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
    updated_at TEXT NOT NULL,
    created_at TEXT NOT NULL,
    comment_count INTEGER NOT NULL,
    generated_title TEXT,
    class TEXT NOT NULL,
    base_class TEXT NOT NULL,
    content TEXT,
    content_html TEXT,
    description TEXT,
    description_html TEXT,
    owner_id INTEGER NOT NULL
)

CREATE TABLE IF NOT EXISTS connections (
    id INTEGER PRIMARY KEY NOT NULL UNIQUE,
    position INTEGER NOT NULL,
    connected_at TEXT NOT NULL,
    connected_by_user_id INTEGER NOT NULL,
    parent_id INTEGER NOT NULL,
    channel_id INTEGER NOT NULL,
    block_id INTEGER NOT NULL,
    downloaded TEXT NOT NULL
)


insert into channel values(9932, 'funky title', 'funky-title-sevas8s9', 0, 2, 'private', '', '');
