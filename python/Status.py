#!/usr/bin/python

import json
import os
import twitter
import urllib2
import xml.dom.minidom

import Config
import File
import FixText
import Secret

JSON_FIELDS = (('online', '1'),
               ('title', ''),
               ('listeners', '0'),
               ('unique', '0'),
               ('bitrate', '128'),
               ('error', ''))


PREVIOUS_TITLE = readFile(Config.TITLE_FILE)

def getStatusRecord():
  status = {}

  try:
    dom = xml.dom.minidom.parse(urllib2.urlopen(Config.STATUS_DATA_URL))
    items = dom.getElementsByTagName('item')
    if items:
      for child in items[0].childNodes:
        name = child.tagName
        if child.childNodes:
          text = child.childNodes[0].wholeText
          if name == 'title':
            text = FixText.fixStatusTitle(text)
          status[name] = text
  except:
    print "Couldn't read status record for", Config.STATUS_DATA_URL)

  return status


def getJson(status, **kwds):
  a = dict((k, status.get(k, d)) for k, d in JSON_FIELDS)
  a.update(**kwds)
  return a


def onNewStatus(status, title):
  scroller = Scroller.scroll(readFile(Config.SCROLLER_FILE), title)
  if scroller:
    File.replaceAtomic(Config.SCROLLER_FILE, scroller)
    status = StatusRecord.getJson(status, scroller=scroller)
    if status:
      File.replaceAtomic(Config.JSON_FILE, json.dumps(status))


def getStatus(onNewStatus):
  status = getStatusRecord(URL)
  title = status.get('title', '')
  if title and title != PREVIOUS_TITLE:
    global PREVIOUS_TITLE
    PREVIOUS_TITLE = title
    File.replaceAtomic(Config.TITLE_FILE, title)

    onNewStatus(status, title)

  return title

      status = Status.getNewStatus()
      if status and status.title:
        Config.log('New status %s', status.title)
        if Config.POST_TO_TWITTER:
          API.PostUpdate(status.title)

API = twitter.Api(
  consumer_key = Secret.consumer_key,
  consumer_secret = Secret.consumer_secret,
  access_token_key = Secret.access_token_key,
  access_token_secret = Secret.access_token_secret)


class StatusJob(object):
  def __init__(self):
    self.interval = Config.STATUS_INTERVAL
    self.callback = getStatus

