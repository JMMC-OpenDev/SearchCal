#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: ctooGetTemplateFile.sh,v 1.2 2004-08-11 10:00:22 gluck Exp $"
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
# \e ctooGetTemplateFile \e \<template\> \e <file> \e [remove]
# 
# \param template : original template filename (eventually with its path) 
#                   from which the new customized template file is generated.
# \param file : customized generated template filename (eventually with its 
#               path).
# 
# \n
# \opt
# \optname remove : if used, the temporary backup file will be removed. If not
#                   used, the temporary backup file will be kept, which will 
#                   allow to continue the treatment in the calling script,
#                   without generating a new temporary backup file. But this
#                   temporary backup file will have to be removed in the
#                   calling script.
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
# OPTIONAL. If files are used, for each one, name, and usage description.
# \filename template : cf description of template parameter above.
# 
# \n 
# \sa ctooGetTemplateForCoding, ctooGetCode, ctooGetTemplate
# 
# */


# Input parameters given should be 3 and in the correct format:
if [ $# != 2 -a $# != 3 ]
then 
    echo -e "\n\tUsage: ctooGetTemplateFile"
    echo -e "<template> <file> [remove]\n"
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

    case $backupFile in
        "")
            # Keep the temporary backup file => do nothing
            ;;
        remove)
            # Remove the temporary backup file
            rm -f ${FILE}.BAK
            ;;
        *)
            # Wrong parameter value
            echo "ERROR third option is not a valid."
            echo -e "\n\tUsage: ctooGetTemplateFile"
            echo -e "<template> <file> [remove]\n"
            exit 1
            ;;
    esac
            
else
    # File copy failed
    echo -e "\n>>> CANNOT CREATE --> $FILE\n"
fi

#___oOo___
