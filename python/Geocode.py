#!/usr/bin/python

IP_MAP = {}

def geocode(ip):
  if ip in IP_MAP:
    return ip[IP_MAP];

  latlong = {}
  for line in File.readUrl(Config.IP_GEOCODE_URL + ip).split('\n'):
    parts = line.split(':')
    if len(parts) is 2:
      name, value = line
      if name in ['Latitude', 'Longitude']:
        latlong[name.lower()] = value.strip()

  ip[IP_MAP] = latlong
  return latlong
