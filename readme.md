# Purpose of this repository

### Learning C
This is my absolute first experience with C. And this seemed like a good exercise to learn C with. There is managing a bunch of strings, getting network requests, parsing and storing the data.

### Learning Emacs (apparently) 
If learning C wasn't destabilizing enough, I'm also learning to use emacs. So far at the point of writing this it has been 2 days, I got evil-mode working so its been a pretty smooth transition. (this has nothing to do with this repo I guess...

;; Maybe by that time I'm finished working with this I'll want to do this in lisp :)

### Making myself a "robust" offline are.na proxy.
I have a sort of working node implementation of this. But I think it will be more fun* to do this in c.

*painful

### Learning CMD line gymnastics
I wanna implement something like the [are.na-cli](https://github.com/ivangreene/arena-cli/blob/master/index.js), and then maybe play around with some sort of org mode integration or smth will be fun. I think just playing with a terminal vibe for are.na will be fun.

# TODO

* all the while making sure there's no leaks and corrupted memory :)

### Basics
- [x] Get single channel
- [x] Parse Channel
- [x] Get multiple channels
- [ ] Store channel in sqlite DB
- [ ] Store block in DB (image, attachment, source, user, etc)
- [ ] Get users channels
- [ ] API Calls limiting
- [ ] Handle errors (maxed out requests, failed response, null values, etc)
- [ ] Manage updating blocks/channels

### Downloading
- [ ] Download images, attachments, etc
- [ ] Resolve updates
- [ ] Folder structure, etc
- [ ] Symlinks (configurable)

### CLI
- [ ] Query Data in db
- [ ] Create an interface for querying

### Server (perhaps another module)
- [ ] Resolve URL request and respond
- [ ] Resolve offline response or proxy req are.na


# Links
1. Checkout the [log](./log.md)
