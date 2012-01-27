#!/usr/bin/python

import datetime
import operator
import sys
import traceback

import BeautifulSoup

import Config
import File
import Geocode
import Job

EPOCH = datetime.datetime(year=2000, month=1, day=1)

def makeListener(ip, connected, client):
  conn = int(connected.strip())
  now = datetime.datetime.now()
  start = now - datetime.timedelta(seconds=conn)
  since = start.strftime('%H:%M:%S' if start.day is now.day else '%A, %H:%M')
  time = int((start - EPOCH).total_seconds())

  geocode = Geocode.geocode(ip)

  return dict(ip=ip, geocode=geocode, time=time, client=client, since=since)

def getFromHTML(data):
  listeners = []
  if data:
    try:
      contents = BeautifulSoup.BeautifulSoup(data).find(id='listenerTable')
      for c in contents:
        try:
          def get(i):
            return c.contents[i].contents[0].extract()
          listeners.append(dict(ip=get(1), connected=get(3), client=get(5)))
        except:
          pass
    except:
      print "Error in parsing: "
      traceback.print_exc(file=sys.stdout)

  listeners = [makeListener(**li) for li in listeners[1:]]
  return sorted(listeners, key=operator.itemgetter('time'))


class ListenerJob(Job.Job):
  TIME_WINDOW = 5

  def __init__(self):
    Job.Job.__init__(self, Config.LISTENER)
    self.output = self.output or {}
    self.index = 0

  def process(self, data):
    listeners = getFromHTML(data)
    self.merge(listeners)
    return {'listeners': listeners, 'broadcaster' : Config.BROADCASTER}

  def merge(self, listeners):
    oldListeners = self.output.get('listeners', [])[:]
    for i, listener in enumerate(listeners):
      time = int(listener['time'])
      while oldListeners:
        oldListener = oldListeners[0]
        nextTime = int(oldListener['time'])
        delta = nextTime - time
        if delta < -ListenerJob.TIME_WINDOW:
          oldListeners.pop()

        elif delta > ListenerJob.TIME_WINDOW:
          break

        elif oldListener['ip'] == listener['ip']:
          listeners[i] = oldListener
          self.index = max(self.index, oldListener['index'] + 1)
          break

      if 'index' not in listener:
        listener['index'] = self.index
        self.index += 1

