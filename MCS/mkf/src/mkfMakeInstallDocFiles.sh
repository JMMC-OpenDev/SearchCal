#! /bin/sh
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: mkfMakeInstallDocFiles.sh,v 1.2 2005-02-15 08:40:15 gzins Exp $" 
#
# History
# -------
# $Log: not supported by cvs2svn $
# gzins     26-Aug-2004  Adapted from VLT 
#
#************************************************************************
#   NAME
#   mkfMakeInstallDoc - copy the documentation files into target area.
# 
#   SYNOPSIS
#
#   mkfMakeInstallDoc <DOCROOT> <protectionMask>
#
# 
#   DESCRIPTION
#   Utility used by mkfMakefile to generate the mkfMakefile.install section
#   in charge to copy the documentation files into target area.
#   It is not intended to be used as a standalone command.
#
#   According to the current parent directory, files are copied into:
#
#     <mod>
#           <mod>/doc           --->    <DOCROOT>/<mod>/   
#
#
#     <DOCROOT>         root directory for copying documentation files
#
#     <protectionMask>  how to set the protection of created file
#
#   FILES
#     $MCSROOT/include/mkfMakefile   
#
#   ENVIRONMENT
#
#   RETURN VALUES
#
#   SEE ALSO 
#     mkfMakefile, Makefile, (GNU) make
#
#   BUGS    
#
#----------------------------------------------------------------------

if [ $# != 2 ]
then
    echo "" >&2
    echo " ERROR:  mkfMakeInstallDocFiles: $*" >&2
    echo " Usage:  mkfMakeInstallDocFiles <DOCROOT> <protectionMask>" >&2
    echo "" >&2
    exit 1
fi

DOCROOT=$1
MASK=$2

if [ ! -d $DOCROOT ]
then 
    echo "" >&2
    echo " ERROR: mkfMakeInstallDocFiles: " >&2
    echo "          Internal error: >>$DOCROOT<< not a valid directory " >&2
    echo "" >&2
    exit 1
fi

#
# get current directory. It should have this form: ...../<mod>/src)
PWD=`pwd`
#
# where am I?
SUB_DIR=`basename $PWD`
# 
# check if this is a standard directory structure.
if [ $SUB_DIR != "src" -a $SUB_DIR != "test" ]
then
    echo ""
    echo " ERROR: mkfMakeInstallDocFiles: "
    echo "          Makefile can be either src/ or in test/ "
    echo "          Where are you now?"
    echo ""
    exit 1
fi

#
# find module name. There are two cases:
PARENT_DIR=`dirname $PWD`
MODULE_NAME=`basename $PARENT_DIR`

WHERE_TO_COPY=$DOCROOT/$MODULE_NAME
# if any, remove files currently stored in the target area
if [ -d $WHERE_TO_COPY ]
then
    if rm -rf $WHERE_TO_COPY
    then
        continue
    else
        echo ""
        echo " ERROR: mkfMakeInstallDocFiles: cannot remove $WHERE_TO_COPY"
        echo ""
        exit 1
    fi

fi
if mkdir $WHERE_TO_COPY
then
    continue
else
    echo ""
    echo " ERROR: mkfMakeInstallDocFiles: cannot access/create $WHERE_TO_COPY"
    echo ""
    exit 1
fi

#
# according to the file currently under doc, if any, produce
# the needed targets:
if [ -d ../doc  -a  "`ls ../doc/api 2>/dev/null`" != "" ]
then 
    echo ""
    echo " Copying documentation files "

    echo "        from: $PARENT_DIR/doc"


    echo "          to: $WHERE_TO_COPY"
    #
    # copy current files into target area 
    cp -r $PARENT_DIR/doc/* $WHERE_TO_COPY

    # set the files group writable (so they can be overwritten 
    # by another team member during integration activity)
    chmod -R $MASK $WHERE_TO_COPY

    echo ""
    mkfBuildDocIndex $WHERE_TO_COPY/.. > /dev/null

    echo "                                               . . . done "
    echo ""

fi
#
# ___oOo___
