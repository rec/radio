#!/bin/bash

EZSTREAM=/usr/local/bin/ezstream

HOME=/home/radio/radio
FILE_PATH=$HOME/audio/0
EZSTREAM_SCRIPT=$HOME/ezstream/ezstream.xml
EZSTREAM_RESULT=$HOME/ezstream/ezstream_files.txt

find $FILE_PATH -name \*.mp3 \! -type l | $EZSTREAM -vvc $EZSTREAM_SCRIPT
