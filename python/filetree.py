#!/usr/local/bin/python

# A tree of files representing a directory structure.
#
# Copyright (c) 2010 Tom Swirly
# This software is licensed under the terms of the artistic license,
# http://www.opensource.org/licenses/artistic-license-2.0.php
#
# Essentially, you can do as you please with it.  Have fun!

import os
import stat

import collections


def filetree(file, accept=lambda x: True):
  """Returns a tree of all files beneath the given file.

  If the file is a directory, this tree is an ordered dictionary mapping the
  relative name of each file in this directory to its filetree.

  If the file is not a directory, the filetree is simply the full path toe the
  tree as a string.

  The parameter accept allows you to selectively accept files at each level.  By
  default, all files are accepted.
  """

  try:
      st_mode = os.lstat(file).st_mode

  except OSError:
      print 'os.error, skipped file', file
      return

  if stat.S_ISDIR(st_mode):
      values = collections.OrderedDict()

      for name in os.listdir(file):
          tree = filetree(os.path.join(file, name), accept)            
          if tree:
              values[name] = tree

      return values

  elif stat.S_ISREG(st_mode) and accept(file):
      return file
