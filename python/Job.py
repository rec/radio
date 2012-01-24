#!/usr/bin/python

class Job(object):
  ROOT_URL = 'http://ax.to:8000/'
  GENERATED_FILES = '/home/radio/radio/public_html/generated/'

  def __init__(self, process, interval, url, filepath):
    self.process = process
    self.interval = interval
    self.url = url
    self.filepath = filepath
    self.output = File.readFile(Job.GENERATED_FILES + self.filepath)

  def run(self, time):
    if not (time % self.interval):
      output = self.process(File.readUrl(Job.ROOT_URL + self.url))
      if output != self.output:
        self.onOutputChanged(output)
    return (-(t + 1) % self.interval) + 1

  def onOutputChanged(self, output):
    File.replaceAtomic(Job.GENERATED_FILES + self.filepath, output)
    self.output = output


def runJobs(*jobs):
  t = 0
  while True:
    delta = min(*(job.run(t) for job in jobs))
    t += delta
    time.sleep(delta)
