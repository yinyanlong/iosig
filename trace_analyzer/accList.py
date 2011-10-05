#!/usr/bin/env python

"""
Define the class of AccList, which contains all the access entries and detect 
signatures among them.

This program is part of the "I/O Signature detection software". Visit 
http://www.cs.iit.edu/~scs/iosig for the latest version.
"""

__author__ = "Yanlong Yin (yyin2@iit.edu)"
__version__ = "$Revision: 1.4$"
__date__ = "$Date: 10/02/2011 18:09:23 $"
__copyright__ = "Copyright (c) 2010-2011 SCS-Lab, IIT"
__license__ = "Python"

import sys
import access

import numpy as np
import matplotlib.pyplot as plt
import matplotlib.mlab as mlab

import makeHTML
#import signatureList_pb2

from pattern import *
from util import *

class AccList(list):
    """
    AccList is the list of accesses in the trace file.
    """

    def __init__(self, *args):
        debugPrint( 'Initializing AccList...')
        self.name = ''
        self.signatures = [] # list of signatures
        self.max_pos = 0
        self.min_pos = 0
        self.max_size = 0
        self.min_size = 0
        self.trace = ''

        
    def print_signature(self):
        debugPrint ('The number of signatures: ', len(self.signatures))
        n = 1
        for i in self.signatures:
            print '- ' + str(n) + ' -'
            print i
            n += 1

    def detect_signature(self, start, end):
        debugPrint( 'Detecting signatures ...')
        debugPrint( 'Start:', start)
        debugPrint( 'End:', end)
        index = start ## The cursor of processed accesses
        
        K = 2
        MMAXSIZE = 5 # Markov Max size
        msize = 2

        while start < end-3:
            index = self.detect_signature_cont(start, end, 0)

            if index > start:
                start = index
                continue

            index = self.detect_signature_2d(start, end, 2)

            if index > start:
                start = index
                continue

            msize = 2
            while msize < MMAXSIZE:
                index = self.detect_signature_markov(start, end, msize)

                if index > start:
                    start = index
                    break
                else:
                    continue

            if index > start:
                start = index
                continue
            else:
                index+= 1
                start = index
                continue

            msize = 2 # markov size
            while msize < MMAXSIZE:
                if index > start+msize:
                    print 'hhh'
                    #index += 1
                    #start = index
                    #msize = 2
                    #continue
                    break
                msize += 1

            if msize == MMAXSIZE:
                index += 1

            if start < end-2:
                #index += 1
                start = index
                continue
            else:
                break

        if start > end-3:
            return

    def detect_signature_cont(self, start, end, k):
        """Detect the contiguous trace pattern"""
        
        debugPrint('Detecting contiguous pattern ...\n', 'start: ', start, ', end: ', end)

        ## if the index is illegal then exit
        if start >= len(self):
            print 'Illegal parameters: Start index is too large, return from detect_signature_cont'
            return 0

        r_index = start + 1

        while r_index <= min( end-k, (len(self)-1)-k ):
            if self[r_index].pos == self[r_index-1].pos + self[r_index-1].size:
                r_index = r_index + 1
            else:
                break

        if r_index > end:
            r_index = end

        if (r_index - start) > 1:
            sign = Signature()
            sign.start = start
            sign.end = r_index
            sign.repetition = 1
            sign.type = "CONTIGUOUS"
            sign.operation = self[r_index].op

            patt = ContPattern()
            patt.access_size = self[r_index-1].size;
            if r_index == end:
                patt.access_count = r_index - start + 1
            else:
                patt.access_count = r_index - start
            # to be continue
            sign.addOne(patt)
            self.signatures.append(sign)
            
            start = r_index
        
        return start

    def detect_signature_1d(self, start, end, k):
        """Detect the 1D fixed strided trace pattern"""

        debugPrint( 'Detecting 1D Fixed Strided Pattern ...\n','start: ', start, ',end: ', end)

        ## if the index is illegal then exit
        if start >= len(self):
            print 'Start index is too large, return from detect_signature_1d'
            return 0

        k = 1
        index = start
        r_index = index
        index1 = 0
        pro2end = 1
        pos_diff = []
        size_diff = []

        access_repetion = 1
        #pattern_repetion = 0
        sign = Signature()
        sign.repetition = -1

        # pos_diff[i] means the difference of position of access[i] and access[i+1]
        if index <= min( end-2, (len(self)-1)-2 ):
            pos_diff.append(self[index+1].pos - self[index].pos)
            size_diff.append(self[index+1].size - self[index].size)

        index1 = 1
        r_index = index+index1
        
        while r_index <= min( end-k, (len(self)-1)-k ):
            pos_diff.append(self[r_index+1].pos - self[r_index].pos)
            size_diff.append(self[r_index+1].size - self[r_index].size)

            debugPrint ("HHH", len(pos_diff), "HHH", index1)
            if pos_diff[index1] == pos_diff[index1-1] and size_diff[index1] == size_diff[index1-1]:
                index1 = index1+1
                r_index = index+index1
            else:
                pro2end = 0
                break

        if sign.repetition == -1 and (r_index-index)>1:
            sign.repetition = (r_index+1 - index) #+pro2end

        ## check whether one pattern is found
        if index1 > 1:
            debugPrint ('[SIG_1D]: one pattern found.')
            debugPrint ('[SIG_1D]: r_index=', r_index)
            
            access_repetion = index1+1

            sign.operation = self[start].op
            sign.initial_position = self[start].pos

            patt = 0
            patt = AdvPattern()

            # offset pattern
            patt.offset_pattern.initial_position = self[index].pos
            patt.offset_pattern.dimension = 1
            if (pos_diff[0] > 0):
                patt.offset_pattern.trend = 1
                patt.offset_pattern.increment = pos_diff[0]
            if (pos_diff[0] == 0):
                patt.offset_pattern.trend = 0
                patt.offset_pattern.increment = pos_diff[0]
            if (pos_diff[0] < 0):
                patt.offset_pattern.trend = -1
                patt.offset_pattern.increment = pos_diff[0]*-1
            patt.offset_pattern.repetition = 0

            # size pattern
            patt.size_pattern.initial_position = self[index].size
            patt.size_pattern.dimension = 1
            if (size_diff[0] > 0):
                patt.size_pattern.trend = 1
                patt.size_pattern.increment = size_diff[0]
            if (size_diff[0] == 0):
                patt.size_pattern.trend = 0
                patt.size_pattern.increment = size_diff[0]
            if (size_diff[0] < 0):
                patt.size_pattern.trend = -1
                patt.size_pattern.increment = size_diff[0] * -1
            patt.size_pattern.repetition = 0

            # repetition pattern
            patt.repetition_pattern.initial_position = 1 # TODO: repetition could vary
            patt.repetition_pattern.dimension = 1
            patt.repetition_pattern.trend = 0
            patt.repetition_pattern.repetition = 0
                    
            sign.addOne(patt) ## Pattern saved.
            debugPrint ('[SIG_1D]: pattern saved.')
            pro2end = 1 ## Reset this value before start to detect the second pattern
        
        if (len(sign) > 0 and sign.repetition > 1):
            debugPrint ('[SIG_1D]: One signature found ...', 'start=', start)
            self.signatures.append(sign)

        # r_index = start + sig.repetition * 1
        debugPrint ('[SIG_1D]: index=', r_index)
        return r_index

    #######################################################################
    # This method detect the real kd strided method
    # currently it only detect 2d and 1d fixed strided pattern
    # 
    def detect_signature_2d(self, start, end, k):
        """Detect the 2D fixed strided trace pattern"""

        debugPrint ('Detecting 2D\/Fixed Strided Pattern ...','start:',start,',end:',end)

        ## if the index is illegal then exit
        if start >= len(self)-3:
            debugPrint ('Start index is too large, return from detect_signature_2d')
            return 0

        k = 2
        r_index = start
        index1 = 0
        index2 = 0
        pro2end = 1
        pos_diff1 = []
        size_diff1 = []
        pos_diff2 = []
        size_diff2 = []

        access_repetion = 1
        #pattern_repetion = 0
        sign = Signature()
        sign.repetition = -1

        # pos_diff[i] means the difference of position of access[i] and access[i+1]
        if start <= min( end-2, (len(self)-1)-2 ):
            pos_diff1.append(self[r_index+1].pos - self[r_index].pos)
            size_diff1.append(self[r_index+1].size - self[r_index].size)
            r_index = r_index+1
            pos_diff1.append(self[r_index+1].pos - self[r_index].pos)
            size_diff1.append(self[r_index+1].size - self[r_index].size)
            r_index = r_index+1

            pos_diff2.append(pos_diff1[index1+1] - pos_diff1[index1])
            size_diff2.append(size_diff1[index1+1] - size_diff1[index1])
            index1 = index1+1
            debugPrint ("GGG", pos_diff2[index1-1])

        index2 = 1
        while r_index <= min( end-k, (len(self)-1)-k ):
            pos_diff1.append(self[r_index+1].pos - self[r_index].pos)
            size_diff1.append(self[r_index+1].size - self[r_index].size)
            r_index = r_index+1

            pos_diff2.append(pos_diff1[index1+1] - pos_diff1[index1])
            size_diff2.append(size_diff1[index1+1] - size_diff1[index1])
            index1 = index1+1
            debugPrint ("GGG", pos_diff2[index1-1])

            debugPrint ("HHH", len(pos_diff2), "HHH", index2)
            if pos_diff2[index2] == pos_diff2[index2-1] and size_diff2[index2] == size_diff2[index2-1]:
                index2 = index2+1
            else:
                pro2end = 0
                break

        if sign.repetition == -1 and (r_index - start) > k+1:
            sign.repetition = (r_index - start) +pro2end
            # sig.repetition = (r_index+1 - index) +pro2end

        ## check whether one pattern is found
        if index2 > 1:
            debugPrint ('[SIG_2D]: one pattern found.')
            debugPrint ('[SIG_2D]: r_index=', r_index)
            
            access_repetion = index2+2

            sign.operation = self[start].op
            sign.initial_position = self[start].pos

            patt = 0
            patt = AdvPattern_2D()

            # offset pattern
            patt.offset_pattern.initial_position = self[start].pos
            patt.offset_pattern.dimension = 1
            if (pos_diff1[0] != 0):
                patt.offset_pattern.trend = 1
            patt.offset_pattern.increment.initial_position = pos_diff1[0]
            patt.offset_pattern.increment.dimension = 1
            if (pos_diff2[0] != 0):
                patt.offset_pattern.increment.trend = 1
            patt.offset_pattern.increment.increment = pos_diff2[0]
            patt.offset_pattern.increment.repetition = 0
            patt.offset_pattern.repetition = 0

            # size pattern
            patt.size_pattern.initial_position = self[start].size
            patt.size_pattern.dimension = 1
            if (size_diff1[0] != 0):
                patt.size_pattern.trend = 1
            patt.size_pattern.increment.initial_position = size_diff1[0]
            patt.size_pattern.increment.dimension = 1
            if (size_diff2[0] != 0):
                patt.size_pattern.increment.trend = 1
            patt.size_pattern.increment.increment = size_diff2[0]
            patt.size_pattern.increment.repetition = 0
            patt.size_pattern.repetition = 0

            #repetition pattern
            patt.repetition_pattern.initial_position = 1 # TODO: repetition could vary
            patt.repetition_pattern.dimension = 1
            patt.repetition_pattern.trend = 0
            patt.repetition_pattern.repetition = 0
                    
            sign.addOne(patt) ## Pattern saved.
            debugPrint ('[SIG_2D]: pattern saved.')
            pro2end = 1 ## Reset this value before start to detect the second pattern
        
        if (len(sign) > 0 and sign.repetition > 2):
            debugPrint ('[SIG_2D]: One signature found ...', 'start=', start)
            sign.start = start
            sign.end = r_index
            if sign[0].offset_pattern.trend == 0 and sign[0].size_pattern.trend == 0:
                sign.type = 'FIXED_STRIDED'
            else:
                sign.type = 'TWOD_STRIDED'
                
            self.signatures.append(sign)
            r_index = start + sign.repetition * 1
        else:
            r_index = start
            del sign[:]
            sign.repetition = 0

        # r_index = start + sig.repetition * 1
        debugPrint ('[SIG_2D]: index=', r_index)
        return r_index

    def detect_signature_markov(self, start, end, k):
        """Detect the Markov trace pattern"""

        debugPrint ('Detecting ', k,'-size Markov Pattern ...','start:',start,',end:',end)
        
        ## if the index is illegal then exit
        if start >= len(self)-2*(k+1):
            debugPrint ('Start index is too large, return from detect_signature_markov')
            return start

        r_index = start
        size = k
        bingo = 0
        pro2end = 1
        pos_diff1 = []
        size_diff1 = []
        pos_diff2 = []
        size_diff2 = []

        for i in range(size):
            pos_diff1.append(0)
            size_diff1.append(0)
            pos_diff2.append(0)
            size_diff2.append(0)
            
        access_repetion = 1
        #pattern_repetion = 0
        sign = Signature()

        ## here is be a loop of loop, while index stay unchanged,
        ## and index2 should go further to find pattern
        index2 = start + size        
        while index2 <= min( end-size, (len(self)-1)-size ):
            debugPrint ('>>>>>>>>>>>>>>>>>>>', index2)
            for i in range(size):                    
                pos_diff2[i] = self[index2+i].pos - self[index2+i-size].pos
                size_diff2[i] = self[index2+i].size - self[index2+i-size].size

