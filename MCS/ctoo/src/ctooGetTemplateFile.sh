#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: ctooGetTemplateFile.sh,v 1.5 2004-09-13 06:09:23 gluck Exp $"
#
# who       when         what
# --------  -----------  -------------------------------------------------------
# gluck     10-Aug-2004  Created
#
#
#*******************************************************************************
#   NAME
# 
#   SYNOPSIS
# 
#   DESCRIPTION
#
#   FILES
#
#   ENVIRONMENT
#
#   RETURN VALUES
#
#   CAUTIONS
#
#   EXAMPLES
#
#   SEE ALSO
#
#   BUGS     
#
#-------------------------------------------------------------------------------
#

# signal trap (if any)

#/**
# \file
# Create a customized template file.
# 
# \synopsis
# \e ctooGetTemplateFile \e \<template\> \e \<file\>
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
# \sa ctooGetTemplateForCoding, ctooGetCode, ctooGetTemplate
# 
# */


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

# option to say if the generated temporary file should be removed or not
backupFile=$3


# Copy the template file in current directory.
# The template file is copied in a new temporary file deleting the header of the
# template
if grep -v "#%#" $TEMPLATE > ${FILE}.BAK
then
    # File copy succeeds
    # setup author and date:
    AUTHOR=`whoami`
    AUTHOR=`printf "%-8s" $AUTHOR`
    DATE=`date "+%d-%b-%Y"`

    # Replace author and date in the new temporary file and create the
    # permanent file
    sed -e "1,$ s/NNNNNNNN/$AUTHOR/g" \
        -e "1,$ s/dd-mmm-yyyy/$DATE/g" \
        -e "1,$ s/I>-<d/\Id/g" \
        ${FILE}.BAK > $FILE

    # Remove the temporary backup file
    rm -f ${FILE}.BAK

else
    # File copy failed
    echo -e "\n>>> CANNOT CREATE --> $FILE\n"
fi

#___oOo___
