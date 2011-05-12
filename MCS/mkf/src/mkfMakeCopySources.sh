#! /bin/sh
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************
#   NAME
#   mkfMakeCopySources - copy the module source files into integration area.
# 
#   SYNOPSIS
#
#   mkfMakeCopySources
#
# 
#   DESCRIPTION
#   Utility used by mkfMakefile to copy the module source files into 
#   integration area, when INTROOT is defined.
#   It is not intended to be used as a standalone command.
#
#   According to the current parent directory, files are copied into:
#
#    <mod>
#           <mod>/src           --->    INTROOT/Sources/<mod>/src   
#           <mod>/test          --->    INTROOT/Sources/<mod>/test
#           <mod>/include       --->    INTROOT/Sources/<mod>/include
#
#   The reason why to copy sorce file into integration area is to have
#   the exact files that have been used for generating the software
#   that has been installed. This for debugging purposes.
#
#   FILES
#   $MCSROOT/include/mkfMakefile   
#
#   ENVIRONMENT
#
#     INTROOT    current integration area root directory
#
#   RETURN VALUES
#
#   SEE ALSO 
#   mkfMakefile, Makefile, (GNU) make
#
#   BUGS    
#

# Integration area is not defined
if [ "$INTROOT" = "" ]
then
    # Use MCS area
    WHERE_TO_COPY=$MCSROOT
else
    WHERE_TO_COPY=$INTROOT
fi

# Check destination directory
if [ ! -d $WHERE_TO_COPY ]
then
    echo ""
    echo " ERROR: mkfMakeCopySources: destination directory defined as >> $WHERE_TO_COPY << is not a directory"
    echo ""
    exit 1
fi

#
# get current directory. It should have this form: ...../<mod>/src)
PWD=`pwd`

#
# where am I?
SUB_DIR=`basename $PWD`

# 
# check if this is a standard structure.
if [ $SUB_DIR != "src" -a $SUB_DIR != "test" ]
then
    echo ""
    echo " ERROR: mkfMakeCopySources: "
    echo "          Standard Makefile can be either src/ or in test/ "
    echo "          Where are you now?"
    echo ""
    exit 1
fi

#
# find module name. There are two cases:
#    - normal modules: the module name is the parent directory
#    - multiplatform modules: the module is structured as more than one 
#                     submodules. Each submodule has the normal module
#                     directory tree but fixed names are used:
#                        <mod>/ws     for the ws  part
#                        <mod>/lcu    for the LCU part
#                        <mod>/ace    for the ACE part
#                        <mod>/dsp    for the DSP part
PARENT_DIR=`dirname $PWD`
MODULE_NAME=`basename $PARENT_DIR`

#
# If Sources directory does not exists, create it
for dir in Sources $MODULE_NAME 
do
    WHERE_TO_COPY=$WHERE_TO_COPY/$dir
    if [ ! -d $WHERE_TO_COPY ]
    then
        if mkdir $WHERE_TO_COPY
        then
            continue
        else
            echo ""
            echo " ERROR: mkfMakeCopySources: cannot access/create $WHERE_TO_COPY"
            echo ""
            exit 1
        fi
    fi
done

echo ""
echo " Copying current files "

for dir in $SUB_DIR include
do
    echo "        from: $PARENT_DIR/$dir"
    #
    # because some modules may not follow the standard directory
    # structure, let's check that the directory exists
    if [ -d $PARENT_DIR/$dir ]
    then
        # if any, remove files currently stored in the integration area
        if [ -d $WHERE_TO_COPY/$dir ]
        then
            rm -rf $WHERE_TO_COPY/$dir
        fi
        mkdir $WHERE_TO_COPY/$dir

        echo "          to: $WHERE_TO_COPY/$dir"
        #
        # copy current files into integration area 
        cp -r $PARENT_DIR/$dir $WHERE_TO_COPY

        # set the files group writable (so they can be overwritten 
        # by another team member during integration activity)
        chmod -R 777 $WHERE_TO_COPY/$dir
    else
        echo "                . . . does not exists. Skipped"
        echo "                      (probably this module is not standard)"
    fi
done

echo "                                               . . . done "
echo ""

#
# ___oOo___
