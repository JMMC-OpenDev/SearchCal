#!/bin/bash
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************

#/**
# \file
# Generates an java interface for the given cdf file.
#
# \synopsis
# cmdCdfToJavaInterface
#
# \param commandDefinitionFile : your command definition file or a set of
# several one.
#
# \n
# \details
# This script generates a Java Interface associated to the 
# given command description file.
# 
# \n
# \warning This script requires xsltproc
#
# \n
# \ex
# \n Generate an interface file . 
# \code
# cmdCdfToJavaInterface ../config/VALID.cdf
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

xslFileName="cmdCdfToJavaInterface.xsl"
xslFile=`miscLocateFile $xslFileName`
cdfFile=`basename $1`
cd $(dirname $1)
modName=`ctooGetModuleName`
cd -

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

##### Generate output files 
output=${cmdName}Interface.java
echo "Transforming $1 with $xslFile into $output"
xsltproc --stringparam "moduleName" $modName --stringparam "outputFilename" $output --stringparam "cdfFilename" $cdfFile $xslFile $1

#___oOo___
