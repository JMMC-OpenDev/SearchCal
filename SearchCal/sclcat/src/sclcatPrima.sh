#!/bin/bash
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************

#/**
# @file
# Create calibrator catalog for PRIMA 
#
# @synopsis
# sclcatPrima
# 
# @opt
# @optname h : show usage help
# @optname g : generate a reference catalog
# @optname c : generate a configuration file
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
# The results of SearchCal are VOTables which are converted to html for
# visualisation.
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

#/**
# Parse result
# */
function parseResult () {
        echo -e "Analyse the results..."
        sclcatPrimaParseResult $1
        echo -e "...Done."
}

# Parse command-line parameters
dir="../tmp/prima-run-`date +%Y-%m-%dT%H-%M-%S`";

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
        sclcatPrimaGenerateConfig ;
        exit ;;
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

cd -
parseResult $dir

#___oOo___
