#!/usr/bin/python

FILL = ' [...] '

LISTENER = {'url': 'admin/listclients.xsl?mount=/radio',
            'filepath': 'listeners.html',
            'interval': 15}

STATUS = {'url': 'playerStatus.xsl',
          'filepath': 'listeners.html',
          'interval': 1}

ADMIN = (ROOT_URL + 'admin/', 'admin', 'wombatwombat')


STATUS_FILE = GENERATED_FILES + 'status.html'
LISTENER_FILE =

STATUS_INTERVAL = 1
LISTENER_INTERVAL = 15
POST_TO_TWITTER = False
LOGGING_ENABLED = True

def log(msg):
  if LOGGING_ENABLED:
    print msg
