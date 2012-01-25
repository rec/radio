#!/usr/bin/python

FILL = ' [...] '


def splitSongParts(song):
  parts = song.strip().split(' - ')
  if len(parts) > 3:
    parts[2:] = [' - '.join(parts[2:])]

  return parts

def swapParts(song):
  parts = splitSongParts(song)
  if len(parts) is 1:
    return '"%s"' % parts[0]

  elif len(parts) is 2:
    return '"%s" - %s' % (parts[1], parts[0])

  else:
    return '"%s" - %s (%s)' % (parts[2], parts[1], parts[0])

def fitToSize(line, length = 140):
  n = len(line)
  if n <= length:
    return line

  before = (n - len(FILL)) / 2
  after = length - (before + len(FILL))
  return line[0 : before] + FILL + line[after : ]


def fixHTML(s):
  return (s.replace('&', '&amp;').
          replace('<', '&lt;').
          replace('>', '&gt;').
          replace('"', '&quot;').
          replace("'", '&#39;'))

def fixTitle(title):
  return fitToSize(swapParts(title))

