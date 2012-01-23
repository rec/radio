#!/usr/bin/python

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
    return None

