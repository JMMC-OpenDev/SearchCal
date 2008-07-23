#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclcatRetryFailedQueries.sh,v 1.1 2008-07-23 22:34:10 lafrasse Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
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
# @optname n : show the number of failed queries
# @optname l : show the failed '.err' filenames
# @optname v : show the failed '.vot' filenames
# */

# Print usage 
function printUsage () {
    echo -e "Usage: sclcatRetryFailedQueries [dir-ref] or [dir-run-<YYYY-MM-DDTHH-MM-SS>]" 
    echo -e "\t-h\t\tprint this help."
    echo -e "\t-n\t\tshow the number of failed queries."
    echo -e "\t-l\t\tshow the failed '.err' filenames."
    echo -e "\t-v\t\tshow the failed '.vot' filenames."
    echo -e "\t<dir-run>\tRetry failed queries in dir-run directory"
    exit 1;
}

# Diplay number of failed commands
function showNbOfFailedCommands () {
    totalNbOfQueries=`ls -l log/ | grep ".err" | grep -v "      0 " | wc | awk '{print $1}'`
    echo "${totalNbOfQueries} failed queries found." ;
}

# Diplay commands that failed 
function showFailedCommands () {
    ls -l log/ | grep ".err" | grep -v "      0 " | awk '{print $9}'
}

# Diplay VOTable that failed 
function showFailedVOTables () {
    errFilesList=`ls -l log/ | grep ".err" | grep -v "      0 " | awk '{print $9}'`
    for errFilename in $errFilesList
    do
        # Remove '.err' extension of the current .err filename
        filename="${errFilename%.err}"

        # Display the VOTable filname
        votFilename="${filename}.vot"
        echo ${votFilename}
    done
}

# Define temporary PATH # change it if the script becomes extern
PATH=$PATH:$PWD/../bin

# Parse command-line parameters
eval last=\${$#} # Get last parameter
givenRunDirectory=$last
if [ ! -d "${givenRunDirectory}" ]
then
    printUsage
fi

# Move to the given directory
cd ${givenRunDirectory}

# Parse command-line parameters
while getopts "hnlv" option
do
    case $option in
    h ) # Help option
        printUsage ;;
    n ) # Show number of failed commands
        showNbOfFailedCommands ;
        exit ;;
    l ) # Show list of failed commands
        showFailedCommands ;
        exit ;;
    v ) # Show list of failed VOTables
        showFailedVOTables ;
        exit ;;
    * ) # Unknown option
        printUsage ;;
    esac
done

echo -n "Started at: "
date

# Compute today's date
today=`date +%Y-%m-%dT%H-%M-%S`

# Searching for failed queries
echo -n "Searching for failed queries ... "
errFilesList=`ls -l log/ | grep ".err" | grep -v "      0 " | awk '{print $9}'`
totalNbOfQueries=`ls -l log/ | grep ".err" | grep -v "      0 " | wc | awk '{print $1}'`
echo "DONE (found '${totalNbOfQueries}' to retry)."

# Loop on every calibrators of every stars, then build calibrator file
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
