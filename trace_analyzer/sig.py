#!/usr/bin/env python
"""
I/O Signature Detection Program

Detect the I/O Signatures in a I/O trace file

Usage:  python sig.py [options][source]
     or
       ./detect_iosig [options][source]

Options:
  -h, --help                show this help
  -d                        show debugging information while parsing
  -f ..., --filename=...    the filename of the trace file
  -b ..., --blksz=...       size of block for block based detection
  -r ..., --range=...       # of entries will be processed
  -m ..., --formatFile=...  the properties file for the trace format information
  -p                        require the program to generate one protobuf file
"""

__author__ = "Yanlong Yin (yyin2@iit.edu)"
__version__ = "$Revision: 1.3.2$"
__date__ = "$Date: 03/02/2011 18:08 $"
__copyright__ = "Copyright (c) 2010-2011 SCS IIT"
__license__ = "Python"

import sys, os, string, getopt
from access import *
from accList import *
from prop import *
from util import *

# the following parameters can be specified by user's command line arguments
_range = 50000     # the number of accesses will be processed
_blksz = 0         # the block size - minimal cache unit
_debug = 0
_format_file = "standard.properties"
_format_prop = None
_protobuf = 0

# usage
def usage():
    print __doc__

# main function
def main(argv):
    """Main method of this software"""

    if len(argv) == 0:
        usage()
        sys.exit(2)
        
    # deal with command arguments
    try:
        opts, args = getopt.getopt(argv, "hdr:b:f:m:", ["help", "range=", "blksz=", "filename=", "formatFile="])
    except getopt.GetoptError:
        usage()
        sys.exit(2)

    for opt, arg in opts:
        if opt in ("-h", "--help"):
            usage()
            sys.exit()
        elif opt in ("-d"):
            global _debug
            _debug = 1
        elif opt in ("-p"):
            global _protobuf
            _protobuf = 1
        elif opt in ("-f", "--filename"):
            filename = arg
        elif opt in ("-r", "--range"):
            global _range
            _range = int(arg)
        elif opt in ("-b", "--blksz"):
            global _blksz
            _blksz = int(arg) * 1024
            debugPrint( 'xxxxxxxxxx', sig._blksz)
        elif opt in ("-m", "--formatFile"):
            sig._format_file = arg

    sig._format_prop = Properties()
    sig._format_prop.load(open(sig._format_file))
    print sig._format_prop
    print sig._format_prop.items()
    print sig._format_prop['skip_lines']

    if filename and os.path.isfile(filename):
        print "Using trace file: " + filename
    else:
        print '\033[1;41mSorry, trace file \''+filename+'\' does not exist!\033[1;m'

        sys.exit()

    # the list contains all the accesses
    rlist = AccList()
    wlist = AccList()

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
    for i in range(_range):
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
        if op.count('READ')>0 or op == 'R':
            acc = Access(words)
            rlist.append(acc)

        if op.count('WRITE')>0 or op == 'W':
            acc = Access(words)
            wlist.append(acc)

    ## close the opened file
    f.close()
    rlist.trace = filename
    wlist.trace = filename

    ## print the list
    #for i in acc_list:
    #print i

    ## deal with the list
    rlist.detect_signature(0, min(_range-j-1, len(rlist)-1) )
    wlist.detect_signature(0, min(_range-j-1, len(wlist)-1) )

    ## Done with the whole process of detecting
    ## Print the whole signature
    if len(rlist.signatures) > 0 or len(wlist.signatures) > 0:
        print '----------------------------------------'
        print 'The following signatures are detected:'

    if len(rlist.signatures) > 0:
        rlist.print_signature()
        rlist.gen_protobuf("./makeup_output")
        rlist.makeup_output("./makeup_output")

    if len(wlist.signatures) > 0:
        wlist.print_signature()
        wlist.gen_protobuf("./makeup_output")
        wlist.makeup_output("./makeup_output")

if __name__ == '__main__':
    main(sys.argv[1:])
    print '~ END ~'


