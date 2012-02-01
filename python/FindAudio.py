#!/usr/local/bin/python

import filetree
import os
import sys

"""
Finds all the readable audio files in a given subdirectory and prints them out.
We need this because find doesn't handle UTF-8 song titles correctly.
"""

MIN_SIZE = 16000


def findSortedAudio(root):
  results = []
  def findAudio(node):
    if hasattr(node, 'endswith'):
      if node.endswith('.mp3') and os.stat(node).st_size >= MIN_SIZE:
        results.append(node)
    else:
      for child in node.itervalues():
        findAudio(child)
  findAudio(root)
  return sorted(results)

if __name__ == "__main__":
  for line in findSortedAudio(filetree.filetree(sys.argv[1])):
    print line

