#!/usr/bin/env python
"""
Global Analysis
"""

__author__ = "Yanlong Yin (yyin2@iit.edu)"
__version__ = "$Revision: 1.4$"
__date__ = "$Date: 02/08/2014 $"
__copyright__ = "Copyright (c) 2010-2014 SCS Lab, IIT"
__license__ = "Python"

import sys, os, string, getopt, gc, multiprocessing
import csv
from access import *
from accList import *
from prop import *
from util import *
from sig import *

def global_analysis():
    calculate_exe_time()
    calculated_total_io_time()

def calculate_exe_time():
    """The methon analyzes the global statistics data."""
    end_times = []
    exe_traces = [exe_trace for exe_trace in os.listdir(sig._trace_path) if exe_trace.startswith("exe")]
    for exe_trace in exe_traces:
        with open(exe_trace, 'r') as f:
            last_line = tail(f)
            if len(last_line) > 0:
                words = last_line[0].split()
                if len(words) > 3:
                    end_times.append(float(words[3]))

    global_end_time = max(end_times)
    
    output = os.path.join(sig._out_path, "global.stat.properties")
    f = open(output, 'a+')
    f.write("global_exe_time: {0}\n".format(global_end_time))
    f.close()

def calculated_total_io_time():
    """Calculate the total I/O time in the non-overlap mode."""
    #read_io_intervals = []
    #write_io_intervals = []

    read_interval_csvs = [csv_file for csv_file in os.listdir(sig._out_path) if csv_file.endswith("read.interval.csv")]
    write_interval_csvs = [csv_file for csv_file in os.listdir(sig._out_path) if csv_file.endswith("write.interval.csv")]
    #read_io_intervals = analyze_multiple_interval_csv(read_rate_csvs)
    #write_io_intervals = analyze_multiple_interval_csv(write_rate_csvs)
    global_read_count, global_read_time, global_read_time_nonoverlap = analyze_multiple_interval_csv(read_interval_csvs)
    global_write_count, global_write_time, global_write_time_nonoverlap = analyze_multiple_interval_csv(write_interval_csvs)

    output = os.path.join(sig._out_path, "global.stat.properties")
    f = open(output, 'a+')
    f.write("global_read_count: {0}\n".format(global_read_count))
    f.write("global_read_time: {0}\n".format(global_read_time))
    f.write("global_read_time_nonoverlap: {0}\n".format(global_read_time_nonoverlap))
    f.write("global_write_count: {0}\n".format(global_write_count))
    f.write("global_write_time: {0}\n".format(global_write_time))
    f.write("global_write_time_nonoverlap: {0}\n".format(global_write_time_nonoverlap))
    f.close()
        
def analyze_multiple_interval_csv(csvs):
    io_intervals = []
    total_io_count = 0
    total_io_time = 0.0
    total_io_time_nonoverlap = 0.0
    for io_rate_csv in csvs:
        #print io_rate_csv
        with open(os.path.join(sig._out_path, io_rate_csv), 'Ur') as f:
            tmp_interval_list = list(tuple(rec) for rec in csv.reader(f, delimiter=','))
        if len(tmp_interval_list) <= 1:
            continue
        total_io_count += len(tmp_interval_list) - 1
        total_io_time += sum([float(interval[1]) for interval in tmp_interval_list[1:]]) - sum([float(interval[0]) for interval in tmp_interval_list[1:]])
        io_intervals += tmp_interval_list[1:]
        io_intervals.sort()
        io_intervals = list(merge_intervals(io_intervals))
    
    total_io_time_nonoverlap += sum([float(interval[1]) for interval in io_intervals]) - sum([float(interval[0]) for interval in io_intervals])
    # if necessary, `io_intervals` may also returned
    return (total_io_count, total_io_time, total_io_time_nonoverlap)

