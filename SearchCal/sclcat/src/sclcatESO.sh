#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclcatESO.sh,v 1.5 2008-12-01 09:38:22 lafrasse Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.4  2008/07/23 22:31:07  lafrasse
# Code polishing.
#
# Revision 1.3  2008/07/17 07:12:35  lafrasse
# Added an option to generate config file and run commands.
#
# Revision 1.2  2008/07/11 16:15:02  lafrasse
# Moved generation directo from src/ to tmp/.
# Added exectution timestamps.
# Code syntax polishing.
#
# Revision 1.1  2008/07/11 12:56:55  lafrasse
# Added ESO Calibrator catalog generation scripts.
#
#*******************************************************************************

#/**
# @file
# Create calibrator catalog for ESO
#
# @synopsis
# \<sclcatESO\> [\e \<-g\> \<-c\> \<-r\>] [\e \<instrumentalConfigurationFile\>]
#
# @param instrumentalConfigurationFile : file containing the ESO instrumental
# configuration the calibrator catalog should be generated for.
#
# @opt
# @optname h : show usage help
# @optname g : generate a reference catalog
# @optname c : generate the configuration file and exit
# @optname r : generate the configuration file and run
# 
# @details
# This script creates catalog of calibrator for ESO, using SearchCal tool. The
# result is either placed in :
# \li \em eso-ref directory if \em -g option is specified
# \li \em eso-\<date\> directory otherwise
#
# The log files, resulting of the SearchCal execution for each star, are stored
# in \em log sub-directory.
#
# The results of SearchCal are VOTables which are merged into a single result
# file.
# 
# */

# Print usage 
function printUsage () {
        echo -e "Usage: sclcatESO [-h] [-g] [-c] [-r]" 
        echo -e "\t-h\tprint this help."
        echo -e "\t-g\tgenerate reference."
        echo -e "\t-c\tgenerate the configuration file and exit."
        echo -e "\t-r\tgenerate the configuration file and run."
        exit 1;
}

#/**
# Parse result
# */
function parseResult () {
        echo -e "Analyse the results..."
        sclcatESOParseResult $1
        echo -e "...Done."
}

echo -n "Started at: "
date

# Generate output directory name
dir="../tmp/eso-run-`date +%Y-%m-%dT%H-%M-%S`";

# Parse command-line parameters
while getopts "hrgc" option
do
    case $option in
    h ) # Help option
        printUsage ;;
    g ) # generation reference option
        dir="eso-ref" ;;
    c ) # generate the configuration file and exit
        sclcatESOGenerateConfig ;
        exit ;;
    r ) # generate the configuration file and run
        sclcatESOGenerateConfig ;;
    [?] ) # Unknown option
        printUsage ;
        exit ;;
    esac
done


# Prepare an empty directory 
if [ $dir == "eso-ref" ]; then
    rm -f $dir/*
    rm -rf $dir/log
else
    rm -rf $dir
    mkdir $dir
fi

# Start program
cd $dir
cmdBatch ../../config/sclcatESO.cfg -d log

cd ..
parseResult $dir

echo -n "Finished at: "
date

#___oOo___
