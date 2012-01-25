#!/usr/bin/python

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


def getStatusRecord(data):
  status = {}
  try:
    dom = xml.dom.minidom.parseString(data)
    item = dom.getElementsByTagName('item')[0]
    print '****', item

    for child in item.childNodes:
      name = child.tagName
      if child.childNodes:
        text = child.childNodes[0].wholeText
        if name == 'title' and text:
          text = FixText.fixTitle(text)
        status[name] = text

  except:
    traceback.print_exc(file=sys.stdout)

  return dict((k, status.get(k, d)) for k, d in JSON_FIELDS.iteritems())


class StatusJob(Job.Job):
  MAX_TITLES = 20

  API = twitter.Api(
    consumer_key = Secret.consumer_key,
    consumer_secret = Secret.consumer_secret,
    access_token_key = Secret.access_token_key,
    access_token_secret = Secret.access_token_secret)

  def __init__(self):
    Job.Job.__init__(self, Config.STATUS, self._process)
    self.output = self.output or {}

  def _process(self, data):
    def getTitle(out):
      return (out or {}).get('title', None)

    output = getStatusRecord(data)
    print output
    title = getTitle(output)
    print title, getTitle(self.output)
    if title == getTitle(self.output):
      return self.output

    if title:
      titleList = self.output.get('titleList', [])
      index = (1 + titleList[0]['index']) if titleList else 0
      titleList.insert(0, {'index': index, 'title': title})
      while len(titleList) > StatusJob.MAX_TITLES:
        titleList.pop()
      self.output['titleList'] = titleList
    return output

  def onOutputChanged(self, output):
    if Config.POST_TO_TWITTER and output and output.title:
      if not self.output or (self.output.get('title', None) != output.title):
        StatusJob.API.PostUpdate(output.title)
    Job.Job.onOutputChanged(self, output)
