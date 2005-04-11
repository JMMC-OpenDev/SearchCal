#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: ctooGetSpecificHeaderFile.sh,v 1.7 2005-04-11 07:52:50 gluck Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.6  2005/02/22 15:30:52  gluck
# Added comment block
#
# Revision 1.5  2005/02/21 15:47:02  gluck
# Minor correction: comment
#
# Revision 1.4  2005/01/24 15:47:51  gluck
# Bug correction for log message automatic insertion ($Log: not supported by cvs2svn $
# Bug correction for log message automatic insertion (Revision 1.6  2005/02/22 15:30:52  gluck
# Bug correction for log message automatic insertion (Added comment block
# Bug correction for log message automatic insertion (
# Bug correction for log message automatic insertion (Revision 1.5  2005/02/21 15:47:02  gluck
# Bug correction for log message automatic insertion (Minor correction: comment
# Bug correction for log message automatic insertion ()
#
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
# Create a specific header file from a generated standard header file. The first
# part (from the beginning up to the first " */") of the generated standard
# header file is left (copied). The intermediate part (from the first " */" up
# to the first "#endif /*!") is deleted and replace by a specific block, which
# depends on header file type. The last part (from the first "#endif /*!" up to
# the end of the file) is left (copied).
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

# Generate an .h file template whom name is moduleName[Private].h
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

# get line numero of first block (fb) lines with stop pattern ( */)
fbStopPatternLineNo=(`grep -n "^ \*/$" $headerTemplateFile | awk -F: '{print $1}'`)

# get line numero of the last line of the first block
fbLastLineNo=${fbStopPatternLineNo[0]}

# Copy first block up to the above calculated line
head -$fbLastLineNo $headerTemplateFile > $specificHeaderFile


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
                 #    * Constants definition
                 #    */
                 #    
                 #    /* Module name */
                 #    #define MODULE_ID "$MOD_NAME"
                 #
                 echo -e "\n" >> $specificHeaderFile
                 echo "/*" >> $specificHeaderFile
                 echo " * Constants definition" >> $specificHeaderFile
                 echo " */" >> $specificHeaderFile
                 echo -e "" >> $specificHeaderFile
                 echo "/* Module name */" >> $specificHeaderFile
                 echo "#define MODULE_ID \"$MOD_NAME\"" >> $specificHeaderFile
                 echo -e "\n \n" >> $specificHeaderFile
                 ;;
esac


#
# Copy last block
#

# get line numero of last block (lb) lines with stop pattern (#endif /*!)
lbStopPatternLineNo=(`grep -n "^#endif /\*!" $headerTemplateFile | awk -F: '{print $1}'`)

# Element number of lbStopPatternLineNo array
arrayEltNb=${#lbStopPatternLineNo[*]}

# Index of last array element
lastEltIndex=$(($arrayEltNb - 1))

# get line numero of the first line of the last block
firstLineLbLineNo=${lbStopPatternLineNo[$lastEltIndex]}

# Get file line number
fileLineNumber=(`wc -l $headerTemplateFile`)

# Calculate line number to copy for the last block
lbLineNumber=$(($fileLineNumber - $firstLineLbLineNo + 1))

# Copy last block from the above calculated line
tail -$lbLineNumber $headerTemplateFile >> $specificHeaderFile


# Delete temporary private header template file
rm -f $headerTemplateFile

# Exit with success
exit 0

#___oOo___
