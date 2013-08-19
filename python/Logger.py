import logging

LOGGER = logging.getLogger('RadioDaemon')
LOGGER.setLelevel(logging.INFO)
handler = logging.FileHandler('/home/radio/logs/IceFollow.log')
LOGGER.addHandler(handler)
