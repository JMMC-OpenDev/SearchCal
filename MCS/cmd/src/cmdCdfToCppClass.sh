#!/bin/bash

#
# This script generates a Cpp Class associated to the given command 
# description file.
#
#

if [ $# != 2 ]
then
    echo "Usage: $0 commandDefinition.cdf moduleName"
    exit 1
fi

MODULENAME=$2
XSLFILE=`miscLocateFile cmdCdfToCppClass.xsl`
echo "Transforming $1 with $XSLFILE"

xsltproc --stringparam "moduleName" $MODULENAME $XSLFILE $1
