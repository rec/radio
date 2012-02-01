#!/usr/local/bin/python

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
      output = self.process(File.readUrl(Config.ROOT_URL + self.desc.url))
      if output != self.output:
        Config.log(str(output) + '!' + str(self.output))
        self.onOutputChanged(output)
    return (-(time + 1) % self.desc.interval) + 1

  def onOutputChanged(self, output):
    File.replaceAtomic(self.dataFile(), json.dumps(output))
    self.output = output


def runJobs(*jobs):
  t = 0
  while True:
    delta = min(*[job.run(t) for job in jobs])
    t += delta
    time.sleep(delta)

    reload(Config)
