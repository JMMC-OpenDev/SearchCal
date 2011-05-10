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
# @todo Does not handle LICENCE yet.
# @todo Does not handle shell scripts yet.
# @todo Does not handle Makefiles yet.
#
# @sa http://www-laog.obs.ujf-grenoble.fr/twiki/bin/view/Jmmc/Software/Svn
# @sa http://stackoverflow.com/questions/277999/how-to-use-the-unix-find-command-to-find-all-the-cpp-and-h-files
# @sa http://stackoverflow.com/questions/151677/tool-for-adding-license-headers-to-source-files
# */

# Print usage
function printUsage () {
    scriptName=`basename $0 .sh`
    echo -e "Usage: $scriptName module"
    exit 1;
}

# Cut $1 file content between $2 and $3 line numbers, and add $4 header instead
function fileCut () {
    printf "Removing header from %-50s ... " "'$FILE'"

    if [ $# -ne 4 ]
    then
        echo "SKIPPED (header not found)."
        return
    fi

    TMP_FILE=`mktemp`

    # Keep stuff above previous header
    START_LINE=`expr $2 - 1`
    if [ $START_LINE -ne 0 ]
    then
        head --lines=$START_LINE $FILE > $TMP_FILE
    fi

    # Add new header
    cat $4 >> $TMP_FILE

    # Keep below previous header
    END_LINE=`expr $3 + 1`
    tail --lines=+$END_LINE $FILE >> $TMP_FILE

    mv -f $TMP_FILE $1

    echo "DONE (lines $2 to $3)."

    return;
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

# Find Templates directory
if [ -d ../templates/forCoding ]
then
    TEMPLATES=../templates
elif [ -d $INTROOT/templates/forCoding ]
then
    TEMPLATES=$INTROOT/templates
else
    TEMPLATES=$MCSROOT/templates
fi

# C/C++/Java/module.doc files handling
NEW_HEADER=$TEMPLATES/forCoding/svn-header.template
FILELIST=`find "$1" -name \*.h -print -or -name \*.c -print -or -name \*.cpp -print -or -name \*.java -print -or -name \*.doc -print`
for FILE in $FILELIST;
do
    # Find '/***...***'
    START_LINE=`grep -hn "^/\*\{70,\}$" $FILE | cut -d: -f1`
    # Find '[ |*]***...**/'
    END_LINE=`grep -hn "^[ ,\*]\*\{70,\}/$" $FILE | cut -d: -f1`

    fileCut $FILE $START_LINE $END_LINE $NEW_HEADER
done

# Shell Scripts/Makefile/Config handling
NEW_HEADER=$TEMPLATES/forMakefile/svn-header.template
FILELIST=`find "$1" -name \*.sh -print -or -name \Makefile -print -or -name \*.cfg -print`
for FILE in $FILELIST;
do
    # Find '#***...***'
    START_LINE=`grep -hn "^#\*\{70,\}$" $FILE | head -1 | cut -d: -f1`
    # Find '#***...**[*|/]'
    END_LINE=`grep -hn "^#\*\{70,\}[*,/]$" $FILE | tail -1 | cut -d: -f1`

    fileCut $FILE $START_LINE $END_LINE $NEW_HEADER
done

# XML handling
NEW_HEADER=$TEMPLATES/forDocumentation/svn-header.template
FILELIST=`find "$1" -name \*.xml -print -or -name \*.cdf -print`
for FILE in $FILELIST;
do
    # Find first '***...***'
    START_LINE=`grep -hn "^\*\{70,\}$" $FILE | head -1 | cut -d: -f1`
    # Find second '***...***'
    END_LINE=`grep -hn "^\*\{70,\}$" $FILE | tail -1 | cut -d: -f1`

    # If no closing '***...***' found (CDF case)
    if [ -n "$START_LINE" ]
    then
        if [ "$START_LINE" -eq "$END_LINE" ]
        then
            # Use last line before first comment close tag
            END_LINE=`grep -hn "^\-\->" $FILE | tail -1 | cut -d: -f1`
            END_LINE=`expr $END_LINE - 1`
        fi
    fi

    fileCut $FILE $START_LINE $END_LINE $NEW_HEADER
done

#___oOo___
