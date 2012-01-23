#!/usr/bin/python

FILL = ' [...] '

def swapParts(song):
  parts = song.split(' - ')
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

def fixSongTitle(title):
  return fitToSize(swapParts(title.strip()))

