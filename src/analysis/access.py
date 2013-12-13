#!/usr/bin/env python

"""
Define the data structure of access entry .

This program is part of the "I/O Signature detection software". Visit 
http://www.cs.iit.edu/~scs/iosig for the latest version.
"""

__author__ = "Yanlong Yin (yyin2@iit.edu)"
__version__ = "$Revision: 1.4$"
__date__ = "$Date: 09/28/2011 01:09:23 $"
__copyright__ = "Copyright (c) 2010-2011 SCS-Lab, IIT"
__license__ = "Python"

import sys
import sig

class Access:
    def __init__(self, value=[]):
        if sig._format_prop == None:
            #self.mpi_rank = int(value[1])
            self.op = value[1]
            self.file_id = int(value[2])
            self.pos = int(value[3])
            self.size = int(value[4])
            self.startTime = float(value[5])
            self.endTime = float(value[6])
        else:
            #self.mpi_rank = int(value[ int(sig._format_prop['mpi_rank']) ])
            if  int(sig._format_prop['file_id']) >= 0:
                self.file_id = int(value[ int(sig._format_prop['file_id']) ])
            self.size = int(value[ int(sig._format_prop['size']) ])
            self.pos = int(value[ int(sig._format_prop['pos']) ])
            self.startTime = float(value[int(sig._format_prop['start_time'])])
            self.endTime = float(value[int(sig._format_prop['end_time'])])
            #if value[ int(sig._format_prop['op']) ].count('W') or value[ int(sig._format_prop['op']) ].count('w'):
            #    self.op = 'WRITE'
            #elif value[ int(sig._format_prop['op']) ].count('r') or value[ int(sig._format_prop['op']) ].count('R'):
            #    self.op = 'READ'
            self.op = value[ int(sig._format_prop['op']) ]
        if sig._blksz > 0:
            blkId = self.pos/sig._blksz
            filepos = blkId * sig._blksz
            filepos2 = ((filepos + self.size)/sig._blksz)*sig._blksz
            if filepos2 < (filepos+self.size):
                filepos2 += sig._blksz
            self.size = filepos2 - filepos
            self.pos = filepos

    def __repr__(self):
        format = 'Pos: %d, Size: %d, Op: %s.'
        value = (self.pos, self.size, self.op)
        return format % value

        
