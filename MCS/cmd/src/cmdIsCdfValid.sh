#!/bin/bash
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************

#/**
# \file
# Check your cdf file according the cdf schema rules.
#
# \synopsis
# cmdIsCdfValid \e \<cdfFile\> 
#
# \param cdfFile : your command definition file
#
# \n
# \details
# Use this script to check your command definition file according the cdf schema
# and checking that filename is valid according modulename and mnemonic
# 
# \usedfiles
# OPTIONAL. If files are used, for each one, name, and usage description.
# \filename cmdCommandDefinition.xsd :  xml schema for cdf
#
# \n
# \warning This script requires xmllint.
#
# \n
# \ex
# \n This command check the VALID associated cdf. If it succeed, xmllint tell
# you that the file validates. Else it returns your error.
# \code
# cmdIsCdfValid ../config/VALID.cdf
# \endcode
#
# \sa cmdCdfToCppClass 
# 
# \n 
# 
# */

# one argument must be given
if [ $# != 1 ]
then
    echo "Usage: $0 commandDefinitionFile.cdf"
    exit 1
fi




# Check CDF file name
cdfFile=`basename $1`
modName=`ctooGetModuleName`
# Get command name
lineContainingCommandName=`grep "^ *<mnemonic>.*</mnemonic> *$" $1`
rightSideOfCommandName=${lineContainingCommandName##*<mnemonic>}
cmdName=${rightSideOfCommandName%%</mnemonic>*}

if [ "${modName}${cmdName}.cdf" != "$cdfFile" ]
then
    echo "ERROR: Invalid CDF file name '$cdfFile'!" >&2
    echo "MUST be <modName><cmdName>.cdf; i.e. '${modName}${cmdName}.cdf'" >&2
    echo "" >&2
    exit 1
fi


# get the cdf schema
XSD=`miscLocateFile cmdDefinitionFile.xsd` 

# put informations
echo $0 is Verifying your Command Definition File [$1]
echo Using Command Definition File Schema [$XSD]

# check cdf file
xmllint --schema $XSD  $1

#___oOo___
