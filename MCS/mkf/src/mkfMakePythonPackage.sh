#! /bin/bash
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************
#   NAME
#   mkfMakePythonPackage - create a python module
# 
#   SYNOPSIS
#
#   mkfMakePythonPackage <modName> <objList>
# 
#   DESCRIPTION
#   Utility used by mkfMakefile.all to copy a python scripts in 
#   package into the corresponding <module>lib/python/site-package/<package>
#   directory
#
#   <modName>     The name of the module. The output is named 
#                 ../lib/python/site-packages/<modName>
#
#   <objList>     the list of python scripts in the Package
#
#   FILES
#   $VLTROOT/include/mkfMakefile.all   
#
#   ENVIRONMENT
#
#   RETURN VALUES
#
#   SEE ALSO 
#   mkfMakefile, Makefile, (GNU) make
#
#   BUGS    
#

if [ $# -ne 2 ]
then
    echo "" >&2
    echo "mkfMakePythonPackage <modName> <objList>" >&2
    echo "" >&2
    exit 1
fi

#
# set up more readable variables:
packName=$1
objList=$2

OUTPUT=../lib/python/site-packages/${packName}
if [ ! -e $OUTPUT ] 
then
    mkdir -p $OUTPUT
    chmod 755 $OUTPUT
elif [ ! -d $OUTPUT ]
then
    echo "$OUTPUT is not a directory!!"
    exit 1
fi
# dirty trick of making a cd to avoid a for loop
cd $packName
cp -f $objList  ../$OUTPUT

#
# ___oOo___
