create table TODOS(
  Id integer PRIMARY KEY NOT NULL,
  Title varchar(128) NOT NULL,
  Completed integer NOT NULL
);

CREATE TABLE channel (
    id INTEGER PRIMARY KEY NOT NULL UNIQUE,
    title TEXT NOT NULL,
    slug TEXT NOT NULL
);

insert into TODOS values(NULL, 'code are.na shit', 0);
insert into channel values(9932, 'funky title', 'funky-title-sevas8s9');
