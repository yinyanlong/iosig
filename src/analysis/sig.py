#!/usr/bin/env python
"""
I/O Signature Detection Program

Detect the I/O Signatures in a I/O trace file

Usage:  python sig.py [options][source]
     or
       ./detect_iosig [options][source]

Options:
  -h, --help                show this help
  -g                        show debugging information while parsing
  -f ..., --filename=...    the filename of the trace file
  -b ..., --blksz=...       size of block for block based detection
  -r ..., --range=...       # of entries will be processed
  -m ..., --formatFile=...  the properties file for the trace format information
  -p                        require the program to generate one protobuf file
"""

__author__ = "Yanlong Yin (yyin2@iit.edu)"
__version__ = "$Revision: 1.4$"
__date__ = "$Date: 02/08/2014 $"
__copyright__ = "Copyright (c) 2010-2014 SCS Lab, IIT"
__license__ = "Python"

import sys, os, string, getopt, gc
from access import *
from accList import *
from prop import *
from util import *

# global variables
global _range         # the number of accesses will be processed
global _blksz         # the block size - minimal cache unit
global _debug
global _format_file
global _format_prop
global _protobuf
global _out_path
global _trace_file

global _total_read_time
global _total_write_time
sig._total_read_time = 0.0
sig._total_write_time = 0.0

# the following parameters can be specified by user's command line arguments
sig._range = 5000 #5000 each time, so mem consumption is about 5000 lines
sig._blksz = 0
sig._debug = 0
sig._format_file = "standard.properties"
sig._format_prop = None
sig._protobuf = 0
sig._out_path = "./result_output"
sig._trace_file = ""
sig._trace_path = ""

# usage
def usage():
    print __doc__

# main function
def main(argv):
    """Main method of this software"""
    parse_args(argv)
    # generate iorates figure
    # generateIORates(filename)

    # translate the list to "step data" into "*.dat" files
    # generate R/W bandwidth over time figures
    generateCSVs(sig._trace_file)

    # detect patterns
    # and generates IO rates figure
    # detectSignature(filename)

# main function
def parse_args(argv):
    """The method parses the command arguments."""

    if len(argv) == 0:
        usage()
        sys.exit(2)
        
    filename = ''
    # deal with command arguments
    try:
        opts, args = getopt.getopt(argv, "hgpr:b:f:m:d:", ["help", "debug", "protobuf", "range=", "blksz=", "filename=", "formatFile=", "directory="])
    except getopt.GetoptError:
        usage()
        sys.exit(2)

    for opt, arg in opts:
        if opt in ("-h", "--help"):
            usage()
            sys.exit()
        elif opt in ("-g", "--debug"):
            sig._debug = 1
        elif opt in ("-p", "--protobuf"):
            sig._protobuf = 1
        elif opt in ("-f", "--filename"):
            filename = arg
            sig._trace_file = arg
        elif opt in ("-d", "--directory"):
            sig._trace_path = arg
        elif opt in ("-r", "--range"):
            sig._range = int(arg)
        elif opt in ("-b", "--blksz"):
            sig._blksz = int(arg) * 1024
        elif opt in ("-m", "--formatFile"):
            sig._format_file = arg

    sig._format_prop = Properties()
    sig._format_prop.load(open(sig._format_file))
    debugPrint( sig._format_prop )
    debugPrint( sig._format_prop.items())
    debugPrint( sig._format_prop['skip_lines'])

    if filename and os.path.isfile(filename):
        print "Using trace file: " + filename
    else:
        print '\033[1;41mSorry, trace file \''+filename+'\' does not exist!\033[1;m'
        sys.exit()

    if not os.path.isdir(sig._out_path):
        print "Output put directory does not exist, create it."
        os.makedirs(sig._out_path)
    # Finished handling arguments

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

def generateCSVs(filename):
    """Generate the Read/Write Bandwidth figures"""

    # the list contains all the accesses
    rlist = AccList()
    wlist = AccList()
    rlistEmpty = 1
    wlistEmpty = 1

    total_read_count = 0
    total_write_count = 0
    total_read_time = 0.0
    total_write_time = 0.0

    #read_rate_output_file = open("result_output/read.dat", 'a')
    #write_rate_output_file = open("result_output/write.dat", 'a')

    # Create and empty each CSV files, write the CSV title line
    output = sig._out_path + "/" + sig._trace_file + ".read.rate.csv"
    f = open(output, 'w')
    f.write("Time,Rate\n")
    f.close()
    output = sig._out_path + "/" + sig._trace_file + ".write.rate.csv"
    f = open(output, 'w')
    f.write("Time,Rate\n")
    f.close()
    output = sig._out_path + "/" + sig._trace_file + ".read.hole.sizes.csv"
    f = open(output, 'w')
    f.write("Time,Size\n")
    f.close()

    # open the trace file
    f = open(filename, 'r')
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

        # translate the list to "step data" into "*.csv" files
        # here the write operation should be "append"
        # because it's handling 5000 lines each time
        if (len(rlist) > 0):
            output = sig._out_path + "/" + sig._trace_file + ".read.rate.csv"
            rlist.toIORStep(output, 1) # 1 for read
            output = sig._out_path + "/" + sig._trace_file + ".read.hole.sizes.csv"
            rlist.toDataAccessHoleSizes(output)
            rlistEmpty = 0
        if (len(wlist) > 0):
            output = sig._out_path + "/" + sig._trace_file + ".write.rate.csv"
            wlist.toIORStep(output, 2) # 2 for write
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
        readF = open(sig._out_path + "/" + sig._trace_file + ".read.dat", 'a+')
        readF.write( "{0} {1}\n".format(0, 0) )
        readF.close()
    else:
        print "gnuplot"
        # TODO: gnuplot

    if (wlistEmpty == 1):
        writeF = open(sig._out_path + "/" + sig._trace_file + ".write.dat", 'a+')
        writeF.write( "{0} {1}\n".format(0, 0) )
        writeF.close()
    else:
        print "gnuplot"

    # save the statistics information to files
    output = sig._out_path + "/" + sig._trace_file + ".statistics.dat"
    f = open(output, 'a+')
    f.write("total_read_time: {0}\n".format(total_read_time))
    f.write("total_read_count: {0}\n".format(total_read_count))
    f.write("total_write_time: {0}\n".format(total_write_time))
    f.write("total_write_count: {0}\n".format(total_write_count))
    f.write("global_total_read_time: {0}\n".format(sig._total_read_time))
    f.write("global_total_write_time: {0}\n".format(sig._total_write_time))
        # TODO: gnuplot
    
if __name__ == '__main__':
    main(sys.argv[1:])
    print '~ END ~'


