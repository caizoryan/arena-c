# <2025-07-05 Fri> Breakdown:
### Return
Turns out. Leaks wasn't working becuase it was segfaulting! And it just failed to inform me of this. If you run the program normally, it would let me know, however I assumed leaks would warn as well. Anyways, I figured out the issues, also learned how to use a debugger (lldb), debuggers are awesome! Time to start using them with javascript too...

### Parsing
- Can go through channel contents from the req
- read them into an array and pack it with the channel along with a contents len
- and clean up works as well, no leaks as of yet.

### Next up
- Add this stuff to the db


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
