#!/usr/local/bin/python

import json
import os
import sys
import traceback
import urllib2

import Config

def makeOpener():
  manager = urllib2.HTTPPasswordMgrWithDefaultRealm()
  manager.add_password(None, *Config.ADMIN)
  handler = urllib2.HTTPBasicAuthHandler(manager)
  return urllib2.build_opener(handler)

OPENER = makeOpener()

_index =  0

def readUrl(url):
  try:
    global _index
    _index += 1
    sep = '&' if '?' in url else '?'
    url = '%s%s%d' % (url, sep, _index)
    return OPENER.open(url).read()
  except:
    print("Couldn't open URL" + url)
    return None

def replaceAtomic(filename, value):
  tmpname = filename + '.tmp'

  try:
    f = open(tmpname, 'w')
    f.write(value)
    f.close()

  except:
    print "Couldn't write to", tmpname, value
    return False

  try:
    os.rename(tmpname, filename)
    Config.log('Wrote ' + filename)
    return True

  except:
    print "Couldn't rename", tmpname, "to", filename
    return False

def replaceJson(filename, **args):
  return replaceAtomic(filename, json.dumps(args))


def readFile(f):
  try:
    return open(f).read()

  except:
    print "Couldn't open file", f
    return None

