#!/usr/bin/env python

"""
The implementation of the cost-model.

Usage: python costmodel.py [options]

Options:
  -p ..., --procNum=...                Number of I/O client processes.
  -s ..., --requestSize=...            Data size of one access.
  
  -n ..., --storNum=...                Number of storage nodes.
  -c ..., --clientNum=...              Number of client nodes.
  -g ..., --group=...                  Number of storage groups in 2-D layout.

  -e ..., --estCost=...                Cost of single network connection establishing.
  -v ..., --tranCost=...               Network transmission cost of one unit of data.
  -a ..., --diskStart=...              Start up time of one disk I/O operation.
  -b ..., --diskIO=...                 Cost of reading/writing one unit of data.
"""

import sys, os, string, getopt
import math

# usage
def usage():
    print __doc__

def oneDVCost(p, n, m, e, s, v, a, b):
    cost = math.ceil(p/n)*a + math.ceil(p/n)*s*b

    if p<=n:
        cost += m*e + m*s*v
    if p>n and m<=math.ceil(p/n):
        cost += math.ceil(p/n)*e + math.ceil(p/n)*s*v 
    if p>n and m>math.ceil(p/n):
        cost += m*e + m*s*v
                
    print '1-DV Cost: ', cost 

def oneDHCost(p, n, m, e, s, v, a, b):
    cost = p*a + p*s*b/n

    if m<=math.ceil(p/n):
        cost += p*e + p*s*v/n
    if m>math.ceil(p/n):
        cost += m*n*e + m*s*v
                
    print '1-DH Cost: ', cost 

def twoDCost(p, n, m, e, s, v, a, b):
    cost = 0.0

    for g in [2.0, 4.0]:
        cost = math.ceil(p/g)*a + math.ceil(p/g)*s*b/math.floor(n/g) 
        if p<=g:
            cost += m*e*math.ceil(n/g) + m*s*v
        if p>g and m<=math.ceil(p/g)/math.floor(n/g):
            cost += e*math.ceil(p/g) + math.ceil(p/g)*s*v/math.floor(n/g) 
        if p>g and m>math.ceil(p/g)/math.floor(n/g):
            cost += m*e*math.ceil(p/g) + m*s*v 
                
        print 'group: ', g, ', 2-D Cost: ', cost 

def modelCalculation(p, n, m, e, s, v, a, b):
    oneDVCost(p, n, m, e, s, v, a, b)
    oneDHCost(p, n, m, e, s, v, a, b)
    twoDCost(p, n, m, e, s, v, a, b)

# main function
def main(argv):
    """Main method"""

    p = 8.0
    n = 8.0
    c = 42.0 
    s = 16.0 * 1024 
    e = 0.0003
    v = 1.0 / 120.0 / 1024.0 / 1024.0
    a = 0.0001
    b = 1.0 / 120.0 / 1024.0 / 1024.0

    if len(argv) == 0:
        usage()
        sys.exit(2)

    try:
        opts, args = getopt.getopt(argv, "p:n:c:s:g:", ["procNum=", "storNum=", "clientNum=", "requestSize=", "group="]) 
    except getopt.GetoptError:
        usage()
        sys.exit(2)

    for opt, arg in opts:
        if opt in ("-p", "--procNumber"):
            p = float(arg)
        elif opt in ("-n", "--storNum"):
            n = float(arg)
        elif opt in ("-c", "--clientNum"):
            c = float(arg)
        elif opt in ("-s", "--requestSize"):
            s = float(arg)
        #elif opt in ("-g", "--group"):
        #    g = float(arg)
        
    if p<42:
        c=p
    m = math.ceil(1.0*p/c)

    modelCalculation(p, n, m, e, s, v, a, b)


if __name__ == '__main__':
    main(sys.argv[1:])
    print 'END'
