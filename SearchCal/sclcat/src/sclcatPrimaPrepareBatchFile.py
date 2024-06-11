#!/usr/bin/python
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************
""" This script aims to modify a config file adding radius parameter
"""

import ConfigParser
import os
import sys
import os.path
import os
from optparse import OptionParser
import math

def getRadius(pmra, pmdec, timespan):
    """ Returns radius to the given proper motion and timespan
    -  pmra    mas/yr (*cos(delta))
    -  pmdec   mas/yr
    -  timespan yr
    """
    vitsky=math.sqrt(pmra**2+pmdec**2)/1000.0/60.0
    dkal=vitsky*timespan+1.0
    return dkal

    
class MyParser(ConfigParser.ConfigParser):
    #make option get case sensitive especially for parameter name
    def optionxform(self, option):
        return option

def main(inputFilename, outputFilename):
    timespan=20
    print "Timespan is set to ", timespan," yr"
    
    config = MyParser()
    try:
        config.readfp(open(inputFilename))
    except:
        print "Problem reading " + inputFilename
        return
    
    print "nb of sections:",len(config.sections())

    for s in config.sections():
        # Choose right command for this section        
        #if not config.has_option(s,"file"):
        #    config.set(s,"file",s+"."+ext)
        objectName=config.get(s, "objectName")
        radius=1.0
        try:
            pmra=float(config.get(s, "pmra"))
            pmdec=float(config.get(s, "pmdec"))
            radius=getRadius(pmra, pmdec,timespan)
        except Exception:
            print objectName ," has no pmra or pmdec "
        config.set(s, "radius", radius)
 
        # remove pmra and pmdec
        try:
            config.remove_option(s,"pmra")
        except Exceptio:
            pass
        try:
            config.remove_option(s,"pmdec")
        except Exceptio:
            pass

        # add file information
        config.set(s,"file", objectName+".vot")
           

    output=open(outputFilename,"w")
    config.write(output)
    print "New file has been writen into ",outputFilename


if __name__ == '__main__':
    usage="""usage: %prog [options] configFile.cfg """
    parser = OptionParser(usage=usage)
    parser.add_option("-o", "--output", dest="outputFilename", metavar="OUT",  
            help="Output result in given file instead of default 'output.txt'")

    (options, args) = parser.parse_args()
   
    outputFilename="output.txt"
    if options.outputFilename:
        outputFilename=options.outputFilename
    
    if len(args) != 1:
        parser.print_help()
        sys.exit(1)

    if not os.path.isfile(args[0]) :
        sys.stderr.write("File not found '%s'"%(args[0],))
        sys.exit(1)
    
    main(args[0], outputFilename)     
