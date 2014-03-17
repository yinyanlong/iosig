#!/usr/bin/env python
"""
Single trace Analysis


"""

__author__ = "Yanlong Yin (yyin2@iit.edu)"
__version__ = "$Revision: 1.4$"
__date__ = "$Date: 02/08/2014 $"
__copyright__ = "Copyright (c) 2010-2014 SCS Lab, IIT"
__license__ = "Python"

import sys, os, string, getopt, gc, multiprocessing
from sig import *
from access import *
from accList import *
from prop import *
from util import *

def detectSignature(filename):
    # the list contains all the accesses
    rlist = AccList()
    wlist = AccList()
    accList = AccList()  # all lines with "accList" are commentted out
                          # because the figure drawing using accList
                          # is replaced with rlist and wlist

    # open the trace file
    f = open(filename, 'r')

    # skip the first several lines
    # Maybe the skipped lines are table heads
    for i in range(int(sig._format_prop['skip_lines'])):
        line = f.readline()
             
    # scan the file and put the access item into list
    i = 0
    j = 0
    op_index = int(sig._format_prop['op'])
    debugPrint ('op_index: ', op_index)
    op = ''

    # TODO: add while 1 loop here
    for i in range(sig._range):
        line = f.readline()
        if not line:
            break
        words = string.split(line)
        # there might be some blank lines
        if len(words) < 6:
            j+=1
            continue

        ## only "READ" and "WRITE" will be saved
        #if words[-1].count('READ') == 0 and words[-1].count('WRITE') == 0:
        # to test chomob, only use write
        # if words[-1].count('WRITE') == 0:
        #    j+=1
        #    continue

        ## save to list
        op = words[op_index].upper();
        acc = Access(words)
        if acc.size >= 1:
            accList.append(acc)

        if op.count('READ')>0 or op == 'R':
            debugPrint("one READ")
            rlist.append(acc)

        if op.count('WRITE')>0 or op == 'W':
            debugPrint("one WRITE")
            wlist.append(acc)

    ## close the opened file
    f.close()
    rlist.trace = filename
    wlist.trace = filename
    accList.trace = filename

    # print the time summary
    print 'Total read time: ', sig._total_read_time
    print 'Total write time: ', sig._total_write_time
    print 'Numbers of operations - ', 'Read: ', len(rlist), ' write: ', len(wlist)

    ## deal with the list
    rlist.detect_signature(0, min(sig._range-j-1, len(rlist)-1) )
    wlist.detect_signature(0, min(sig._range-j-1, len(wlist)-1) )

    ## Done with the whole process of detecting
    ## Print the whole signature
    if len(rlist.signatures) > 0 or len(wlist.signatures) > 0:
        print '----------------------------------------'
        print 'The following signatures are detected:'

    if len(rlist.signatures) > 0:
        rlist.print_signature()
        rlist.gen_protobuf(sig._out_path)
        rlist.makeup_output(sig._out_path)

    if len(wlist.signatures) > 0:
        wlist.print_signature()
        wlist.gen_protobuf(sig._out_path)
        wlist.makeup_output(sig._out_path)

    #if len(accList) > 0:
        accList.gen_iorates(sig._out_path)

