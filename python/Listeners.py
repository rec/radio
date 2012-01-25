#!/usr/bin/python

import operator
import sys
import traceback

import BeautifulSoup

import Config
import Job

def getListeners(data):
  listeners = []
  if data:
    try:
      contents = BeautifulSoup.BeautifulSoup(data).find(id='listenerTable')
      for c in contents:
        def get(i):
          return c.contents[i].contents[0].extract()
        listeners.append(dict(ip=get(1), time=get(3), client=get(5)))
    except:
      print "Error in parsing: "
      traceback.print_exc(file=sys.stdout)

  return sorted(listeners[1:], key=operator.itemgetter('time'))

def listenerJob():
  return Job.Job(Config.LISTENER, getListeners)
