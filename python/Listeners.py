#!/usr/bin/python

import operator

import BeautifulSoup

import Config
import File

class Job(object):
  def __init__(self):
    self.interval = Config.LISTENER_INTERVAL
    self.url = Config.LISTENER_URL
    self.file = Config.LISTENER_FILE
    self.output = None

  def Run(self, data):
    listeners = []
    contents = BeautifulSoup.BeautifulSoup(data).find(id='listenerTable')
    for c in contents:
      try:
        def get(i):
          return c.contents[i].contents[0].extract()
        listeners.append(dict(ip=get(1), time=get(3), client=get(5)))
      except:
        pass

    if len(listeners) > 1
      return sorted(listeners[1:], key=operator.itemgetter('time'))
    else:
      return listeners

