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

JSON_FIELDS = {
  'online': '1',
  'title': '',
  'listeners': '0',
  'unique': '0',
  'bitrate': '128',
  'error': ''}


def getStatusRecord(data):
  status = {}
  try:
    dom = xml.dom.minidom.parse(data)
    items = dom.getElementsByTagName('item')
    for child in items[0].childNodes:
      name = child.tagName
      text = child.childNodes[0].wholeText
      if name == 'title':
        text = FixText.fixStatusTitle(text)
      status[name] = text

  except Exception as e:
    print "Couldn't read status record for", Config.STATUS_DATA_URL
    print e

  return dict((k, status.get(k, d)) for k, d in JSON_FIELDS.iteritems())


class StatusJob(Job.Job):
  MAX_TITLES = 20

  API = twitter.Api(
    consumer_key = Secret.consumer_key,
    consumer_secret = Secret.consumer_secret,
    access_token_key = Secret.access_token_key,
    access_token_secret = Secret.access_token_secret)

  def __init__(self):
    Job.Job.init(self, Config.STATUS, self._process)
    self.output = self.output or {}

  def _process(self, data):
    output = getStatusRecord(data)
    if output:
      titleList = self.output.get('titleList', [])
      index = (1 + tl[0]['index']) if tl else 0
      titleList.insert(0, {'index': index, 'title': output.title})
      while len(titleList) > StatusJob.MAX_TITLES:
        titleList.pop()
      self.output['titleList'] = titleList
    return output

  def onOutputChanged(self, output):
    if Config.POST_TO_TWITTER and output and output.title:
      if not self.output or (self.output.get('title', None) != output.title):
        StatusJob.API.PostUpdate(output.title)
    Job.Job.onOutputChanged(self, output)





