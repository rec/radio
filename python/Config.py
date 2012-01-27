#!/usr/bin/python

import collections

JobDesc = collections.namedtuple('Desc', 'filepath interval url')

ROOT_URL = 'http://radio.swirly.com:8000/'
GENERATED_FILES = '/home/radio/radio/public_html/generated/'

FILL = ' [...] '

LISTENER = JobDesc(url='admin/listclients.xsl?mount=/radio',
                   filepath='listeners.json',
                   interval=3)

STATUS = JobDesc(url='playerStatus.xsl',
                 filepath='status.json',
                 interval=2)

BROADCASTER = {'latitude': 40.719569,
               'longitude': -73.961105,
               'name': 'swirly radio international'};

IP_GEOCODE_URL = 'http://api.hostip.info/get_html.php?position=true&id='

ADMIN = (ROOT_URL + 'admin/', 'admin', 'wombatwombat')
MAX_TITLES = 200


POST_TO_TWITTER = not True
LOGGING_ENABLED = not True

def log(msg):
  if LOGGING_ENABLED:
    print msg
