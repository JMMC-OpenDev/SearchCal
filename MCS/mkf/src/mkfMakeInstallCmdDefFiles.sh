#! /bin/sh
# JMMC project
#
# "@(#) $Id: mkfMakeInstallCmdDefFiles.sh,v 1.1 2004-12-03 18:14:21 gzins Exp $" 
#
# who       when         what
# --------  --------     ----------------------------------------------
# gzins     03-Dec-2004  Adapted from VLT

#************************************************************************
#   NAME
#   mkfMakeInstallCmdDefFiles - copy the CDF files into target area.
# 
#   SYNOPSIS
#
#   vltMakeInstallTableFiles <CDF_DIR> <protectionMask>
#
# 
#   DESCRIPTION
#   Utility used by mkfMakefile to copy the Command Definition Files into
#   target area.
#   It is not intended to be used as a standalone command.
#
#    file are copied into:
#
#           ../config/*.cdf     --->   <CDF_DIR>
#
#   <CDF_DIR>   where to copy ".cdf" files
#
#   <protectionMask>  how to set the protection of created file
#
#   FILES
#   $MCSROOT/include/mkfMakefile   
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
#----------------------------------------------------------------------

if [ $# != 2 ]
then
    echo "" >&2
    echo " ERROR: mkfMakeInstallCmdDefFiles: $*" >&2
    echo " Usage: mkfMakeInstallCmdDefFiles <CDF_DIR> <protectionMask>" >&2
    echo "" >&2
    exit 1
fi

#
# check correctness of destination directory
#
CDF_DIR=$1
if [ "${CDF_DIR}" != ""  -a  ! -d $CDF_DIR ]
then 
    echo "" >&2
    echo " ERROR: mkfMakeInstallCmdDefFiles: " >&2
    echo "          Internal error: >>$CDF_DIR<< not a valid directory " >&2
    echo "" >&2
    exit 1
fi

#
# get protection mask
#
MASK=$2


#
# set initial (empty) makefile target. If any, tables will be added to it.
#
target="tables: "

#
# Command definition files
#
if [ -d ../config  -a  "`ls ../config/*.cdf 2>/dev/null`" != "" ]
then 
    target="$target CDFs_begin "

    echo -e "CDFs_begin:"
    echo -e "\t-@echo \"\"; echo \"....CDF files:\""

    for file in `ls ../config/*.cdf 2>/dev/null`
    do
        FILE=`basename $file`
        echo -e "\t-\$(AT)touch ../config/$FILE"
        echo -e "$CDF_DIR/$FILE: ../config/$FILE"
        echo -e "\t-\$(AT)echo \"\t$FILE\";\\"
        echo -e "\t      cp ../config/$FILE  $CDF_DIR/$FILE; \\"
        echo -e "\t      chmod $MASK  $CDF_DIR/$FILE"
        target="$target $CDF_DIR/$FILE"
    done
fi

#
# output the complete target
#
echo -e $target
echo -e "\t-@echo \"\""

exit 0

#
# ___oOo___