#            while index2 <= min ( end-size, (len(self)-1)-size ):
            
               # for loop, iteration time: size
            for i in range(size):                    
                pos_diff1[i] = pos_diff2[i]
                size_diff1[i] = size_diff2[i]
                pos_diff2[i] = self[index2+size+i].pos - self[index2+i].pos
                size_diff2[i] = self[index2+size+i].size - self[index2+i].size
                    
            for i in range(size):
                if pos_diff2[i] == pos_diff1[i] and size_diff2[i] == size_diff1[i]:
                    bingo += 1

            if bingo == size:
                access_repetion +=  1
                index2 += size
                r_index = index2
                bingo = 0
                continue
            else:
                #pro2end = 0
                break                
                # end of if else
            # end of if ... (One pattern is found)

            # check whether one pattern is found    
        if index2 - start > size:
                # create a sig object here
            debugPrint ('[SIG_kd]: one pattern found.')
                # save the pattern into signature object
            sign.operation = self[start].op
            sign.initial_position = self[start].pos
            sign.repetition = (index2-start)/k#+1#+pro2end
            sign.type = 'MARKOV'
            sign.start = start
            sign.end = index2

            # save all th k=size patterns
            for i in range(size):
                patt = 0
                patt = AdvPattern()

                patt.offset_pattern.initial_position = self[start+i].pos
                patt.offset_pattern.dimension = 1
                if (pos_diff1[i] > 0):
                    patt.offset_pattern.trend = 1
                    patt.offset_pattern.increment = pos_diff1[i]
                if (pos_diff1[i] == 0):
                    patt.offset_pattern.trend = 0
                    patt.offset_pattern.increment = pos_diff1[i]
                if (pos_diff1[i] < 0):
                    patt.offset_pattern.trend = -1
                    patt.offset_pattern.increment = pos_diff1[i] * -1
                patt.offset_pattern.repetition = 0

                patt.size_pattern.initial_position = self[start+i].size
                patt.size_pattern.dimension = 1
                if (size_diff1[i] > 0):
                    patt.size_pattern.trend = 1
                    patt.size_pattern.increment = size_diff1[i]
                if (size_diff1[i] == 0):
                    patt.size_pattern.trend = 0
                    patt.size_pattern.increment = size_diff1[i]
                if (size_diff1[i] < 0):
                    patt.size_pattern.trend = -1
                    patt.size_pattern.increment = size_diff1[i] * -1
                patt.size_pattern.repetition = 0

                patt.repetition_pattern.initial_position = 1 # TODO: repetition could vary
                patt.repetition_pattern.dimension = 1
                patt.repetition_pattern.trend = 0
                patt.repetition_pattern.repetition = 0
                  
                sign.addOne(patt) ## Pattern saved.
                debugPrint ('[SIG_kd]: pattern saved.')
                #pro2end = 1 ## Reset this value before start to detect the second pattern
            # end of inner loop, 'while index2'

