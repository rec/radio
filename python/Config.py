#!/usr/bin/python

FILL = ' [...] '
ROOT_URL = 'http://ax.to:8000/'

STATUS_URL = ROOT_URL + 'data.xsl'
LISTENER_URL = ROOT_URL + 'admin/listclients.xsl?mount=/radio'

ADMIN = ROOT_URL + 'admin/', 'admin', 'wombatwombat'
GENERATED_FILES = '/home/radio/radio/public_html/generated/'

STATUS_FILE = GENERATED_FILES + 'status.html'
LISTENER_FILE = GENERATED_FILES + 'listeners.html'

STATUS_INTERVAL = 1
LISTENER_INTERVAL = 15
POST_TO_TWITTER = False
LOGGING_ENABLED = True

def log(msg):
  if LOGGING_ENABLED:
    print msg
