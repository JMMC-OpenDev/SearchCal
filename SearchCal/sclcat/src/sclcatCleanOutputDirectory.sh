#!/bin/bash
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************

#/**
# @file
# Clean and compress all possible files in an output directory.
#
# @synopsis
# sclcatCleanOutputDirectory [h|r|i|l|o] \<dir\>
# 
# @opt
# @optname h : Show usage help.
# @optname r : REMOVE uncompressed files left.
# @optname i : Compress input files (each result votable).
# @optname l : Compress log files (cmd, out and err files in \<dir\>/log/).
# @optname o : Compress output files (aggregation votable and fits files in \<dir\>/results/).
# @optname <dir> : the directory to process, either [dir-ref] or [dir-run-<YYYY-MM-DDTHH-MM-SS>].
# */

# Print usage 
function printUsage () {
    echo -e "Usage: sclcatCleanOutputDirectory [h|r|i|l|o|a] <dir>" 
    echo -e "\t-h\tPrint this help."
    echo -e "\t-r\tREMOVE uncompressed files left."
    echo -e "\t-i\tCompress input files only (each result votable)."
    echo -e "\t-l\tCompress log files only (cmd, out and err files in log/)."
    echo -e "\t-o\tCompress output files only (aggregation votable plus each fits)."
    echo -e "\t<dir>\tThe directory to process, either [dir-ref] or [dir-run-<YYYY-MM-DDTHH-MM-SS>]."
    exit 1;
}

REMOVE_FLAG="FALSE"

# Clean inputs
function cleanInputs () 
{
    echo -n "Cleaning input votables ... "
    tar czf votables.tgz *.vot
    if [ ${REMOVE_FLAG} == "TRUE" ]
    then
        rm -f *.vot
    fi
    echo "DONE."
}

# Clean logs
function cleanLogs () 
{
    echo -n "Cleaning logs files ... "
    tar czf logs.tgz log/
    if [ ${REMOVE_FLAG} == "TRUE" ]
    then
        rm -rf log/
    fi
    echo "DONE."
}

# Clean outputs
function cleanOutputs ()
{
    echo -n "Cleaning output votables and fits ... "
    tar czf result/results.tgz result/catalog*
    if [ ${REMOVE_FLAG} == "TRUE" ]
    then
        rm -f result/catalog*
    fi
    echo "DONE."
}

# Define temporary PATH # change it if the script becomes extern
PATH=$PATH:$PWD/../bin

# Parse command-line parameters
eval last=\${$#} # Get last 'command-line' parameter
givenRunDirectory=$last
if [ ! -d "${givenRunDirectory}" ]
then
    printUsage
fi

# Move to the given directory
cd ${givenRunDirectory}

# Parse command-line parameters
while getopts "hrilo" option
do
    case $option in
    h ) # Print usage help
        printUsage ;;
    r )
        REMOVE_FLAG="TRUE" ;;
    i )
        cleanInputs ;;
    l )
        cleanLogs ;;
    o )
        cleanOutputs ;;
    * ) # Unknown option default
        printUsage ;;
    esac
done

exit

#___oOo___
