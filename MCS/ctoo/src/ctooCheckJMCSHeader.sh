#!/bin/bash
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************

#/**
# @file
# Check jMCS header validity.
#
# @synopsis
# \<ctooCheckJMCSHeader\> moddir
#
# @sa http://www-laog.obs.ujf-grenoble.fr/twiki/bin/view/Jmmc/Software/Svn
# @sa http://stackoverflow.com/questions/277999/how-to-use-the-unix-find-command-to-find-all-the-cpp-and-h-files
# @sa http://stackoverflow.com/questions/151677/tool-for-adding-license-headers-to-source-files
# */

# Print script usage.
function printUsage () {
    scriptName=`basename $0 .sh`
    echo -e "Usage: $scriptName module"
    exit 1;
}

# Check $1 file header between $2 and $3 lines against $4 header CRC.
function fileCheck () {
    # If any function args is missing
    if [ $# -ne 4 ]
    then
        echo "ERROR : missing header in file '$1'."
        return 1
    fi

    if [ ! -f $1 ]
    then
        echo "ERROR : '$1' file not found."
        return 2
    fi

    TMP_FILE=`mktemp`
    sed -n $2,$3p $1 > $TMP_FILE # Extract file header
    echo "$4  $TMP_FILE" | md5sum -c --status # Check extraction CRC
    RESULT=$?
    rm -f $TMP_FILE

    if [ $RESULT -ne 0 ]
    then
        echo "ERROR : bad header in file '$1' (lines $2 to $3)."
        #meld $NEW_HEADER $1
        return 3
    fi

    return $RESULT
}

# Test CLI parameter validity.
if [ ! -d $1 ]
then
    printUsage
fi

# Find Templates directory.
if [ -d ../templates/forCoding ]
then
    TEMPLATES=../templates
elif [ -d $INTROOT/templates/forCoding ]
then
    TEMPLATES=$INTROOT/templates
else
    TEMPLATES=$MCSROOT/templates
fi

NAME="jMCS-BSD-header.template" # for BSD license in jMCS module
#NAME="AppLauncher-GPLv3-header.template" # for GPLv3 license in AppLauncher modules
#echo "Templates taken from '$TEMPLATES' directory, with name '$NAME'."

# C/C++/Java/module.doc files handling.
NEW_HEADER=$TEMPLATES/forCoding/$NAME
HEADER_CRC=`md5sum $NEW_HEADER | cut -f1 -d' '`
FILELIST=`find "$1" -name \*.h -print -or -name \*.c -print -or -name \*.cpp -print -or -name \*.java -print -or -name \*.doc -print`
for FILE in $FILELIST;
do
    # Find first '/***...***'
    START_LINE=`grep -hn "^/\*\{70,\}$" $FILE | cut -d: -f1`

    # Find second ' ***...**/' or '***...**/'
    END_LINE=`grep -hn "^[ ,\*]\*\{70,\}/$" $FILE | cut -d: -f1`

    fileCheck $FILE $START_LINE $END_LINE $HEADER_CRC
done

# Shell Scripts/Python/Makefile/Config handling.
NEW_HEADER=$TEMPLATES/forMakefile/$NAME
if [ -e $NEW_HEADER ]
then
    HEADER_CRC=`md5sum $NEW_HEADER | cut -f1 -d' '`
    FILELIST=`find "$1" -name \*.sh -print -or -name \*.py -print -or -name \Makefile -print -or -name \*.cfg -print`
    for FILE in $FILELIST;
    do
        # Find first '###...###'
        START_LINE=`grep -hn "^#\{70,\}$" $FILE | head -1 | cut -d: -f1`

        # Find second '###...###'
        END_LINE=`tail -n +4 $FILE | grep -hn "^#\{70,\}$" | tail -1 | cut -d: -f1`
        END_LINE=`expr $END_LINE + 3`

        fileCheck $FILE $START_LINE $END_LINE $HEADER_CRC
    done
fi

# XML/XSL/XSD/CDF handling.
NEW_HEADER=$TEMPLATES/forDocumentation/$NAME
HEADER_CRC=`md5sum $NEW_HEADER | cut -f1 -d' '`
FILELIST=`find "$1" -name \*.xml -print -or -name \*.xsd -or -name \*.xsl -print -or -name \*.cdf -print`
for FILE in $FILELIST;
do
    # Find first '***...***'
    START_LINE=`grep -hn "^\*\{70,\}$" $FILE | head -1 | cut -d: -f1`

    # Find second '***...***'
    END_LINE=`grep -hn "^\*\{70,\}$" $FILE | tail -1 | cut -d: -f1`

    # If no closing '***...***' found (CDF file case)
    if [ -n "$START_LINE" ]
    then
        if [ "$START_LINE" -eq "$END_LINE" ]
        then
            # Use last line before first comment close tag
            END_LINE=`grep -hn "^\-\->" $FILE | tail -1 | cut -d: -f1`
            END_LINE=`expr $END_LINE - 1`
        fi
    fi

    fileCheck $FILE $START_LINE $END_LINE $HEADER_CRC
done

# Everything went fine !
echo "DONE"
exit 0

#___oOo___
