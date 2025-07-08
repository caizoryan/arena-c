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

CREATE TABLE IF NOT EXISTS groups (
    id INTEGER PRIMARY KEY NOT NULL UNIQUE,
    title TEXT NOT NULL
)

CREATE TABLE IF NOT EXISTS users (
    id INTEGER PRIMARY KEY NOT NULL UNIQUE,
    slug TEXT NOT NULL,
    username TEXT NOT NULL,
    avatar TEXT NOT NULL,
    created_at TEXT NOT NULL,
    first_name TEXT NOT NULL,
    last_name TEXT NOT NULL,
    channel_count INTEGER NOT NULL,
    following_count INTEGER NOT NULL,
    follower_count INTEGER NOT NULL
)

CREATE TABLE IF NOT EXISTS sources (
    id INTEGER PRIMARY KEY NOT NULL UNIQUE,
    url TEXT NOT NULL
)

CREATE TABLE IF NOT EXISTS image_data (
    id INTEGER PRIMARY KEY NOT NULL UNIQUE,
    filename TEXT,
    content_type TEXT,
    large_url TEXT NOT NULL,
    original_url TEXT NOT NULL,
    display_url TEXT NOT NULL,
    thumb_url TEXT NOT NULL,
    square_url TEXT NOT NULL
)

CREATE TABLE IF NOT EXISTS attachments (
    id INTEGER PRIMARY KEY NOT NULL UNIQUE,
    file_name TEXT NOT NULL,
    file_size INTEGER NOT NULL,
    file_size_display TEXT NOT NULL,
    content_type TEXT NOT NULL,
    extension TEXT NOT NULL,
    url TEXT NOT NULL,
    FOREIGN KEY (id) REFERENCES block(id)
)

