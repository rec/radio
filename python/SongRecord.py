#!/usr/bin/python

import os
import urllib2
import xml.dom.minidom

import FixText

JSON_FIELDS = (('online', '1'), 
               ('title', ''), 
               ('listeners', '0'), 
               ('unique', '0'), 
               ('bitrate', '128'), 
               ('error', ''))


def getSongRecord(url):
  song = {}

  dom = xml.dom.minidom.parse(urllib2.urlopen(url))
  items = dom.getElementsByTagName('item')
  if items:
    for child in items[0].childNodes:
      name = child.tagName
      if child.childNodes:
        text = child.childNodes[0].wholeText
        if name == 'title':
          text = FixText.fixSongTitle(text)
        song[name] = text
  return song


def getJson(song, **kwds):
  a = dict((k, song.get(k, d)) for k, d in JSON_FIELDS)
  a.update(**kwds)
  return a
