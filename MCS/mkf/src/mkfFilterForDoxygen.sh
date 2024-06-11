#!/bin/bash
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************
# \file
# File filter for doxygen.
#
# \synopsis
# \b mkfFilterForDoxygen \e \<file\>
#
# \param file : file to be filtered out
#
# \n
# \details
# The different functionality of this filter allow to :
#
# -# convert files not extractable by doxygen, ie non C or C++ like files into
# doxygen extractable files, transforming them into C or C++ like files
# (comments, code). It gets the header comment block and convert it to the
# doxygen comment block format. Then it copies the filename to rely the
# comment block to a C like code. \n
# File type filtered out : 
#   - shell scripts .sh
#   .
# \n
# -# unescape comments into doxygen documentation blocks. In case user wants to
# include C-style comments in a doxygen code examples block (~ nested comment,
# are not possible), C-style comment pattern pair should be escaped by /#
# and #/ pattern pair.
# 
# \usedfiles
# \filename $FILE : input file feeding doxygen
#

# File to filter out
FILE=$1

# Get file extension
fileExtension=${FILE##*.}

case $fileExtension in
    sh | tcl)
        awk -v file=$FILENAME '
        BEGIN {
            # Initialise the flag to print the line
            printLine = "no"
        }
        
        # For #/** lines = first line of the comment block
        /^#\/\*\*/ {
            # Set print line flag to yes, because the first line to print has
            # been detected, and printing following lines is needed
            printLine = "yes"
            # Print #/** line deleting the # character => /** which is the
            # beginning of a doxygen comment block
            print substr($0, 2, 3)
            # Go to the next input line, because the treatment has been done,
            # and to prevent further treatment by other action block 
            next
        }

        # For # */ lines = last line of the comment block
        /^#\ \*\// {
            # Set print line flag to no, because the last line to print has
            # been detected, and printing following lines is not needed
            printLine = "no"
            # Print # */ line deleting the # character => */ which is the
            # end of a doxygen comment block
            print substr($0, 2, 3)
            # Go to the next input line, because the treatment has been done,
            # and to prevent further treatment by other action block 
            next
        }
        
        # For # */ lines = last line of the comment block
        # Skip proc declaration  
        /^##\ / {
            # Set the line, deleting the ## characters
            line = substr($0, 4)
            # Print the new line adding in front of each one " *"
            print line
            # Go to the next input line, because the treatment has been done,
            # and to prevent further treatment by other action block 
            next
        }
        # For 'proc' lines = procedure declaration
        /^proc\ / {
            # Skip this line which must be precede by a comment line containing
            # the the C-like syntax
            # Go to the next input line, because the treatment has been done,
            # and to prevent further treatment by other action block 
            next
        }
        {
            # If a line has to be printed (inside the comment block)
            if (printLine == "yes")
            {
                # Set the line, deleting the # character
                line = substr($0, 2)
                # Print the new line adding in front of each one " *"
                print " *"line
            }
            else
            {
                # The line has not to be print
                # Comment the line (it is used to have the source code in the
                # documentation)
                print ""$0
            }
        }
        ' $FILE
        ;;
    *)
        # Convert /# to /* and #/ to */, to allow comment inside doxygen
        # comment block, and particularly between \code and \endcode markers,
        # to allow comments for code examples
        sed -e "1,$ s/\/#/\/*/g" $FILE | sed -e "1,$ s/#\//*\//g"
        ;;
esac
    

#
# ___oOo___
