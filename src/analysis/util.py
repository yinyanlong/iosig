#!/usr/bin/env python

"""
Miscellaneous utility functions.

This program is part of the "I/O Signature detection software". Visit 
http://www.cs.iit.edu/~scs/iosig for the latest version.
"""

__author__ = "Yanlong Yin (yyin2@iit.edu)"
__version__ = "$Revision: 1.4$"
__date__ = "$Date: 02/11/2010 01:09:23 $"
__copyright__ = "Copyright (c) 2010 SCS IIT"
__license__ = "Python"

import sys, os, string
import sig

def debugPrint(*args):
    if sig._debug == 0:
        return

    info = ''
    for i in args:
        info += str(i)

    print info

def tail(f, window=1):
    """Returns the last `window` lines of file `f` as a list. """
    if window == 0:
        return []
    BUFSIZ = 1024
    f.seek(0, 2)
    bytes = f.tell()
    size = window + 1
    block = -1
    data = []
    while size > 0 and bytes > 0:
        if bytes - BUFSIZ > 0:
            # Seek back one whole BUFSIZ
            f.seek(block * BUFSIZ, 2)
            # read BUFFER
            data.insert(0, f.read(BUFSIZ))
        else:
            # file too small, start from begining
            f.seek(0,0)
            # only read what was not read
            data.insert(0, f.read(bytes))
        linesFound = data[0].count('\n')
        size -= linesFound
        bytes -= BUFSIZ
        block -= 1
    return ''.join(data).splitlines()[-window:]
