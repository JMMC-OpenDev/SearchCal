#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: cmdIsCdfValid.sh,v 1.1 2004-11-21 18:30:25 mella Exp $"
#
# who       when         what
# --------  -----------  -------------------------------------------------------
# mella     21-Nov-2004  Created
#
#
#*******************************************************************************

#/**
# \file
# Check your cdf file.
# schema rules.
#
# \synopsis
# cmdIsCdfValid \e \<cdfFile\> 
#
# \param cdfFile : your command definition file
#
# \n
# \details
# Use this script to check your command definition file according the cdf
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

# get the cdf schema
XSD=`miscLocateFile cmdDefinitionFile.xsd` 

# put informations
echo $0 is Verifying your Command Definition File [$1]
echo Using Command Definition File Schema [$XSD]

# check cdf file
xmllint --schema $XSD  $1

#___oOo___
