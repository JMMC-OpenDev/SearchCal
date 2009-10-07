#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: vobsCopyVotParamsAsFields.sh,v 1.2 2009-10-07 09:18:28 mella Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.1  2009/09/16 11:10:52  mella
# add vobsCopyVotParamsAsFields.sh script
#
# Revision 1.1  2006/01/05 14:26:48  mella
# First revision
#
#*******************************************************************************

#/**
# @file
# This command output one copy of the given votable with one new column per
# param.
#
# @synopsis
# vobsCopyVotParamsAsFields \<votableFilename\>
#
# @param votableFilename : filename of votable to be read and modified
#
# @details
# The column values will be the param one.
# 
# @usedfiles
# @filename ../config/vobsCopyVotParamsAsFields.xsl :  xslt file
#
# */

# signal trap (if any)

# one argument must be given
if [ $# != 1 ]
then
    echo "Usage: $0 votable.xml"
    exit 1
fi
votableFile="$1"

XSL=vobsCopyVotParamsAsFields.xsl

XSLT=$(miscLocateFile $XSL)
if [ $? -ne 0 ]
then
    echo "ERROR: Can't find '$XSL'!" >&2
    exit 1
fi


# proceed to real job
xsltproc $XSLT "$votableFile"

#___oOo___
