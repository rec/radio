#!/usr/bin/python

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
  'online': '0',
  'title': '',
  'titleList': [],
  'listeners': '0',
  'unique': '0',
  'bitrate': '128',
  'error': ''}


def getRawStatusRecords(data, status):
  dom = xml.dom.minidom.parseString(data)
  item = dom.getElementsByTagName('item')[0]

  for child in item.childNodes:
    name = child.tagName
    if child.childNodes:
      text = child.childNodes[0].wholeText
      if name == 'title' and text:
        text = FixText.fixTitle(text)
      status[name] = text

def getStatusRecord(data):
  status = {}
  if data:
    try:
      getRawStatusRecords(data, status)
    except:
      traceback.print_exc(file=sys.stdout)

  return dict((k, status.get(k, d)) for k, d in JSON_FIELDS.iteritems())


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
    if Config.POST_TO_TWITTER and output and output.title:
      if not self.output or (self.output.get('title', None) != output.title):
        StatusJob.API.PostUpdate(output.title)
    Job.Job.onOutputChanged(self, output)
