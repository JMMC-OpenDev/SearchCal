#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: cmdCdfToCppClass.sh,v 1.4 2004-12-09 17:53:36 gzins Exp $"
#
# who       when         what
# --------  -----------  -------------------------------------------------------
# mella     21-Nov-2004  Created
# gzins     09-Dec-2004  Removed module name argument
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

MODULENAME=`ctooGetModuleName`
XSLFILE=`miscLocateFile cmdCdfToCppClass.xsl`
CDFFILE=`basename $1`

echo "Transforming $1 with $XSLFILE"

xsltproc --stringparam "moduleName" $MODULENAME --stringparam "cdfFilename" $CDFFILE $XSLFILE $1

#___oOo___
