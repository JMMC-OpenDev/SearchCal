#! /bin/sh
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: mkfMakeXslPath.sh,v 1.1 2007-11-15 08:09:04 mella Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
#
#************************************************************************

#/**
# @file
# Write on standard output then path to search xsl files into.
#
# @synopsis
#
# mkfMakeXslPath 
# 
#
# @details
# Utility used by scripts and mkfMakefile to dynamically generate the
# path for xsl prior to transformation.
# It should be not intended to be used as a standalone command,
# and it must be executed from the module's src directory. We are waiting 
# the final decision
# 
#----------------------------------------------------------------------

# signal trap (if any)

#
# compute the value of path, taking
# $INTROOT, $MCSROOT and $MODROOT into account
# 
XSLTPATH="../config"
for rootDir in $INTROOT $MCSROOT
do
    if [ -d "$rootDir" ]
    then
        XSLTPATH+=":$rootDir/config"
    fi
done
echo $XSLTPATH
#
# ___oOo___
