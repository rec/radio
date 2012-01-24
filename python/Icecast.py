#!/usr/bin/python

import json
import time

import Job
import Listeners
import Status


Job.runJobs(Job.job(Status.getStatus, callback=Status.newStatus,
                    **Config.STATUS),
            Job.job(Listeners.getListeners, **Config.LISTENER))
