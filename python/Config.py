#!/usr/bin/python

import collections

JobDesc = collections.namedtuple('Desc', 'filepath interval url')

ROOT_URL = 'http://ax.to:8000/'
GENERATED_FILES = '/home/radio/radio/public_html/generated/'

FILL = ' [...] '

LISTENER = JobDesc(url='admin/listclients.xsl?mount=/radio',
                   filepath='listeners.json',
                   interval=15)

STATUS = JobDesc(url='playerStatus.xsl',
                 filepath='status.json',
                 interval=1)

ADMIN = (ROOT_URL + 'admin/', 'admin', 'wombatwombat')

POST_TO_TWITTER = False
LOGGING_ENABLED = True

def log(msg):
  if LOGGING_ENABLED:
    print msg
