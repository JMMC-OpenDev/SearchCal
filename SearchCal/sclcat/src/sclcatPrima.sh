#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclcatPrima.sh,v 1.1 2006-10-31 14:23:54 gzins Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
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
        echo -e "Usage: sclcatPrima [-h] [-g]" 
        echo -e "\t-h\tprint this help."
        echo -e "\t-g\tgenerate reference."
        exit 1;
}

# Parse command-line parameters
dir="prima-run-`date +%Y-%m-%dT%H:%M:%S`";

while getopts "hg" option
# Initial declaration.
# c, h, u and t are the options (flags) expected.
# The : after option 't' shows it will have an argument passed with it.
do
  case $option in
    h ) # Help option
        printUsage ;;
    g ) # generation reference option
        dir="prima-ref";;
    * ) # Unknown option
        printUsage ;;
    esac
done

# Prepare an empty directory 
rm -rf $dir
mkdir $dir

# Start program
cd $dir
cmdBatch ../../config/sclcatPRIMA.cfg  -d log

#___oOo___
