#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: cmdCdfToCppClass.sh,v 1.2 2004-11-21 19:59:02 mella Exp $"
#
# who       when         what
# --------  -----------  -------------------------------------------------------
# mella     21-Nov-2004  Created
#
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

if [ $# != 2 ]
then
    echo "Usage: $0 commandDefinition.cdf moduleName"
    exit 1
fi

MODULENAME=$2
XSLFILE=`miscLocateFile cmdCdfToCppClass.xsl`
echo "Transforming $1 with $XSLFILE"

xsltproc --stringparam "moduleName" $MODULENAME $XSLFILE $1

#___oOo___
