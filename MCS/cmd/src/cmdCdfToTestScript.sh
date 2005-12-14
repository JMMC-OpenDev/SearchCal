#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: cmdCdfToTestScript.sh,v 1.1 2005-12-14 12:46:46 mella Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
#
#*******************************************************************************

#/**
# \file
# Generates a dedicated test config file from given cdf file.
#
# \synopsis
# cmdCdfToTestScript commandDefinitionFile 
#
# \param commandDefinitionFile : your command definition file.
#
# \details
# This script generates a general config file that can be used to process loop
# over commands usgin scripts. It uses the description of the given command 
# description file.
# 
# \warning This script requires xsltproc
#
# \sa cmdIsCdfValid
# 
# */

if [ $# != 1 ]
then
    echo "Usage: $0 <commandDefinition.cdf>"
    exit 1
fi

xslFileName="cmdCdfToTestScript.xsl"
xslFile=`miscLocateFile $xslFileName`

# Check if xslt file exist
if [ -z "$xslFile" ]
then
    echo "ERROR: No $xslFileName stylesheet file found to convert cdf into cpp" >&2
    echo "" >&2
    exit 1
fi

# Check CDF file 
TST=$(cmdIsCdfValid $1)
if [ $? -ne 0 ]
then
    echo "ERROR: Invalid CDF file name '$cdfFile'!" >&2
    exit 1
fi

modName=`ctooGetModuleName`
cdfFile="$1"

##### Generate C++ class files 
echo "Transforming $cdfFile with $xslFile"
xsltproc --stringparam "moduleName" $modName --stringparam "cdfFilename" $cdfFile $xslFile $1

#___oOo___
