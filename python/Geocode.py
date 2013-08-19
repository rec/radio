#!/usr/local/bin/python

import Config
import File
import Logger

IP_MAP = {}

def geocode(ip):
  if ip in IP_MAP:
    return IP_MAP[ip];

  latlong = {}
  url = Config.IP_GEOCODE_URL + ip
  try:
    lines = File.readUrl(url).split('\n')
  except:
    Logger.LOGGER.error("Couldn't read URL %s", url)
    return
    
  for line in lines:
    parts = line.split(':')
    if len(parts) is 2:
      name, value = parts
      if name in ['Latitude', 'Longitude']:
        latlong[name.lower()] = value.strip()

  IP_MAP[ip] = latlong
  return latlong
