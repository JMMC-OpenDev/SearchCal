#! /bin/sh
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: mkfMakeGeneratedFilesDependencies.sh,v 1.1 2004-12-09 17:34:54 gzins Exp $" 
# who       when         what
# --------  --------     ----------------------------------------------
# gzins     09-Dec-2004  Created
#

#************************************************************************
#   NAME
#   mkfMakeGeneratedFilesDependencies - create the makefile to build the
#   generated files 
# 
#   SYNOPSIS
#
#   mkfMakeGeneratedFilesDependencies <cdfList>
# 
#   DESCRIPTION
#   Utility used by mkfMakefile to create the makefile to build the generated
#   files; i.e. C header file containing error definitions and C++ header and
#   class class to handle command defined in CDF.
#   It is not intended to be used as a standalone command.
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

# if Linux: disable the bash builtin command 'echo'.
if [ "`uname`" = "Linux" ]
then
    enable -n echo
fi

# output the make-rules:
target="do_gen:"

# Rule to generate error header file
if [ "`which errXmlToH 2>/dev/null`" != "" ]
then
    XML2H=`which errXmlToH`
else
    if [ -f ./errXmlToH ]
    then
        XML2H="./errXmlToH"
    fi
fi

list=""
if [ -d ../errors -a  "`ls ../errors/*Errors.xml 2>/dev/null`" != "" ]
then
    if [ "$XML2H" == "" ]
    then
        echo "ERROR: mkfMakeErrorFileDependencies no errXmlToH in PATH" >&2
        exit 1
    fi

    if [ "`ls ../errors/*Errors.xml 2>/dev/null`" != "" ]
    then 
        for file in `ls ../errors/*Errors.xml 2>/dev/null`
        do
            if [ -s $file ]
            then 
                # Get the base file mane
                name=`basename $file .xml`
                list="$list $name"
            fi
        done

        for name in $list
        do
            XML_FILE="../errors/${name}.xml"
            H_FILE="../include/${name}.h"

            target="$target $H_FILE"

            echo "$H_FILE : $XML_FILE"
            echo "	@echo \"== Generating error include file: $H_FILE\""
            echo "	-\$(AT) \$(RM) $H_FILE"
            echo "	-\$(AT) sh $XML2H $XML_FILE $H_FILE >/dev/null"
        done

    fi
fi 

# Rules to generated header and class file for each define command 
cdfList=$1
if [ "${cdfList}" != "" ]
then
    # For each CDF, add rule for C++ class file
    for member in ${cdfList}
    do
        echo "../src/${member}_CMD.cpp: ../config/${member}.cdf"
        echo "	-@echo == Generating C++ class: ${member}_CMD.cpp and ${member}_CMD.h"
        echo "	@\$(AT)cmdCdfToCppClass ../config/${member}.cdf>/dev/null"
        target="$target ../src/${member}_CMD.cpp"
    done
    # And rule for header file
    for member in ${cdfList}
    do
        echo "../include/${member}_CMD.h: ../config/${member}.cdf"
        echo "	-@echo == Generating C++ class: ${member}_CMD.h and ${member}_CMD.h"
        echo "	@\$(AT)cmdCdfToCppClass ../config/${member}.cdf>/dev/null"
        target="$target ../include/${member}_CMD.h"
    done

    list="$list $cdfList"
fi

# Check if files have to be generated 
if [ "$list" = "" ]
then 
    echo "do_gen:"
    echo "	-\$(AT)echo \"\""        
else
    echo "$target"
fi

#
# ___oOo___

