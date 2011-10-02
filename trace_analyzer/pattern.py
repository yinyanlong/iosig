#!/usr/bin/env python

"""
Define all the data structures of patterns and signatures.

This program is part of the "I/O Signature detection software". Visit 
http://www.cs.iit.edu/~scs/iosig for the latest version.
"""

__author__ = "Yanlong Yin (yyin2@iit.edu)"
__version__ = "$Revision: 1.4$"
__date__ = "$Date: 02/11/2010 01:09:23 $"
__copyright__ = "Copyright (c) 2010 SCS IIT"
__license__ = "Python"

import sys

# patt_types =

class ContPattern:
    def __init__(self):
        self.access_size = 0
        self.access_count = 0

    def __repr__(self):
        format = '{%d, %d}'
        value = (self.access_size, self.access_count)
        return format % value

class BasicPattern:
    def __init__(self):
        self.initial_position = 0
        self.dimension = 1
        self.trend = 0
        self.increment = 0
        self.repetition = 1

    def __repr__(self):
        if self.trend != 0:
            format = '{%d, %d, (%d, %d)}'
            value = (self.initial_position, self.dimension,
                     self.increment, self.trend)
        else:
            format = '%d'
            value = (self.initial_position)
        #print '*****', self.trend
        return format % value

class AdvPattern:
    def __init__(self):
        self.offset_pattern = BasicPattern()
        self.size_pattern = BasicPattern()
        # self.temporal_pattern = BasicPattern()
        self.repetition_pattern = BasicPattern()

    def __repr__(self):
        offset_string = '%s' % (self.offset_pattern)
        size_string = '%s' % (self.size_pattern)
        repetition_string = '%s' % (self.repetition_pattern)

        format = '[%s, %s, %s]'
        value = (offset_string, size_string, repetition_string)
        pattern_string = format % value

        return pattern_string

class BasicPattern_2D:
    def __init__(self):
        self.initial_position = 0
        self.dimension = 1
        self.trend = 0
        self.increment = BasicPattern()
        self.repetition = 1

    def __repr__(self):
        if self.trend != 0:
            format = '{%d, %d, (%s, %d)}'
            value = (self.initial_position, self.dimension,
                     self.increment, self.trend)
        else:
            format = '%d'
            value = (self.initial_position)
        #print '*****', self.trend
        return format % value

class AdvPattern_2D:
    def __init__(self):
        self.offset_pattern = BasicPattern_2D()
        self.size_pattern = BasicPattern_2D()
        # self.temporal_pattern = BasicPattern()
        self.repetition_pattern = BasicPattern()

    def __repr__(self):
        offset_string = '%s' % (self.offset_pattern)
        size_string = '%s' % (self.size_pattern)
        repetition_string = '%s' % (self.repetition_pattern)

        #if (self.repetition_pattern.
        format = '[%s, %s, %s]'
        value = (offset_string, size_string, repetition_string)
        pattern_string = format % value

        return pattern_string

class Signature(list):
    def __init__(self):
        self.type = ''
        self.operation = ''
        self.initial_position = 0
        self.dimension = 0
        self.repetition = 1
        self.start = 0
        self.end = 0

    def addOne(self, value):
        """add one pattern to the signature"""
        
        self.append(value)
        self.dimension += 1

    def __repr__(self):  
        index = 0
        patterns_string = ''
        while index < self.dimension:
            if index > 0:
                patterns_string += ', '
            patterns_string += '%s' % (self[index])
            index += 1

        sig_format = '{%s, %d, %d, (%s), %s}'
        sig_value = (self.operation, self.initial_position, 
                     self.dimension, patterns_string, self.repetition)
        sig_string = sig_format % sig_value
        sig_string = 'Type: ' + self.type + '\n' + 'Start(number of accesses): ' + str(self.start) + '        End(# of accesses): ' + str(self.end) + '\n' + 'Trace signatures: ' + str(sig_string)

        return sig_string

