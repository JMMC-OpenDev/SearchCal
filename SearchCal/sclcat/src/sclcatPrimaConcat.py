#!/usr/bin/python
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclcatPrimaConcat.py,v 1.1 2006-11-27 10:31:19 scetre Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
#*******************************************************************************

import ConfigParser
import os
import sys
import os.path
import os

def main():
    
    headerFile = open("../config/sclcatPrimaHeader.cfg", "r")
    headerFileList = headerFile.readlines()
    headerFile.close()
    
    commandFile =  open("../tmp/command.dat", "r")
    commandFileList = commandFile.readlines() 
    commandFile.close()
    
    print len(commandFileList)
    
    sclcatPRIMA = open("../config/sclcatPRIMA.cfg","w")
    for i in headerFileList:
        sclcatPRIMA.write(i)
    for j in commandFileList:
        sclcatPRIMA.write(j)
    sclcatPRIMA.close()

if __name__ == '__main__':
    main()     
