#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: ctooGetSpecificHeaderFile.sh,v 1.4 2005-01-24 15:47:51 gluck Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
# gluck     17-Dec-2004  Created
# gzins     04-Jan-2005  Changed ROOT_NAME to MOD_NAME 
#
#
#*******************************************************************************

#/**
# \file
# Create a specific header file.
# 
# \synopsis
# ctooGetSpecificHeaderFile.sh \e \<headerFileType\>
# 
# \n
# \details
# Create a specific header file from a generated standard header file. The
# first part (from the beginning up to the first #endif) of the generated
# standard header file is left (copied). The intermediate part (from the first
# #endif up to the second #ifdef __cplusplus) is deleted and replace by a
# specific block, which depends on header file type. The last part (from the
# second #ifdef __cplusplus up to the end of the file) is left (copied).
# 
# \n 
# \sa ctooGetTemplateForCoding, ctooGetCode, ctooGetTemplate
# 
# */


# signal trap (if any)


# Input parameters given should be 1 and in the correct format:
if [ $# != 1 ]
then 
    echo -e "\n\tUsage: ctooGetSpecificHeaderFile module|private \n"
    exit 1
fi 

# Get the input parameters
headerFileType=$1

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
MOD_NAME=`ctooGetModuleName`
if [ $? != 0 ]
then
    exit 1
fi

#
# Get a "standard" .h header file
#

# Set private header file name
case $headerFileType in
    module) headerFilename=${MOD_NAME};;
    private) headerFilename=${MOD_NAME}Private;;
    *) echo "ERROR parameter is not a valid."
       echo -e "\n\tUsage: ctooGetSpecificHeaderFile module|private \n";;
esac

# Generate an .h file template whom name is moduleNamePrivate.h
echo $headerFilename | ctooGetTemplateForCoding h-file > /dev/null


#
# Get file to build specific header file
#

# Rename created "standard" .h header file to be used as template for specific
# header file
mv ${headerFilename}.h ${headerFilename}.template 

# Set specific header template file
headerTemplateFile=${headerFilename}.template

# Set private header file
specificHeaderFile=${headerFilename}.h


#
# Copy first block
#

# get line numero of lines beginning by #endif
endifLineNo=(`grep -n "^#endif$" $headerTemplateFile | awk -F: '{print $1}'`)

# get line numero of the last line of the first block
lastLineBlock1LineNo=${endifLineNo[0]}

# Copy first block up to the above calculated line
head -$lastLineBlock1LineNo $headerTemplateFile > $specificHeaderFile


# Insert intermediate specific block
case $headerFileType in
        module) # Insert the following block :
                #
                #   /*
                #    * Local headers
                #    */
                #
                echo -e "\n" >> $specificHeaderFile
                echo "/*" >> $specificHeaderFile
                echo " * Local headers" >> $specificHeaderFile
                echo " */" >> $specificHeaderFile
                echo -e "\n \n" >> $specificHeaderFile
                ;;
        private) # Insert the following block :
                 #
                 #   /*
                 #    * Module name
                 #    */
                 #    #define MODULE_ID "$MOD_NAME"
                 #
                 echo -e "\n" >> $specificHeaderFile
                 echo "/*" >> $specificHeaderFile
                 echo " * Module name" >> $specificHeaderFile
                 echo " */" >> $specificHeaderFile
                 echo "#define MODULE_ID \"$MOD_NAME\"" >> $specificHeaderFile
                 echo -e "\n \n" >> $specificHeaderFile
                 ;;
esac


#
# Copy last block
#

# get line numero of lines beginning by #ifdef __cplusplus
ifdefLineNo=(`grep -n "^#ifdef __cplusplus$" $headerTemplateFile | awk -F: '{print $1}'`)

# Element number of ifdefLineNo array
arrayEltNb=${#ifdefLineNo[*]}

# Index of last array element
lastEltIndex=$(($arrayEltNb - 1))

# get line numero of the first line of the last block
firstLineLastBlockLineNo=${ifdefLineNo[$lastEltIndex]}

# Get file line number
fileLineNumber=(`wc -l $headerTemplateFile`)

# Calculate line number to copy for the last block
lastBlockLineNumber=$(($fileLineNumber - $firstLineLastBlockLineNo + 1))

# Copy last block from the above calculated line
tail -$lastBlockLineNumber $headerTemplateFile >> $specificHeaderFile


# Delete temporary private header template file
rm -f $headerTemplateFile

# Exit with success
exit 0

#___oOo___
