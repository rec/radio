#!/usr/bin/python

import time

import Config
import File

class Job(object):
  def __init__(self, desc, process):
    self.desc = desc
    self.process = process
    self.output = File.readFile(Config.GENERATED_FILES + desc.filepath)

  def run(self, time):
    if not (time % self.desc.interval):
      output = self.process(File.readUrl(Config.ROOT_URL + self.desc.url))
      if output != self.output:
        self.onOutputChanged(output)
    return (-(time + 1) % self.desc.interval) + 1

  def onOutputChanged(self, output):
    File.replaceAtomic(Config.GENERATED_FILES + self.desc.filepath, output)
    self.output = output


def runJobs(*jobs):
  t = 0
  while True:
    delta = min(*[job.run(t) for job in jobs])
    t += delta
    time.sleep(delta)
