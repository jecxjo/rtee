# rtee

Read from standard input and write to standard output and rotating files.

`rtee` is similar to the standard unix command `tee` with the added option of
rotating files. Supplying a max number of bytes and a number of previous
files, `rtee` can be used for log output without the worry of the output
filling up a file system.

## Install

Build is straight forward using the `Makefile`.

    make
    make install

## Synopsis

rtee [OPTION]... [FILE]...

## Description

    -a       Append to the given FILE.

    -b num   Maximum number of bytes per file (default 1M)

    -f num   Maximum number of previous files before rotating (default 4)

    File names append the rotation number, for example `rtee FILE` will generate
    `FILE.0 FILE.1 FILE.2`. When the max number of previous files is hit the
    oldest is deleted.

## Example

To create 5 backup files with a size of 10KB or less of file listings:

    find / | rtee -b 10000 -f 5 listing.txt

This will output files named `listing.txt.N` with 6 files, the highest
number being the latest.

## Credits

Idea was derived from the BSD `tee` utility.

Copyright (c) 1988, 1993 The Regents of the University of California.
