# <2025-10-05 Thu> Lots of stuff:
### Updates/log
Added loads of new stuff.
- Added table for image and user, will soon add attachment and sources
- Added connections table
- Fixed some json parsing and string processing issues
- Cleaned up repo
- Added 'make clean' in makefile
- added get blocks for a channel
- added cli commands like pull, mount, check, list

##### pull
Pull will download channel into sqlite db
- Can pull slugs given a comma, space, newline separated list.
- Can read from cli, or from a file

##### mount
Mount given a slug (that exists in db) will make a folder for the channel, and add text blocks as .txt files into the folder.
- gets blocks from channel (title, id, content, class)
- checks for folder, and makes it
- creates file
- marks mounted_at in db

##### check
Check will go through folders in root directory and parse files. Files that get parsed as blocks will be checked against file modified at from STAT and mounted at from DB. Based on this they can be marked as modified to be updated.
- reads directory
- parses file for block id
- reads file modified
- get mounted_at attribute from db
- compare and report for update


### Funky diaries
Was having a hard time with utf-8 strings, or so I thought. Whenever I tried processing and adding a channel with hindi text in blocks I would get segmentation fault. I went out for a walk, came back and realised it was faulting cause the channel was private and the request I got back wasn't 200. I don't have checks for that, so will add that!

### Next up
- Parse title from file
- Download images, attachments and media
- Update blocks after modified
- Add max requests per minute

# <2025-07-05 Fri> Return of the dumbass:
### Return
Turns out. Leaks wasn't working becuase it was segfaulting! And it just failed to inform me of this. If you run the program normally, it would let me know, however I assumed leaks would warn as well. Anyways, I figured out the issues, also learned how to use a debugger (lldb), debuggers are awesome! Time to start using them with javascript too...

### Parsing
- Can go through channel contents from the req
- read them into an array and pack it with the channel along with a contents len
- and clean up works as well, no leaks as of yet.
- Add blocks and connections to db
- Sanitize strings for db!

### Next up
- add image, attachment, sources tables
- add user table
- download/generated timestamp for blocks (connections)
- query and get blocks of a channel and compile channel
- clean up and re-organize codebase
  - [db/sqlite]
  - [are.na]
  - [cli]

# <2025-07-03 Thu> Breakdown:
### Emotional
Got super frustrated because leaks stopped working. Leaks was the valgrind alternative that I was using to look for memory leaks. And I couldn't figure out how to make it work. I tried looking for any alternatives but m1 macs are super unsupported with this stuff. Anyways, I made peace with not continuing this project by reflecting that ok, I got out of learning C what I wanted, i.e. to understand how memory works under the hood. How you can get by with a really simple set of tools for programming and just the references, that other programming languages have to C, etc, etc.

Anyways, so that was the end.

# <2025-06-30 Mon> Stuff:
### SQLITE
- Added channel table (copied from old (js) proj)
- Able to add to channel table
- Able to parse json and add to channel table

### Next up
- Add contents to db
  - Make a blocks table
  - Make a connections table
  - Make an image, attachment, source table
  - Process a json from the req, populate and handle contents
  - check for if channel/block already added with id
	- if added check if date before or after...
	
# <2025-05-23 Fri> Basics:
### SQLITE
Added some sqlite stuff. Can add to and read a sqlite db. Now will have funky fun :)

### Next up
Recieve a channel, process it and add its contents to the sqlite db.
When asked respond with the saved stuf...
	
# <2025-05-23 Fri> Up until now:
### Technical
- Learned basics of C
- Figured out how to link libraries (ncurses & libcurl) and header files (cJSON)
- Learned to use leaks (valgrind not available on arm macbooks?)

### Technical (but actual feature)
- Can do a GET http req (Get channel(s))
- Read the response into a memory struct
- Parse the response using a cJSON
- And do appropriate clean up so there are no memory leaks*

*This was somewhat fun, because when leak occured it wasn't that I had to search up and then blindly apply solutions (case when I was learning rust 2 years ago) but rather it was more intuitive to understand. It sort of made sense why something happened. Like for instance the bug that took me a really long time to debug was, I gave pointer to a struct that was allocated on the stack (rookie mistake) and it was working earlier when I was allocating it in main, but the second I moved it to a fn it stopped working. While it wasn't evident instantly, when I figure it out, it just made a lot of sense. C is simple so it makes sense when things don't work, its frustrating but it makes sense.

### Next up?
- Figure out how to setup a debugger
- Connect to a sqlite db?
