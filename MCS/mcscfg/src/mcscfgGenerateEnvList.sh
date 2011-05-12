#!/bin/bash
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************

#/**
# @file
# Generate a MCS env list from the given general xml description file.
#
# @synopsis
# mcscfgGenerateEnvList \<mcscfgEnvList.xml\> \<mcscfgEnvList\> 
#
# @usedfiles
# @filename mcscfgGenerateEnvList :  transformation rules
#
# 
# */



if [ $# -ne 2 ]
then
    echo "Usage: $0 <mcscfgEnvList.xml> <mcscfgEnvList>"
    exit 1
fi

GIVENHOSTNAME=$(hostname -s)
XMLMCSENVLIST=$1
MCSENVLIST=$2

# remove previous file
rm -f $MCSENVLIST

# Search xslt file
if [ -e ../config/mcscfgGenerateEnvList.xsl ]
then
    XSLTFILE="../config/mcscfgGenerateEnvList.xsl"
else
    echo "Can't find file 'mcscfgGenerateEnvList.xsl'"
    exit 1
fi

echo "Generating $MCSENVLIST for $GIVENHOSTNAME"

echo "#  This file has been automatically generated on $(date)" >> $MCSENVLIST
echo "#  for hostname '$GIVENHOSTNAME' by $0" >> $MCSENVLIST
echo "#  to change some entry in this file , please:"  >> $MCSENVLIST
echo "#   - go into the mcscfg module"  >> $MCSENVLIST
echo "#   - edit $XMLMCSENVLIST"  >> $MCSENVLIST
echo "#   - execute make all install"  >> $MCSENVLIST
echo "#" >> $MCSENVLIST
echo "# !!!!!!!!!!!  DO NOT MANUALLY EDIT THIS FILE  !!!!!!!!!!!" >> $MCSENVLIST
echo "#" >> $MCSENVLIST
echo "# Please use ${0##*/} with your modified configuration file " >> $MCSENVLIST
echo "#" >> $MCSENVLIST
echo "#" >> $MCSENVLIST
echo "#" >> $MCSENVLIST

xsltproc --stringparam hostname "$GIVENHOSTNAME" $XSLTFILE "$1" >> $MCSENVLIST

#___oOo___
