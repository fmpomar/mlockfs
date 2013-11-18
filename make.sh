#! /bin/sh
gcc -Wall `pkg-config fuse --cflags --libs` linkedlist.c tree.c main.c file.c dir.c -o main -ggdb
