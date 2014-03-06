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

import sys, os, string, getopt, gc, multiprocessing
import single_trace_analysis, global_analysis, access, accList, prop, util
from single_trace_analysis import *
from global_analysis import *
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
sig._format_file = "format.properties"
sig._format_prop = None
sig._protobuf = 0
sig._out_path = "result_output"
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
    if len(sig._trace_path) > 0 and os.path.isdir(sig._trace_path):
        posix_traces = [ filename for filename in os.listdir(sig._trace_path) if filename.startswith('posix') and filename.endswith('.out') ]
        pool = multiprocessing.Pool()
        pool.map(generateCSVs, posix_traces)
    else:
        if len(sig._trace_file) > 0 and os.path.isfile(sig._trace_file):
            generateCSVs(sig._trace_file)

    global_analysis()
    # detect patterns
    # and generates IO rates figure
    # detectSignature(filename)

# main function
def parse_args(argv):
    """The method parses the command arguments."""

    if len(argv) == 0:
        usage()
        sys.exit(2)
        
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
            sig._trace_file = os.path.abspath(arg)
        elif opt in ("-d", "--directory"):
            sig._trace_path = os.path.abspath(arg)
        elif opt in ("-r", "--range"):
            sig._range = int(arg)
        elif opt in ("-b", "--blksz"):
            sig._blksz = int(arg) * 1024
        elif opt in ("-m", "--formatFile"):
            sig._format_file = arg

    sig._format_prop = Properties()
    sig._format_prop.load(open(sig._format_file)) # file close?
    debugPrint( sig._format_prop )
    debugPrint( sig._format_prop.items())
    debugPrint( sig._format_prop['skip_lines'])

    if len(sig._trace_path) > 0 and os.path.isdir(sig._trace_path):
        print "Processing the traces in path: " + sig._trace_path
        sig._out_path = os.path.join(sig._trace_path, sig._out_path)
    else:
        if len(sig._trace_file) > 0 and os.path.isfile(sig._trace_file):
            print "Processing one trace file: " + sig._trace_file
            sig._out_path = os.path.join( os.path.dirname(sig._trace_file), sig._out_path )
        else:
            print '\033[1;41mSorry, trace file \''+sig._trace_file+'\' does not exist!\033[1;m'
            sys.exit()

    #sig._out_path = os.path.abspath(sig._out_path)
    if not os.path.isdir(sig._out_path):
        print "Output put directory does not exist, create it."
        os.makedirs(sig._out_path)
    # Finished handling arguments

if __name__ == '__main__':
    main(sys.argv[1:])
    print '\033[1;42mIOSIG analysis done.\033[1;m'
    #print '~ END ~'


