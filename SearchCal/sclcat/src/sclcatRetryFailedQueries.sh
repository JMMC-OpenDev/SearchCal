#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclcatRetryFailedQueries.sh,v 1.2 2008-07-25 12:56:33 lafrasse Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.1  2008/07/23 22:34:10  lafrasse
# Creation.
#
#*******************************************************************************

#/**
# @file
# Try to relaunch queries that returned errors while processed
#
# @synopsis
# sclcatRetryFailedQueries \<run-directory\>
# 
# @details
# This script search for log/*.err files whith size not null, and then restart
# the correponsding command.
#
# @opt
# @optname h : show usage help
# @optname n : compute the number of failed queries
# @optname l : list all failed filenames (without extension)
# @optname e : report warnings (problem during queries) and errors (failed queries)
# */

# Print usage 
function printUsage () {
    echo -e "Usage: sclcatRetryFailedQueries [dir-ref] or [dir-run-<YYYY-MM-DDTHH-MM-SS>]" 
    echo -e "\t-h\t\tPrint this help."
    echo -e "\t-n\t\tCompute the number of failed queries."
    echo -e "\t-l\t\tList all failed filenames (without extension)."
    echo -e "\t-e\t\tReport warnings (problem during queries) and errors (failed queries)."
    echo -e "\t<dir-run>\tRetry failed queries in dir-run directory"
    exit 1;
}

# Compute the number of failed commands
function computeNbOfFailedCommands () {
    # Get the number of .err files in <dir>/log with size not null
    totalNbOfQueries=`ls -l log/ | grep ".err" | grep -v "      0 " | wc | awk '{print $1}'`
    echo "${totalNbOfQueries} failed queries found." ;
}

# List all failed filenames (without extension)
function listFailedFilenames () {
    # For each .err files in <dir>/log with size not null
    errFilesList=`ls -l log/ | grep ".err" | grep -v "      0 " | awk '{print $9}'`
    for errFilename in $errFilesList
    do
        # Remove '.err' extension of the current .err filename
        filename="${errFilename%.err}"

        # Display the filename
        echo ${filename}
    done
}

# Report found warnings (.err file not empty but .vot exist) and errors (.err file not empty and .vot does not exist)
function reportWarningsAndErrors () {
    warningCounter=0
    errorCounter=0

    # For each .err files in <dir>/log with size not null
    errFilesList=`ls -l log/ | grep ".err" | grep -v "      0 " | awk '{print $9}'`
    for errFilename in $errFilesList
    do
        # Remove '.err' extension of the current .err filename
        filename="${errFilename%.err}"

        # If the corresponding VOTable file does not exist
        votFilename="${filename}.vot"
        if [ ! -f $votFilename ]; then
            # The query failed, so report it as an error
            let "errorCounter += 1"
            echo "ERROR: ${filename} failed."
        else
            # The query did not complete succesfully, so report it as a warning
            let "warningCounter += 1"
            echo "WARNING: ${filename} did not complete succesfully."
        fi
    done

    # Generate general error and warning report
    everyThingIsOK=1
    if [ $warningCounter -gt 0 ]; then
        echo "Found ${warningCounter} warnings."
        everyThingIsOK=0
    fi
    if [ $errorCounter -gt 0 ]; then
        echo "Found ${errorCounter} errors."
        everyThingIsOK=0
    fi
    if [ $everyThingIsOK -eq 1 ]; then
        echo "No errors nor warnings found."
        everyThingIsOK=0
    fi
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
while getopts "hnle" option
do
    case $option in
    h ) # Print usage help
        printUsage ;;
    n ) # Compute the number of failed queries
        computeNbOfFailedCommands ;
        exit ;;
    l ) # List all failed filenames (without extension)
        listFailedFilenames ;
        exit ;;
    e ) # Report warnings (problem during queries) and errors (failed queries)
        reportWarningsAndErrors ;
        exit ;;
    * ) # Unknown option default
        printUsage ;;
    esac
done

echo -n "Started at: "
date

# Compute today's date
today=`date +%Y-%m-%dT%H-%M-%S`

# Searching for failed queries
echo -n "Searching for failed queries ... "
# Get the name of each .err files in <dir>/log with size not null
errFilesList=`ls -l log/ | grep ".err" | grep -v "      0 " | awk '{print $9}'`
# Get the number of .err files in <dir>/log with size not null
totalNbOfQueries=`ls -l log/ | grep ".err" | grep -v "      0 " | wc | awk '{print $1}'`
echo "DONE (found '${totalNbOfQueries}' to retry)."

# For each .err files in <dir>/log with size not null
nbOfRetryDone=0
for errFilename in $errFilesList
do
    # Increment counter
    let "nbOfRetryDone += 1"

    # Remove '.err' extension of the current .err filename
    filename="${errFilename%.err}"

    # Compute corresponding '.cmd' filename
    cmdFilename="${filename}.cmd"
    echo "Retrying '${cmdFilename}' (${nbOfRetryDone} / ${totalNbOfQueries}) ..."

    # Display the previous size of the VOTable (if any) before retry
    votFilename="${filename}.vot"
    if [ -f $votFilename ]; then
        votFileSize=`du -sk ${votFilename} | awk '{print $1}'`
        echo "Previous VOTable size was ${votFileSize}KB."
    else
        echo "Previous VOTable was not created."
    fi

    # Compute temporary output '.out' filename
    tmpOutFilename="log/${filename}-${today}.out"
    outFilename="log/${filename}.out"

    # Compute temporary err '.err' filename
    tmpErrFilename="log/${filename}-${today}.err"
    errFilename="log/${filename}.err"

    # Retry the command
    command=`cat log/${cmdFilename}`
    command="${command} 1>${tmpOutFilename} 2>${tmpErrFilename}"
    echo $command
    eval $command

    # Only overwrite previous files after command completion.
    # If the script is interrupted while retrying a comand (i.e ctrl-c),
    # previous files are not overwritten nor zeroed.
    mv -f ${tmpOutFilename} ${outFilename}
    mv -f ${tmpErrFilename} ${errFilename}

    # Get the new size of the VOTable (if any) after retry
    if [ -f $votFilename ]; then
        votFileSize=`du -sk ${votFilename} | awk '{print $1}'`
        echo "New VOTable size is ${votFileSize}KB."
    else
        echo "New VOTable was not created."
    fi

    echo "...DONE"
    echo
done

echo -n "Finished at: "
date

#___oOo___
