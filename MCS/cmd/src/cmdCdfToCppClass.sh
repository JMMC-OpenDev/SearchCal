#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: cmdCdfToCppClass.sh,v 1.6 2005-01-31 07:48:03 mella Exp $"
#
# who       when         what
# --------  -----------  -------------------------------------------------------
# mella     21-Nov-2004  Created
# gzins     09-Dec-2004  Removed module name argument
# gzins     10-Dec-2004  Checked CDF name
#
#*******************************************************************************

#/**
# \file
# Generates a dedicated command class.
#
# \synopsis
# cmdCdfToCppClass commandDefinitionFile moduleName 
#
# \param commandDefinitionFile : your command definition file.
# \param moduleName : the name of the module to prefix for code generation.
#
# \n
# \details
# This script generates a Cpp Class associated to the given command 
# description file.
# 
# \n
# \warning This script requires xsltproc
#
# \n
# \ex
# \n Generate h and C file for a command class (dummyVALID_CMD.C and
# dummyVALID_CMD.h). 
# \code
# cmdCdfToCppClass ../config/VALID.cdf dummy
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
xslFileName="cmdCdfToCppClass.xsl"
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
if [ -z $xslFile ]
then
    echo "ERROR: No $xslFileName stylesheet file found to convert cdf into cpp" >&2
    echo "" >&2
    exit 1
fi

##### Generate C++ class files 
echo "Transforming $1 with $xslFile"
xsltproc --stringparam "moduleName" $modName --stringparam "cdfFilename" $cdfFile $xslFile $1

#___oOo___
