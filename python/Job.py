#!/usr/bin/python

class Job(object):
  def __init__(self, interval, url, filepath):
    self.interval = interval
    self.url = url
    self.filepath = filepath
    self.output = File.readFile(filepath)

  def run(self, time):
    if not (time % self.interval):
      output = self.process(File.readUrl(self.url))
      if output != self.output:
        self.output = output
        File.replaceAtomic(self.filepath, self.output)
    return (-(t + 1) % self.interval) + 1


def runJobs(*jobs):
  t = 0
  while True:
    delta = min(*(job.run(t) for job in jobs))
    t += delta
    time.sleep(delta)
