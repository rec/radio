#!/usr/local/bin/python

import daemon
import json
import time

import Config
import File

class Job(object):
  def __init__(self, desc):
    self.desc = desc
    data = File.readFile(self.dataFile()) or '{}'
    self.output = json.loads(data)

  def dataFile(self):
    return Config.GENERATED_FILES + self.desc.filepath

  def run(self, time):
    if not (time % self.desc.interval):
      self.doRun()
    return (-(time + 1) % self.desc.interval) + 1

  def doRun(self):
    url = Config.ROOT_URL + self.desc.url
    output = self.process(File.readUrl(url))
    if output != self.output:
      self.onOutputChanged(output)

  def runAndWait(self):
    self.doRun()
    time.sleep(self.desc.interval)

  def onOutputChanged(self, output):
    File.replaceAtomic(self.dataFile(), json.dumps(output))
    self.output = output

  def runAsDaemon(self):
    pass


def runJobs(*jobs):
  t = 0
  while True:
    delta = min(*[job.run(t) for job in jobs])
    t += delta
    time.sleep(delta)

    reload(Config)
