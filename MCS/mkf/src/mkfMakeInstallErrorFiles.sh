#! /bin/sh
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: mkfMakeInstallErrorFiles.sh,v 1.3 2004-09-30 08:14:50 gzins Exp $" 
#
# who       when         what
# --------  --------     ----------------------------------------------
# gzins     26-Aug-2004  Adapted from VLT 

#************************************************************************
#   NAME
#   mkfMakeInstallErrorFiles - copy the Error files into target area.
# 
#   SYNOPSIS
#
#   mkfMakeInstallErrorFiles <ERRORS> <protectionMask>
#
# 
#   DESCRIPTION
#   Utility used by mkfMakefile to generate the mkfMakefile.install section
#   in charge to copy the Error files into target area.
#   It is not intended to be used as a standalone command.
#
#    treated files are:
#
#           ../include/*Errors.h    --->   <ERRORS>/../include
#           ../errors/*Errors.xml   --->   <ERRORS>
#
#   <ERRORS>   root directory for copying Error files
#
#   <protectionMask>  how to set the protection of created file
#
#
#   FILES
#   $MCSROOT/include/mkfMakefile   
#
#   ENVIRONMENT
#
#   RETURN VALUES
#
#   SEE ALSO 
#   mkfMakefile
#
#   BUGS    
#
#----------------------------------------------------------------------
if [ $# != 2 ]
then
    echo "" >&2
    echo " ERROR:  mkfMakeInstallErrorFiles: $*" >&2
    echo " Usage:  mkfMakeInstallErrorFiles <ERRORS> <protectionMask>" >&2
    echo "" >&2
    exit 1
fi

ERRORS=$1
MASK=$2


if [ ! -d $ERRORS ]
then 
    echo "" >&2
    echo " ERROR: mkfMakeInstallErrorFiles: " >&2
    echo "          Internal error: >>$ERRORS<< not a valid directory " >&2
    echo "" >&2
    exit 1
fi

#
# according to the file currently under ERRORS, if any, produce
# the needed targets:
if [ -d ../errors  -a  "`ls ../errors/*Errors.xml 2>/dev/null`" != "" ]
then 
    
    target="errors: errors_begin "

    echo -e "errors_begin:"
    echo -e "\t-@echo \"\"; echo \"..ERROR files:\""

    for file in `ls ../errors/*Errors.xml 2>/dev/null`
    do
        FILE=`basename $file`
        echo -e "$ERRORS/$FILE: ../errors/$FILE"
        echo -e "\t-\$(AT)cp ../errors/$FILE  $ERRORS/$FILE; \\"  
        echo -e "\t      chmod $MASK $ERRORS/$FILE"
        target="$target $ERRORS/$FILE"
    done

    for file in `ls ../include/*Errors.h 2>/dev/null`
    do
        FILE=`basename $file`
        echo -e "$ERRORS/../include/$FILE: ../include/$FILE"
        echo -e "\t-\$(AT)cp ../include/$FILE $ERRORS/../include/$FILE; \\" 
        echo -e "\t      chmod $MASK $ERRORS/../include/$FILE"
        target="$target $ERRORS/../include/$FILE"
    done

    for file in `ls ../errors/help 2>/dev/null`
    do
        FILE=`basename $file`
        if [ $FILE != CVS ] 
        then
            echo -e "$ERRORS/help/$FILE: ../errors/help/$FILE"
            echo -e "\t-\$(AT)cp ../errors/help/$FILE $ERRORS/help/$FILE; \\"
            echo -e "\t      chmod $MASK $ERRORS/help/$FILE"
            target="$target $ERRORS/help/$FILE"
        fi
    done
    echo -e "$target"
else
    echo -e "errors:"
    echo -e "\t-@echo \"\""
fi

exit 0
#
# ___oOo___

