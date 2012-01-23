#!/usr/bin/python

import json
import os
import time
import twitter

import ClientInfo
import Config
import File
import Scroller
import Secret
import SongRecord

API = twitter.Api(
  consumer_key = Secret.consumer_key,
  consumer_secret = Secret.consumer_secret,
  access_token_key = Secret.access_token_key,
  access_token_secret = Secret.access_token_secret)

def onNewSong(song, title):
  File.replaceAtomic(TITLE_FILE, title)

  scroller = Scroller.scroll(readFile(SCROLLER_FILE), title)
  if scroller:
    File.replaceAtomic(SCROLLER_FILE, scroller)
    song = SongRecord.getJson(song, scroller=scroller)
    if song:
      File.replaceAtomic(JSON_FILE, json.dumps(song))

  if POST_TO_TWITTER:
    API.PostUpdate(title)

def getSong(previousTitle):
  song = SongRecord.getSongRecord(URL)
  title = song.get('title', '')
  if title and title != previousTitle:
    previousTitle = title
    onNewSong(song, title)

  return title

def run(previousTitle):
  t = 0
  while True:
    clocks = t % INTERVAL, t % CLIENT_INTERVAL
    if not clocks[0]:
      previousTitle = getSong(previousTitle)
    if not clocks[1]:
      ClientInfo.getClients()

    delta = min(INTERVAL - clocks[0], CLIENT_INTERVAL - clocks[1])
    time.sleep(delta)
    t += delta

run(readFile(TITLE_FILE))

