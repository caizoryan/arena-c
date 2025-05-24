# <2025-05-23 Fri> Today:

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

# Next up?
- Figure out how to setup a debugger
- Connect to a sqlite db?
