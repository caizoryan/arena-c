# Architecture

### General
The program can be given a list of slugs, or a single slug for a channel. This will start a loop to asynchronously curl JSON responses from are.na's API. Once response is recieved, it is parsed into structs using cJSON and then added to a sqlite DB. This DB then can be queried later for channel. If a channel is available in the DB, it will be returned unless --force/-f is enabled, then channel is refreshed.
