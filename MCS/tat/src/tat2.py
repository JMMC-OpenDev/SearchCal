#!/usr/bin/python
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************
""" This script aims to reproduce at a lower lever the old tat functionnality.
"""

import ConfigParser
import os
import sys
import os.path
import os
from optparse import OptionParser

Id="@(#) $Id: tat2.py,v 1.1 2011-02-23 14:21:23 mella Exp $"

def executeTest(idx,reffile,cmd):
    print ("\n\n\nExecuting test %s (%s)"%(idx,cmd))
    outfile = reffile.replace(".ref",".test")
    os.system(cmd + """ | awk '{print "1 -",$0}'>""" + outfile)
    os.system("echo diff "+reffile+" "+outfile)
    os.system("diff "+reffile+" "+outfile)


def executeTestFile(filename):
    testfile = open(filename)
    for l in testfile.readlines():
        idx,reffile,cmd=l.split()
        executeTest(idx,reffile+".ref",cmd)

            
if __name__ == '__main__':
    usage="""usage: %prog [options]\n Read and executes tests reading 'TestList' file."""
    parser = OptionParser(usage=usage)

    (options, args) = parser.parse_args()
    
    if len(args) != 0:
        parser.print_help()
        sys.exit(1)
    filename="TestList"
    if not os.path.isfile(filename) :
        sys.stderr.write("'TestList' file not found '%s'"%(args[0],))
        sys.exit(1)

    executeTestFile(filename)
