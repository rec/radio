#!/bin/bash

EZSTREAM=/usr/local/bin/ezstream

FILE_PATH=/home/radio/audio
HOME=/home/radio/radio/ezstream
EZSTREAM_SCRIPT=$HOME/ezstream-test.xml
EZSTREAM_RESULT=$HOME/ezstream_files.txt
LOG_FILE=/home/radio/logs/ezstream/`date +%Y-%m-%d-%H-%M`.log

$EZSTREAM -vvc $EZSTREAM_SCRIPT

# )  2>&1 > $LOG_FILE