#            index += 1
#            if (index - start == size):
#                break

        if (len(sign) > 0):
            debugPrint ('[SIG_kd]: One signature found ...')
            self.signatures.append(sign)

        # r_index = start + len(sig) * 1
        debugPrint ('[SIG_kd]: index=', r_index)
        return r_index

    def gen_protobuf(self, path):
        """Generate the protobuf file"""

        debugPrint("Generating the protobuf output file")
        #sList = signatureList_pb2.SignatureList()

        for s in self.signatures:
            #sig = signatureList_pb2.Signature()
            
            if s.type == 'CONTIGUOUS':
                debugPrint("TYPE: CONTIGUOUS")
                #sig.SigType = signatureList.Signature.CONTIGUOUS
                
            elif s.type == 'FIXED_STRIDED':
                debugPrint("TYPE: FIXED_STRIDED")

                
            elif s.type == 'TWOD_STRIDED':
                debugPrint("TYPE: TWOD_STRIDED")

                
            elif s.type == 'THREED_STRIDED':
                debugPrint("TYPE: THREED_STRIDED")
                #current not supported
                
            elif s.type == 'MARKOV':
                debugPrint("TYPE: MARKOV")
                
                
    def makeup_output(self, path):
        """Generate the fancy output in HTML page"""

        debugPrint("Generating webpage output")

        self.max_pos = self[0].pos
        self.min_pos = self.max_pos
        self.max_size = self[0].size
        self.min_size = self.max_size

        # draw the figure of position
        x = range(len(self))
        y = []
        for i in self:
            y.append(i.pos)
            if self.min_pos > i.pos:
                self.min_pos = i.pos
            if self.max_pos < i.pos:
                self.max_pos = i.pos
            if self.min_size > i.size:
                self.min_size = i.size
            if self.max_size < i.size:
                self.max_size = i.size
        
        plt.xlabel('access')
        plt.ylabel('access offset')
        plt.title('pattern of access offset')

        #plt.axis(1, len(x), self.min_pos, self.max_pos)
        plt.plot(x, y, 'r.')
        #plt.grid(True)

        plt.savefig(path+"/pos.png")
        plt.cla()

        # draw the figure of size
        x = range(len(self))
        y = []
        for i in self:
            y.append(i.size)
        
        plt.xlabel('access')
        plt.ylabel('size of each access (byte)')
        plt.title('pattern of access offset')

        #plt.axis(1, len(x), self.min_pos, self.max_pos)
        plt.plot(x, y, 'r.')
        #plt.grid(True)

        plt.savefig(path+"/size.png")

        # generate the html output file
        pageTitle = 'I/O Signature Information'
        pageHead = makeHTML.part('head')
        pageHead.addPart('title', content=pageTitle)
        
        pageBody = makeHTML.part('body')
        
        pageBody.addPart('h1', content="Configurations")
        trace_file = 'Trace file: ' + self.trace + '.'
        pageBody.addPart('p', content=trace_file)

        pageBody.addPart('h1', content="I/O Signatures")

        sig_string = 'Trace Signature:'
        for i in self.signatures:
            sig_string = sig_string + i.__repr__()
        sig_string = sig_string +'.'
        pageBody.addPart('p', content=sig_string)

        pageBody.addPart('h1', content="Pattern Figures")
        fig_string = "<img src=\"./pos.png\"/> <img src=\"./size.png\"/>"
        pageBody.addPart('p', content=fig_string)

        pageBody.addPart('hr')
        fullPage = makeHTML.part('html')
        fullPage.addPiece(pageHead)
        fullPage.addPiece(pageBody)

        f = open(path+'/sig.html', 'w')
        f.write(fullPage.make())

    def gen_iorates(self, path):
        """Generate the iorates figure"""

        debugPrint("Generating the time figure")

        plt.title('IO rates on time')
         


