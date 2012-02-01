#!/usr/local/bin/python

import Config
import File

IP_MAP = {}

def geocode(ip):
  if ip in IP_MAP:
    return IP_MAP[ip];

  latlong = {}
  for line in File.readUrl(Config.IP_GEOCODE_URL + ip).split('\n'):
    parts = line.split(':')
    if len(parts) is 2:
      name, value = parts
      if name in ['Latitude', 'Longitude']:
        latlong[name.lower()] = value.strip()

  IP_MAP[ip] = latlong
  return latlong
