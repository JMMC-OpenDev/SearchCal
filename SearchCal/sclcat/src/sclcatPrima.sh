#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclcatPrima.sh,v 1.4 2007-01-31 10:02:23 mella Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.3  2006/11/27 10:31:19  scetre
# Added Fortrant treatment in the catalogue file generation
#
# Revision 1.2  2006/11/06 15:30:26  scetre
# Cancelled removal of CVS repository when generated reference
#
# Revision 1.1  2006/10/31 14:23:54  gzins
# Added
#
#*******************************************************************************

#/**
# @file
# Create calibrator catalog for PRIMA 
#
# @synopsis
# \<sclcatPrima\>
# 
# @details
# This script creates catalog of calibrator for PRIMA, using SearchCal tool. The
# result is either placed in :
# \li \em prima-ref directory if \em -g option is specified
# \li \em prima-\<date\> directory otherwise
#
# The log files, resulting of the SearchCal execution for each star, are stored
# in \em log sub-directory.
#
# @usedfiles
# @filename ../config/sclcatPRIMA.cfg : list of stars of interest for PRIMA
#
# */

# Print usage 
function printUsage () {
        echo -e "Usage: sclcatPrima [-h] [-g] [-c]" 
        echo -e "\t-h\tprint this help."
        echo -e "\t-c\tgenerate the configuration file."
        echo -e "\t-g\tgenerate reference."
        exit 1;
}

# generate the configuration file
function generateConfig () {
        echo -e "Generate configuration file ..."
        ../bin/sclcatPrimaPreClass
        ../bin/sclcatPrimaConcat
        echo -e "...Done."
}

# Parse result
function parseResult () {
        echo -e "Analyse the results..."
        sclcatPrimaParseResult $1
        echo -e "...Done."
}

# Parse command-line parameters
dir="prima-run-`date +%Y-%m-%dT%H-%M-%S`";

while getopts "hrgc" option
# Initial declaration.
# c, h, u and t are the options (flags) expected.
# The : after option 't' shows it will have an argument passed with it.
do
  case $option in
    h ) # Help option
        printUsage ;;
    g ) # generation reference option
        dir="prima-ref";;
    c ) # generate the configuration file
        generateConfig ;
        exit 1;;
    * ) # Unknown option
        printUsage ;;
    esac
done


# Prepare an empty directory 
if [ $dir == "prima-ref" ]; then
    rm -f $dir/*
    rm -rf $dir/log
else
    rm -rf $dir
    mkdir $dir
fi

# Start program
cd $dir
cmdBatch ../../config/sclcatPRIMA.cfg  -d log

cd ..
parseResult $dir

#___oOo___
