#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id$"
#
# History
# -------
# $Log$
#*******************************************************************************

#/**
# @file
# Remove old standard JMMC headers in source files, and replace them with new one.
#
# @synopsis
# \<ctooCleanStandardHeader\> moddir
# 
# @details
# Old CVS-related headers are replaced by a new static one compliant with Subversion.
# 
# @usedfiles
# @filename LICENCE :  If any, add a licence to the new header.
#
# @todo Does not handle LICENCE yet.
# @todo Does not handle shell scripts yet.
# @todo Does not handle Makefiles yet.
#
# @sa http://www-laog.obs.ujf-grenoble.fr/twiki/bin/view/Jmmc/Software/Svn
# @sa http://stackoverflow.com/questions/277999/how-to-use-the-unix-find-command-to-find-all-the-cpp-and-h-files
# @sa http://stackoverflow.com/questions/151677/tool-for-adding-license-headers-to-source-files
# */

#*****************************************************************************

# Print usage 
function printUsage () {
    scriptName=`basename $0 .sh`
    echo -e "Usage: $scriptName module"
    exit 1;
}


# Test parameter validity
if [ ! -d $1 ]
then
    printUsage
fi

# Usage warning
echo -e "WARNING : This script overwrites source files."
echo -e "          Be sure to have backups at hand if anything goes wrong !!!"
echo
echo -e "Press enter to continue or ^C to abort."
read

# C/C++/Java files handling
C_FILES=`find "$1" -name \*.h -print -or -name \*.c -print -or -name \*.cpp -print -or -name \*.java -print`
for FILE in $C_FILES;
do
    echo -n "Removing $FILE header ... "
    TMP_FILE=`mktemp`

    # Search header beginning line number
    START_LINE=`grep -hn "^/\*\{70,\}$" $FILE | cut -d: -f1`
    START_LINE=`expr $START_LINE - 1`
    if [ $START_LINE -ne 0 ]
    then
        head --lines=$START_LINE $FILE > $TMP_FILE
    fi

    # Add new header file

    # Search header ending line number
    END_LINE=`grep -hn "^\ \*\{70,\}/$" $FILE | cut -d: -f1`
    END_LINE=`expr $END_LINE + 1`
    tail --lines=+$END_LINE $FILE >> $TMP_FILE

    mv -f $TMP_FILE $FILE
    echo "DONE."
done

# Shell Scripts/Makefile handling
C_FILES=`find "$1" -name \*.sh -print -or -name \Makefile -print`
for FILE in $C_FILES;
do
    echo -n "Removing $FILE header ... "
    TMP_FILE=`mktemp`

    # Search header beginning line number
    START_LINE=`grep -hn "^#\*\{70,\}$" $FILE | head -1 | cut -d: -f1`
    START_LINE=`expr $START_LINE - 1`
    if [ $START_LINE -ne "0" ]
    then
        head --lines=$START_LINE $FILE > $TMP_FILE
    fi

    # Add new header file

    # Search header ending line number
    END_LINE=`grep -hn "^#\*\{70,\}$" $FILE | tail -1 | cut -d: -f1`
    END_LINE=`expr $END_LINE + 1`
    tail --lines=+$END_LINE $FILE >> $TMP_FILE

    mv -f $TMP_FILE $FILE
    echo "DONE."
done

#___oOo___
