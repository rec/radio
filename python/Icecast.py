#!/usr/bin/python

import time

import Clients
import Status

def runJobs(*jobs):
  t = 0
  while True:
    for c in jobs:
      if not (t % c.sleep):
        c.callback()
    delta = min(*(1 + ((-1 - t) % c.sleep) for c in jobs))
    t += delta
    time.sleep(delta)

runJobs(Status.StatusJob(), Clients.ClientJob())
