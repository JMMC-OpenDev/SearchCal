#!/bin/bash
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************

#/**
# @file
# This command check the given votable.
#
# @synopsis
# vobsCheckVOTable \<votableFilename\>
#
# @param votableFilename : filename of votable to be analysed
#
# @details
# It check conformity according ivoa xml schemas.
# 
# @usedfiles
# @filename VOTable-1.0.xsd :  xml schema for 1.0 version
# @filename VOTable-1.1.xsd :  xml schema for 1.1 version
# */

# signal trap (if any)

# one argument must be given
if [ $# != 1 ]
then
    echo "Usage: $0 votable.xml"
    exit 1
fi
votableFile=$1
echo "Checking VOTable '$(basename $votableFile)'"

# Get version of votable
VERSION=$(xml sel -t -v '//@version' $votableFile)
echo "Detected version '$VERSION'"

case $VERSION in
    1.0)
    XSD=vobsVOTable-1.0.xsd
    ;;
    1.1)
    XSD=vobsVOTable-1.1.xsd
    ;;
esac

SCHEMA=$(miscLocateFile $XSD)
if [ $? -ne 0 ]
then
    echo "ERROR: Can't find '$XSD'!" >&2
    exit 1
fi

echo "Using '$SCHEMA' schema for validation"

# proceed to real job
xmllint --noout --schema $SCHEMA $votableFile 

#___oOo___
