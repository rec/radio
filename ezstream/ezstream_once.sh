#!/bin/bash

EZSTREAM=/usr/local/bin/ezstream

HOME=/home/radio/radio
FILE_PATH=$HOME/audio
EZSTREAM_SCRIPT=$HOME/ezstream/ezstream.xml
EZSTREAM_RESULT=$HOME/ezstream/ezstream_files.txt

echo "find $FILE_PATH -name \*.mp3 \! -type l > $EZSTREAM_RESULT"
find $FILE_PATH -name \*.mp3 \! -type l | $EZSTREAM -vvc $EZSTREAM_SCRIPT
