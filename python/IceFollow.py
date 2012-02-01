#!/usr/local/bin/python

import datetime

import Job
import Listeners
import Status

print 'Starting IceFollow at', datetime.datetime.now()


Job.runJobs(Status.StatusJob(), Listeners.ListenerJob())
