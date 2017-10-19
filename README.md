# mlockfs

The in-locked-memory filesystem

### What is mlockfs?

mlockfs is a very simple in-ram filesystem that runs in
user space (with FUSE) and stores all of its data in memory locked (mlock) pages.

### Why mlock pages?

Well, it seemed a useful idea to have some guarantee that the contents of a filesystem
are never going to be paged out to disk...

You might need an instant but infrequently accessed cache,
or maybe just somewhere to store sensitive information (like encryption keys)
that should never end up written in a (probably unencrypted) disk.

## Dependencies
- FUSE (+ libfuse-dev for building)

## Building

run `./make.sh`

## Running

./main

## Notes

This is a PoC and coursework for an operating systems course.
