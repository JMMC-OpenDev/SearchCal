#! /bin/bash
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************
#   NAME
#   mkfMakePythonModule - create a python module
# 
#   SYNOPSIS
#
#   mkfMakePythonModule <modName>
# 
#   DESCRIPTION
#   Utility used by mkfMakefile to copy a script to ../bin.
#   If the script is a /bin/sh script, then an instruction
#   to disable the build-in echo command under Linux is added 
#   to the script.
#
#   <modName>     The name of the module. The output is named 
#                 ../lib/python/site-packages/<modName>.py
#
#   FILES
#   $VLTROOT/include/mkfMakefile   
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

if [ $# -ne 1 ]
then
    echo "" >&2
    echo "mkfMakePythonModule <modName>" >&2
    echo "" >&2
    exit 1
fi

#
# set up more readable variables:
modName=$1

OUTPUT=../lib/python/site-packages/
if [ ! -e $OUTPUT ] 
then
    mkdir -p $OUTPUT
    chmod 755 $OUTPUT
elif [ ! -d $OUTPUT ]
then
    echo "$OUTPUT is not a directory!!"
    exit 1
fi

cp ${modName}.py $OUTPUT

#
# ___oOo___
