#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclcatESO.sh,v 1.1 2008-07-11 12:56:55 lafrasse Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
#*******************************************************************************

#/**
# @file
# Create calibrator catalog for ESO
#
# @synopsis
# \<sclcatESO\> [\e \<-g\>] [\e \<instrumentalConfigurationFile\>]
#
# @param instrumentalConfigurationFile : file containing the ESO instrumental
# configuration the calibrator catalog should be generated for.
#
# @opt
# @optname h : show usage help
# @optname g : generate a reference catalog
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
        echo -e "Usage: sclcatESO [-h] [-g] [-c]" 
        echo -e "\t-h\tprint this help."
        echo -e "\t-c\tgenerate the configuration file."
        echo -e "\t-g\tgenerate reference."
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

# Parse command-line parameters
dir="eso-run-`date +%Y-%m-%dT%H-%M-%S`";

while getopts "hrgc" option
# Initial declaration.
# c, h, u and t are the options (flags) expected.
# The : after option 't' shows it will have an argument passed with it.
do
  case $option in
    h ) # Help option
        printUsage ;;
    g ) # generation reference option
        dir="eso-ref";;
    c ) # generate the configuration file
        sclcatESOGenerateConfig ;
        exit ;;
    * ) # Unknown option
        printUsage ;;
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
cmdBatch ../../config/sclcatESO.cfg  -d log

cd ..
parseResult $dir

#___oOo___
