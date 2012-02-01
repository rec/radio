#!/usr/local/bin/python

import datetime
import json
import os
import sys
import traceback
import twitter
import urllib2
import xml.dom.minidom

import Config
import File
import FixText
import Job
import Secret

JSON_FIELDS = {
  'title': '',
  'titleList': [],
  'listeners': '0',
  'unique': '0',
  'bitrate': '128'}

RUNNING = 'running'
NOT_RUNNING = 'not running'
NO_STREAM = 'no stream'

def getRawStatusRecords(data, status):
  if not data:
    return NOT_RUNNING
  
  dom = xml.dom.minidom.parseString(data)
  items = dom.getElementsByTagName('item')
  if not items:
    return NO_STREAM

  item = items[0]

  for child in item.childNodes:
    name = child.tagName
    if child.childNodes:
      text = child.childNodes[0].wholeText
      if name == 'title' and text:
        text = FixText.swapParts(text)
      status[name] = text

  return ''

def getStatusRecord(data):
  statusRecord = {}
  error = getRawStatusRecords(data, statusRecord)
  if error:
    return dict(error=error)
  else:
    return dict((k, statusRecord.get(k, d)) for k, d in JSON_FIELDS.iteritems())


class StatusJob(Job.Job):
  API = twitter.Api(
    consumer_key = Secret.consumer_key,
    consumer_secret = Secret.consumer_secret,
    access_token_key = Secret.access_token_key,
    access_token_secret = Secret.access_token_secret)

  def __init__(self):
    Job.Job.__init__(self, Config.STATUS)
    self.output = self.output or {}

  def process(self, data):
    def getTitle(out):
      return (out or {}).get('title', None)

    output = getStatusRecord(data)
    title = getTitle(output)
    if title == getTitle(self.output):
      return self.output

    titleList = self.output.get('titleList', [])
    if title:
      index = (1 + titleList[0]['index']) if titleList else 0
      time = time=datetime.datetime.now().strftime('%H:%M')
      titleList.insert(0, {'index': index, 'title': title, 'time': time})
      while len(titleList) > Config.MAX_TITLES:
        titleList.pop()

    output['titleList'] = titleList
    return output

  def onOutputChanged(self, output):
    if Config.POST_TO_TWITTER and output:
      t = output.get('title', None)
      if t and (not self.output or (self.output.get('title', None) != t)):
        File.replaceAtomic(Config.STATUS_TITLE_FILE, json.dumps({'title': t}))
        try:
          StatusJob.API.PostUpdate(FixText.fitToSize(t))
        except:
          traceback.print_exc(file=sys.stdout)

    Job.Job.onOutputChanged(self, output)
