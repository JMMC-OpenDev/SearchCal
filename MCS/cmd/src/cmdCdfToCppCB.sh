#!/bin/bash
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************

#/**
# \file
# Generates a dedicated command callback class.
#
# \synopsis
# cmdCdfToCppCB commandDefinitionFile
#
# \param commandDefinitionFile : your command definition file.
#
# \n
# \details
# This script generates a Cpp Class for callback squeleton associated to the 
# given command description file.
# 
# \n
# \warning This script requires xsltproc
#
# \n
# \ex
# \n Generate CB file for a command class (dummyValidCB.cpp). 
# \code
# cmdCdfToCppCB ../config/VALID.cdf
# \endcode
# 
#
# \sa cmdIsCdfValid
# 
# \n 
# 
# */

if [ $# != 1 ]
then
    echo "Usage: $0 <commandDefinition.cdf>"
    exit 1
fi

modName=`ctooGetModuleName`
xslFileName="cmdCdfToCppCB.xsl"
xslFile=`miscLocateFile $xslFileName`
cdfFile=`basename $1`

##### Check CDF file name ; must be <modName><cmdName>.cdf

# Get command name
lineContainingCommandName=`grep "^ *<mnemonic>.*</mnemonic> *$" $1`
rightSideOfCommandName=${lineContainingCommandName##*<mnemonic>}
cmdName=${rightSideOfCommandName%%</mnemonic>*}

# Check CDF file name
if [ "${modName}${cmdName}.cdf" != "$cdfFile" ]
then
    echo "ERROR: Invalid CDF file name '$cdfFile'!" >&2
    echo "MUST be <modName><cmdName>.cdf; i.e. '${modName}${cmdName}.cdf'" >&2
    echo "" >&2
    exit 1
fi

# Check if xslt file exist
if [ -z "$xslFile" ]
then
    echo "ERROR: No $xslFileName stylesheet file found to convert cdf into cpp" >&2
    echo "" >&2
    exit 1
fi

##### Generate C++ class files 
echo "Transforming $1 with $xslFile"
xsltproc --stringparam "moduleName" $modName --stringparam "cdfFilename" $cdfFile $xslFile $1

#___oOo___
