#!/usr/bin/python

import urllib2

import Config

def makeOpener():
  manager = urllib2.HTTPPasswordMgrWithDefaultRealm()
  manager.add_password(None, *Config.ADMIN)
  handler = urllib2.HTTPBasicAuthHandler(manager)
  return urllib2.build_opener(handler)

OPENER = makeOpener()

def readUrl(url):
  try:
    return OPENER.open(url).read()
  except:
    print "Couldn't open", url
    return None

def replaceAtomic(filename, value):
  tmpname = filename + '.tmp'

  try:
    f = open(tmpname, 'w')
    f.write(value)
    f.close()

  except:
    print "Couldn't write to", tmpname
    return False

  try:
    os.rename(tmpname, filename)
    return True

  except:
    print "Couldn't rename", tmpname, "to", filename
    return False

def readFile(f):
  try:
    return open(f).read()

  except:
    print "Couldn't open", f
    return None

