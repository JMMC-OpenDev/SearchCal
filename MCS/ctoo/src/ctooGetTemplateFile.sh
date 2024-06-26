#!/bin/bash
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************

#/**
# \file
# Create a customized template file.
# 
# \synopsis
# ctooGetTemplateFile \e \<template\> \e \<file\>
# 
# \param template : original template filename (eventually with its path) 
#                   from which the new customized template file is generated.
# \param file : customized generated template filename (eventually with its 
#               path).
# 
# \n
# \details
# Create a customized template file from an original template file. If no
# paths are specified for template and file parameters, the script will look
# for the template file and will create the new template file in the directory
# where this script is executed. If paths are specified, script will use them
# to get and create files, respectively.
# 
# \usedfiles
# \filename template : cf description of template parameter above.
# 
# \n 
# \sa ctooGetTemplateForCoding.sh, ctooGetTemplateForDirectoryStructure.sh
# 
# \n 
# 
# */


# signal trap (if any)


# Input parameters given should be 3 and in the correct format:
if [ $# != 2 ]
then 
    echo -e "\n\tUsage: ctooGetTemplateFile"
    echo -e "<template> <file>\n"
    exit 1
fi

# Get input parameters

# Template file to get, with its path
TEMPLATE=$1

# New file to create from the template
FILE=$2

# test if the template file exists
if [ ! -f $TEMPLATE ]
then
    # the template file doesn't exist
    echo -e "ERROR : the template $TEMPLATE file doesn't exist"
fi


# Copy the template file in current directory.
# The template file is copied in a new temporary file deleting the header of the
# template
if grep -v "#%#" $TEMPLATE > ${FILE}.BAK
then
    # File copy succeeds

    # Test INSTITUTE validity
    if [ "$INSTITUTE" = "" ]
    then
        INSTITUTE="JMMC"
    fi
    # Replacement in the new temporary file, to include from CVS, id and message
    # log automatic insertion. Then create the permanent file.
    sed -e "1,$ s/<INSTITUTE>/$INSTITUTE/g" \
        -e "1,$ s/I>-<d/Id/g" \
        -e "1,$ s/>-Log-</Log/g" \
        ${FILE}.BAK > $FILE

    # Remove the temporary backup file
    rm -f ${FILE}.BAK

else
    # File copy failed
    echo -e "\n>>> CANNOT CREATE --> $FILE\n"
fi


#___oOo___
