#!/usr/bin/python
#******************************************************************************
# JMMC project
#
# "@(#) $Id: cmdBatch.py,v 1.2 2005-12-16 15:36:43 mella Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.1  2005/12/16 15:35:52  mella
# First revision
#
#
#************************************************************************
#
""" This script aims to launch multiple request based onto an input file that must be 
given as first parameter.
The config file must contain the minimum line:
[DEFAULT]
command=msgSendCommand sclsvrServer GETCAL

Then following section will 
"""

import ConfigParser
import os
import sys
import os.path
import os
from optparse import OptionParser

Id="@(#) $Id: cmdBatch.py,v 1.2 2005-12-16 15:36:43 mella Exp $"

# default output will 
resultDir="results"

class MyParser(ConfigParser.ConfigParser):
    #make option get case sensitive especially for parameter name
    def optionxform(self, option):
        return option

def main(filename):
    config = MyParser()
    try:
        config.readfp(open(filename))
    except:
        print "Problem reading " + filename
        return

    # Store new config summary
    newCfg=open(filename+".tmp","w")
    config.write(newCfg)
    print "A summary "+filename+".tmp"

    # Start to build batch list for every section
    batchList=[]
    defaultCommand = config.defaults()["command"]
    # Remove command to sections else every commands will  getnext arg:
    # -command XXX as argument
    config.remove_option("DEFAULT", "command")
    
    for s in config.sections():
        # Choose right command for this section        
        if config.has_option(s,"command"):
            command=config.get(s,"command")
            config.remove_option(s, "command")
        else:
            command=defaultCommand
            
        cmd = command
        cmd += ' "'

        # And append to cmd every option  -optionName Value ...
        for n,v in config.items(s):
            if n and v:
                cmd += "-"+n+" "+v+" "
            else: 
                sys.stderr.write("ERROR: Missing value for '%s' item into '%s' section"%(n,s))
                sys.exit(1)
        cmd += ' "'

        # Store new batch into file " are placed for case were section includes
        # spaces
        batchList.append( cmd + ' > "' + resultDir + os.path.sep + s + '.out"' ) 

    # Execute batch line by line
    for cmd in batchList:
        print cmd
        os.system(cmd)

if __name__ == '__main__':
    usage="""usage: %prog [options] configFile.cfg"""
    parser = OptionParser(usage=usage)
    parser.add_option("-d", "--directory", dest="resultDir", metavar="DIR",  
            help="Output results in given directory instead of default 'results'")

    (options, args) = parser.parse_args()
    
    if options.resultDir:
        resultDir=options.resultDir
    
    if len(args) != 1:
        parser.print_help()
        sys.exit(1)

    if not os.path.isfile(args[0]) :
        sys.stderr.write("File not found '%s'"%(args[0],))
        sys.exit(1)
    
    try:
        print "This batch will loop over objects defined into '%s'." % (args[0],)
        print "Output directory for results '%s'"%(resultDir,)
        print "Press Control-C to stop process now or something else to \
continue"
        raw_input()
        
        if not os.path.isdir(resultDir) :
            try:
                os.mkdir(resultDir)
                print "'%s' directory has been created." %(resultDir,) 
            except:
                sys.stderr.write( "Failed to create '%s' directory." %(resultDir,))
       
   
        main(args[0])     
    except:
        print "Usage: %s <inputscript.cfg>"%(sys.argv[0],)
