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
