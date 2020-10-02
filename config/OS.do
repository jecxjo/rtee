#!/usr/bin/env bash

echo $(uname) >$3
redo-always
redo-stamp <$3

exit 0
