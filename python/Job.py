#!/usr/bin/python

import collections

Desc = collections.namedtuple('Desc', 'filepath interval url')

class Job(object):
  ROOT_URL = 'http://ax.to:8000/'
  GENERATED_FILES = '/home/radio/radio/public_html/generated/'

  def __init__(self, desc, process):
    self.desc = desc
    self.process = process
    self.output = File.readFile(Job.GENERATED_FILES + self.filepath)

  def run(self, time):
    if not (time % self.interval):
      output = self.process(File.readUrl(Job.ROOT_URL + self.desc.url))
      if output != self.output:
        self.onOutputChanged(output)
    return (-(t + 1) % self.desc.interval) + 1

  def onOutputChanged(self, output):
    File.replaceAtomic(Job.GENERATED_FILES + self.desc.filepath, output)
    self.output = output


def runJobs(*jobs):
  t = 0
  while True:
    delta = min(*(job.run(t) for job in jobs))
    t += delta
    time.sleep(delta)
