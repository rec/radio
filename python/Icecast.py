#!/usr/bin/python

import json
import os
import time
import twitter

import ClientInfo
import Scroller
import Secret
import SongRecord

API = twitter.Api(*Secret.secret())

FILL = ' [...] '
URL = 'http://localhost:8000/data.xsl'

ROOT = '/home/radio/public_html/staging/generated/'

TITLE_FILE = ROOT + 'title.txt'
JSON_FILE = ROOT + 'json.html'
SCROLLER_FILE = ROOT + 'scroller.html'
USER_FILE = ROOT + 'users.html'

INTERVAL = 1
CLIENT_INTERVAL = 15

def replaceAtomic(filename, value):
  tmpname = filename + '.tmp'
  f = open(tmpname, 'w')
  f.write(value)
  f.close()
  os.rename(tmpname, filename)

def readFile(f):
  try:
    return open(f).read()
  except:
    return None

def onNewSong(song, title):
  replaceAtomic(TITLE_FILE, title)
  try:
    scroller = Scroller.scroll(readFile(SCROLLER_FILE), title)
    replaceAtomic(SCROLLER_FILE, scroller)
  except:
    # print e
    pass

  try:
    j = json.dumps(SongRecord.getJson(song, scroller=scroller))
    replaceAtomic(JSON_FILE, j)
  except:
    # print e
    pass
  # api.PostUpdate(title)

def run(api, previousTitle):
  i = 0
  while True:
    song = SongRecord.getSongRecord(URL)
    title = song.get('title', '')
    if title and title != previousTitle:
      previousTitle = title
      onNewSong(song, title)
    if not i:
      try:
        replaceAtomic(USER_FILE, json.dumps(list(ClientInfo.getClientInfo())))
      except:
        # print e
        # pass
        raise
    i += 1
    if i >= CLIENT_INTERVAL:
      i = 0

    time.sleep(INTERVAL)

run(API, readFile(TITLE_FILE))

