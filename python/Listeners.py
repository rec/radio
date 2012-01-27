#!/usr/bin/python

import operator
import sys
import traceback

import BeautifulSoup

import Config
import File
import Geocode
import Job

def getRawListeners(data):
  listeners = []
  if data:
    try:
      contents = BeautifulSoup.BeautifulSoup(data).find(id='listenerTable')
      for c in contents:
        try:
          def get(i):
            return c.contents[i].contents[0].extract()
          listeners.append(dict(ip=get(1), time=get(3), client=get(5)))
        except:
          pass
    except:
      print "Error in parsing: "
      traceback.print_exc(file=sys.stdout)

  return sorted(listeners[1:], key=operator.itemgetter('time'))


class ListenerJob(Job.Job):
  def __init__(self):
    Job.Job.__init__(self, Config.LISTENER)
    self.output = self.output or {}

  def process(self, data):
    return getRawListeners(data);


LISTENERS = {'broadcast' : Config.BROADCASTER,
             'listeners' : {}};

