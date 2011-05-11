#!/bin/bash
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************

#/**
# \file
# Get module name.
#
# \synopsis
# ctooGetModuleName 
#
# \details
# Get module name, extracting it from a file, normaly existing in each module
# directory  structure.
# 
# \usedfiles
# \filename moduleDescription.xml : file used to extract module name, located
# in the doc subdirectory of a module directory structure
#
# \n
# \warning This script should be executed from one of the first level
# subdirectory of a module structure.
#
# \n
# \sa ctooGetPrivateHeaderFile.sh, ctooGetTemplateForCoding.sh
# 
# \n
#
# */


# signal trap (if any)


# test doc directory existence
dir=../doc
if [ ! -d $dir ]
then
    echo "ERROR - ctooGetmoduleName: $dir directory does not exist." >&2
    echo "        please check your module directory structure" >&2
    exit 1
fi

# test moduledescription.xml file existence
file=../doc/moduleDescription.xml
if [ ! -f $file ]
then
    echo "ERROR - ctooGetmoduleName: $file file does not exist." >&2
    echo "        please check your module directory structure"  >&2
    exit 1
fi
    
# get line containing the name of the module in moduledescription.xml file
# => <module name="modulename">
lineContainingModuleName=`grep "^<module name=\".*\">$" ../doc/moduleDescription.xml`

# trim left the above extracted line => modulename">
rightSideOfModuleName=${lineContainingModuleName##<module name=\"}

# trim right the above extracted string to get module name => modulename
ROOT_NAME=${rightSideOfModuleName%%\">}

# If no module name can be found => error
if [ "$ROOT_NAME" = "" ]
then
    echo "ERROR - ctooGetModuleName: Could not get module name." >&2
    exit 1
else
    # Print out module name
    echo $ROOT_NAME
    exit 0
fi

#___ooo___
