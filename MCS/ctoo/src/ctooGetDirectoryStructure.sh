#! /bin/bash

#*******************************************************************************
# JMMC project
#
# "@(#) $Id: ctooGetDirectoryStructure.sh,v 1.4 2005-01-24 15:47:51 gluck Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
# lgluck    07/05/04    Created
#
#*******************************************************************************

#/**
# \file
# Interactive script to create/check standard module directory structure.
#
# \synopsis
# ctooGetDirectoryStructure
#
# \details
# Interactive utility used ot create or check MCS module directory structure.
# It asks the user to enter the module name to create.
# 
# \sa ctooGetTemplateForDirectoryStructure.sh, ctooGetTemplate.sh
# \sa MCS - Programming Standard
# 
# \n
#
# */


# signal trap (if any)


# Print out
echo "-------------------------------------------------------------------------"
echo "This utility allows to create or to check a module directory structure"


# get the module name to create
echo -e "\n-> Enter module name or press <Enter> to quit: \c"
read moduleName
if [ $moduleName != "" ]
then 
    ctooGetTemplateForDirectoryStructure $moduleName
fi


# ___oOo___
