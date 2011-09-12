#!/usr/bin/env python

"""
Define the data structure of access entry .

This program is part of the "I/O Signature detection software". Visit 
http://www.cs.iit.edu/~scs/SIG/sig.html for the latest version.
"""

__author__ = "Yanlong Yin (yyin2@iit.edu)"
__version__ = "$Revision: 1.0$"
__date__ = "$Date: 02/11/2010 01:09:23 $"
__copyright__ = "Copyright (c) 2010 SCS IIT"
__license__ = "Python"

import sys
import sig

class Access:
    def __init__(self, value=[]):
        if sig._format_prop == None:
            self.mpi_rank = int(value[-6])
            self.file_id = int(value[-5])
            if sig._blksz > 0:
                self.size = int(value[-3])
                self.pos = int(value[-4])
                blkId = self.pos/sig._blksz
                filepos = blkId * sig._blksz
                filepos2 = ((filepos + self.size)/sig._blksz)*sig._blksz
                if filepos2 < (filepos+self.size):
                    filepos2 += sig._blksz
                self.size = filepos2 - filepos
                self.pos = filepos
            elif sig._blksz == 0:
                self.pos = int(value[-4])
                self.size = int(value[-3])
#            if value[-1].count('W') or value[-1].count('w'):
#                self.op = 'WRITE'
#            elif value[-1].count('R') or value[-1].count('r'):
#                self.op = 'READ'
            self.op = value[ int(sig._format_prop['op']) ]
        else:
            self.mpi_rank = int(value[ int(sig._format_prop['mpi_rank']) ])
            if  int(sig._format_prop['file_id']) >= 0:
                self.file_id = int(value[ int(sig._format_prop['file_id']) ])
            if sig._blksz > 0:
                self.size = int(value[ int(sig._format_prop['size']) ])
                self.pos = int(value[ int(sig._format_prop['pos']) ])
                blkId = self.pos/sig._blksz
                filepos = blkId * sig._blksz
                filepos2 = ((filepos + self.size)/sig._blksz)*sig._blksz
                if filepos2 < (filepos+self.size):
                    filepos2 += sig._blksz
                self.size = filepos2 - filepos
                self.pos = filepos
            elif sig._blksz == 0:
                self.size = int(value[ int(sig._format_prop['size']) ])
                self.pos = int(value[ int(sig._format_prop['pos']) ])
            if value[ int(sig._format_prop['op']) ].count('W') or value[ int(sig._format_prop['op']) ].count('w'):
                self.op = 'WRITE'
            elif value[ int(sig._format_prop['op']) ].count('r') or value[ int(sig._format_prop['op']) ].count('R'):
                self.op = 'READ'
            self.op = value[ int(sig._format_prop['op']) ]

    def __repr__(self):
        format = 'Pos: %d, Size: %d, Op: %s.'
        value = (self.pos, self.size, self.op)
        return format % value

        
