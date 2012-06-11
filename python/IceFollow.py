#!/usr/local/bin/python

import datetime
import sys

import Job
import Listeners
import Status

print 'Starting IceFollow at', datetime.datetime.now()
sys.stdout.flush()

Job.runJobs(Status.StatusJob(), Listeners.ListenerJob())
