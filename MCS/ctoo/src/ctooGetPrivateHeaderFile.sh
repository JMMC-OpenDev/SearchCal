#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: ctooGetPrivateHeaderFile.sh,v 1.3 2004-09-14 08:59:58 gluck Exp $"
#
# who       when         what
# --------  -----------  -------------------------------------------------------
# gluck     06-Sep-2004  Created
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
# Create a private header file.
# 
# \synopsis
# \e ctooGetPrivateHeaderFile.sh
# 
# \n
# \details
# Create a private header file from a generated standard header file. The
# first part (from the beginning up to the first #endif) of the generated
# standard header file is left (copied). The intermediate part (from the first
# #endif up to the second #ifdef __cplusplus) is deleted and replace by a
# specific block concerning module id. The last part (from the second #ifdef
# __cplusplus up to the end of the file) is left (copied).
# 
# \n 
# \sa ctooGetTemplateForCoding, ctooGetCode, ctooGetTemplate
# 
# */


# Verify that current directory is an include directory (location for header
# files)
currentDirectory=`basename \`pwd\``
if [ $currentDirectory != "include" ]
then
    echo "ERROR - ctooGetPrivateHeaderFile: the current directory is not an"
    echo "                                  include directory"
    exit 1
fi


# Get module name
ROOT_NAME=`ctooGetModuleName`


#
# Get a "standard" .h header file
#

# Set private header file name
privateHeaderFilename=${ROOT_NAME}Private

# Generate an .h file template whom name is moduleNamePrivate.h
echo $privateHeaderFilename | ctooGetTemplateForCoding h-file > /dev/null


#
# Get file to build private header file
#

# Rename "standard" .h header file to be used as template for private header
# file
mv ${privateHeaderFilename}.h ${privateHeaderFilename}.template 

# Set private header template file
privateHeaderTemplateFile=${privateHeaderFilename}.template

# Set private header file
privateHeaderFile=${privateHeaderFilename}.h


#
# Copy first block
#

# get line numero of lines beginning by #endif
endifLineNo=(`grep -n "^#endif$" $privateHeaderTemplateFile | awk -F: '{print $1}'`)

# get line numero of the last line of the first block
lastLineBlock1LineNo=${endifLineNo[0]}

# Copy first block up to the above calculated line
head -$lastLineBlock1LineNo $privateHeaderTemplateFile > $privateHeaderFile


# Insert intermediate block
# Insert the following block :
#
#   /*
#    * Module name
#    */
#    #define MODULE_ID "$ROOT_NAME"
#
echo -e "\n" >> $privateHeaderFile
echo "/*" >> $privateHeaderFile
echo " * Module name" >> $privateHeaderFile
echo " */" >> $privateHeaderFile
echo "#define MODULE_ID \"$ROOT_NAME\"" >> $privateHeaderFile
echo -e "\n \n" >> $privateHeaderFile


#
# Copy last block
#

# get line numero of lines beginning by #ifdef __cplusplus
ifdefLineNo=(`grep -n "^#ifdef __cplusplus$" $privateHeaderTemplateFile | awk -F: '{print $1}'`)

# Element number of ifdefLineNo array
arrayEltNb=${#ifdefLineNo[*]}

# Index of last array element
lastEltIndex=$(($arrayEltNb - 1))

# get line numero of the first line of the last block
firstLineLastBlockLineNo=${ifdefLineNo[$lastEltIndex]}

# Get file line number
fileLineNumber=(`wc -l $privateHeaderTemplateFile`)

# Calculate line number to copy for the last block
lastBlockLineNumber=$(($fileLineNumber - $firstLineLastBlockLineNo + 1))

# Copy last block from the above calculated line
tail -$lastBlockLineNumber $privateHeaderTemplateFile >> $privateHeaderFile


# Delete temporary private header template file
rm -f $privateHeaderTemplateFile

# Exit with success
exit 0

#___oOo___
