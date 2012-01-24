#!/usr/bin/python

import Job

FILL = ' [...] '

LISTENER = Job.Desc(url='admin/listclients.xsl?mount=/radio',
                    filepath='listeners.html',
                    interval=15)

STATUS = Job.Desc(url='playerStatus.xsl',
                  filepath='listeners.html',
                  interval=1)

ADMIN = (Job.Job.ROOT_URL + 'admin/', 'admin', 'wombatwombat')

POST_TO_TWITTER = False
LOGGING_ENABLED = True

def log(msg):
  if LOGGING_ENABLED:
    print msg