def generateCSVs(single_trace_filename):
    """Generate the Read/Write Bandwidth figures"""
    trace_path, trace_filename = os.path.split(single_trace_filename)

    # the list contains all the accesses
    rlist = AccList()
    wlist = AccList()
    rlistEmpty = 1
    wlistEmpty = 1

    total_read_count = 0
    total_write_count = 0
    total_read_time = 0.0
    total_write_time = 0.0

    # Create and empty each CSV files, write the CSV title line
    output = os.path.join(sig._out_path, trace_filename + ".read.rate.csv")
    f = open(output, 'w')
    f.write("Time,Rate\n")
    f.close()
    output = os.path.join(sig._out_path, trace_filename + ".write.rate.csv")
    f = open(output, 'w')
    f.write("Time,Rate\n")
    f.close()

    output = os.path.join(sig._out_path, trace_filename + ".read.interval.csv")
    f = open(output, 'w')
    f.write("Begin,End\n")
    f.close()
    output = os.path.join(sig._out_path, trace_filename + ".write.interval.csv")
    f = open(output, 'w')
    f.write("Begin,End\n")
    f.close()

    output = os.path.join(sig._out_path, trace_filename + ".read.hole.sizes.csv")
    f = open(output, 'w')
    f.write("Time,Size\n")
    f.close()

    # open the trace file
    f = open(single_trace_filename, 'r')
    # skip the first several lines
    # Maybe the skipped lines are table heads
    for i in range(int(sig._format_prop['skip_lines'])):
        line = f.readline()
             
    # scan the file and put the access item into list
    i = 0
    j = 0
    eof = 0 # reaching the EOF?
    op_index = int(sig._format_prop['op'])
    debugPrint ('op_index: ', op_index)
    op = ''
    while 1:
        
        # handle 5000 operations once
        for i in range(sig._range):
            line = f.readline()
            if not line:
                eof = 1
                break
            words = string.split(line)
            # there might be some blank lines
            if len(words) < 6:
                j+=1
                continue

            ## only "READ" and "WRITE" will be saved
            #if words[-1].count('READ') == 0 and words[-1].count('WRITE') == 0:
            # to test chomob, only use write
            # if words[-1].count('WRITE') == 0:
            #    j+=1
            #    continue

            ## save to list
            op = words[op_index].upper();
            acc = Access(words)

            if acc.size >= 1:
                if op.count('READ')>0 or op == 'R':
                    debugPrint("one READ")
                    rlist.append(acc)
                    total_read_count += 1
                    total_read_time += acc.endTime - acc.startTime

                if op.count('WRITE')>0 or op == 'W':
                    debugPrint("one WRITE")
                    wlist.append(acc)
                    total_write_count += 1
                    total_write_time += acc.endTime - acc.startTime
        # finish reading a batch of 5000 lines of the trace file

        # Generate all kinds of CSV files using the rlist and wlist
        # here the write operation should be "append"
        # because it's handling 5000 lines each time
        if (len(rlist) > 0):
            rlist.toIORStep(trace_filename, 'r') # 'r' for read
            rlist.toDataAccessHoleSizes(trace_filename, 'r')
            rlistEmpty = 0
        if (len(wlist) > 0):
            wlist.toIORStep(trace_filename, 'w') # 'w' for write
            wlistEmpty = 0

        # empty the two lists
        rlist = AccList()
        wlist = AccList()
        gc.collect()   # garbage collection

        # reached EOF? exit the "while 1" loop
        if eof == 1:
            break

    ## close the opened file
    f.close()
    if (rlistEmpty == 1):
        readF = open( os.path.join(sig._out_path, trace_filename + ".read.rate.csv"), 'a+')
        readF.write( "{0},{1}\n".format(0, 0) )
        readF.close()
        readF = open( os.path.join(sig._out_path, trace_filename + ".read.hole.sizes.csv"), 'a+')
        readF.write( "{0},{1}\n".format(0, 0) )
        readF.close()

    if (wlistEmpty == 1):
        writeF = open( os.path.join(sig._out_path, trace_filename + ".write.rate.csv"), 'a+')
        writeF.write( "{0},{1}\n".format(0, 0) )
        writeF.close()
    # TODO: gnuplot for read and write rates

    # save the statistics information to files
    output = os.path.join(sig._out_path, trace_filename + ".stat.properties")
    f = open(output, 'a+')
    f.write("total_read_time: {0}\n".format(total_read_time))
    f.write("total_read_count: {0}\n".format(total_read_count))
    f.write("total_write_time: {0}\n".format(total_write_time))
    f.write("total_write_count: {0}\n".format(total_write_count))
    #f.write("global_total_read_time: {0}\n".format(sig._total_read_time))
    #f.write("global_total_write_time: {0}\n".format(sig._total_write_time))



