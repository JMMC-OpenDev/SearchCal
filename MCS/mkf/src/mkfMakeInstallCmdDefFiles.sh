#! /bin/sh
# JMMC project
#
# "@(#) $Id: mkfMakeInstallCmdDefFiles.sh,v 1.2 2004-12-10 07:04:04 gzins Exp $" 
#
# who       when         what
# --------  --------     ----------------------------------------------
# gzins     03-Dec-2004  Adapted from VLT
# gzins     10-Dec-2004  Added list of files to be installed

#************************************************************************
#   NAME
#   mkfMakeInstallCmdDefFiles - copy the CDF files into target area.
# 
#   SYNOPSIS
#
#   mkfMakeInstallCmdDefFiles <cdfList> <CDF_DIR> <protectionMask>
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
#   <cdfList>         CDF files to be copied
#   <CDF_DIR>         where to copy ".cdf" files
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

if [ $# != 3 ]
then
    echo "" >&2
    echo " ERROR: mkfMakeInstallCmdDefFiles: $*" >&2
    echo " Usage: mkfMakeInstallCmdDefFiles <cdfList> <CDF_DIR> <protectionMask>" >&2
    echo "" >&2
    exit 1
fi

cdfList=${1}
#
# check correctness of destination directory
#
CDF_DIR=${2}
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
MASK=${3}


#
# set initial (empty) makefile target. If any, tables will be added to it.
#
target="tables: "

#
# Command definition files
#
if [ "$cdfList" != "" ]
then 
    target="$target CDFs_begin "

    echo -e "CDFs_begin:"
    echo -e "\t-@echo \"\"; echo \"....CDF files:\""

    for file in $cdfList
    do
        FILE="`basename $file`.cdf"
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
