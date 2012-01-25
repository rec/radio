#!/usr/bin/python

import operator

import BeautifulSoup

import Config
import Job

def getListeners(data):
  listeners = []
  try:
    contents = BeautifulSoup.BeautifulSoup(data).find(id='listenerTable')
    for c in contents:
      def get(i):
        return c.contents[i].contents[0].extract()
      try:
        listeners.append(dict(ip=get(1), time=get(3), client=get(5)))
      except:
        pass
  except:
    pass

  if len(listeners) > 1:
    return sorted(listeners[1:], key=operator.itemgetter('time'))
  else:
    return listeners

def listenerJob():
  return Job.Job(Config.LISTENER, getListeners)
