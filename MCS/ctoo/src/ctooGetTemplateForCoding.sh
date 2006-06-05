#! /bin/bash

#*******************************************************************************
# JMMC project
#
# "@(#) $Id: ctooGetTemplateForCoding.sh,v 1.21 2006-06-05 13:57:37 lsauge Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.20  2006/06/05 13:44:54  lsauge
# Integrate xml-cdf template (Command Description File)
#
# Revision 1.19  2006/03/07 14:22:22  mella
# Add templates for java
#
# Revision 1.18  2005/11/30 13:41:55  gzins
# Fixed documentation concerning options to be used for C++ files
#
# Revision 1.17  2005/04/11 07:52:50  gluck
# Fix problem report 3: directories creation with correct header file for C++ and template names update
#
# Revision 1.16  2005/02/25 15:11:43  gluck
# Deleted obsolete comment block
#
# Revision 1.15  2005/02/15 15:48:11  gluck
# Correct bug due to missing space
#
# Revision 1.14  2005/02/15 14:55:26  gluck
# Correct wrapping line bug
#
# Revision 1.13  2005/02/15 14:51:16  gluck
# Add module name automatic replacement in Makefile comment header
#
# Revision 1.12  2005/01/24 15:47:51  gluck
# Bug correction for log message automatic insertion ($Log: not supported by cvs2svn $
# Bug correction for log message automatic insertion (Revision 1.20  2006/06/05 13:44:54  lsauge
# Bug correction for log message automatic insertion (Integrate xml-cdf template (Command Description File)
# Bug correction for log message automatic insertion (
# Bug correction for log message automatic insertion (Revision 1.19  2006/03/07 14:22:22  mella
# Bug correction for log message automatic insertion (Add templates for java
# Bug correction for log message automatic insertion (
# Bug correction for log message automatic insertion (Revision 1.18  2005/11/30 13:41:55  gzins
# Bug correction for log message automatic insertion (Fixed documentation concerning options to be used for C++ files
# Bug correction for log message automatic insertion (
# Bug correction for log message automatic insertion (Revision 1.17  2005/04/11 07:52:50  gluck
# Bug correction for log message automatic insertion (Fix problem report 3: directories creation with correct header file for C++ and template names update
# Bug correction for log message automatic insertion (
# Bug correction for log message automatic insertion (Revision 1.16  2005/02/25 15:11:43  gluck
# Bug correction for log message automatic insertion (Deleted obsolete comment block
# Bug correction for log message automatic insertion (
# Bug correction for log message automatic insertion (Revision 1.15  2005/02/15 15:48:11  gluck
# Bug correction for log message automatic insertion (Correct bug due to missing space
# Bug correction for log message automatic insertion (
# Bug correction for log message automatic insertion (Revision 1.14  2005/02/15 14:55:26  gluck
# Bug correction for log message automatic insertion (Correct wrapping line bug
# Bug correction for log message automatic insertion (
# Bug correction for log message automatic insertion (Revision 1.13  2005/02/15 14:51:16  gluck
# Bug correction for log message automatic insertion (Add module name automatic replacement in Makefile comment header
# Bug correction for log message automatic insertion ()
#
# lgluck    23-Apr-2004  Created
# gzins     04-Dec-2004  Changed C++ file extension to cpp
#                        Look for templates in the following order:
#                        ../templates, $INTROOT/templates and
#                        $MCSROOT/templates
# gzins     06-Dec-2004  Added class and module names substitution in C++
#                        header files
# gzins     09-Dec-2004  Changed call to ctooGetModuleName
# gzins     04-Jan-2005  Updated to accept relative path when giving file name
#
#*******************************************************************************

#/**
# \file
#  Get templates for coding.
#
# \synopsis
# ctooGetTemplateForCoding \e \<templateType\> 
#
# \param templateType : template type to generate. Should have one of the
# following value :
#   - c-main
#   - c-procedure
#   - h-file
#   - c++-small-main
#   - c++-class-file
#   - c++-h-file
#   - java-class
#   - script
#   - cdf-xml
#   - Makefile
# 
# \n
# \details
# Utility used to get a template file for coding. It asks the user
# to enter the template type to generate the corresponding file.
#
# \sa ctooGetCode.sh, ctooGetTemplate.sh, ctooGetTemplateFile.sh, \
# ctooGetModuleName.sh
#  
# \n 
# 
# */


# signal trap (if any)

usage()
{
    echo -e "\n\tUsage: ctooGetTemplateForCoding c-main|c-procedure|h-file|" 
    echo -e "\tc++-small-main|c++-class-definition-file|c++-class-interface-file|"
    echo -e "\tjava-main|java-class|java-interface|script|cdf-xml|Makefile"
}


# Set templates directories
if [ -d ../templates/forCoding ]
then
    TEMPLATES=../templates
elif [ -d $INTROOT/templates/forCoding ]
then
    TEMPLATES=$INTROOT/templates
else
    TEMPLATES=$MCSROOT/templates      
fi

CODE_DIR=$TEMPLATES/forCoding
MAKEFILE_DIR=$TEMPLATES/forMakefile

# check environment : verify that templates directories exist
if [ ! -d "$TEMPLATES" ]
then 
    echo "ERROR - ctooGetTemplateForCoding: $TEMPLATES not available. "
    echo "                                  Please check your MCS environment "
    exit 1
fi

if [ ! -d "$CODE_DIR" ]
then 
    echo "ERROR - ctooGetTemplateForCoding: $CODE_DIR not available. "
    echo "                                  Please check your MCS environment "
    exit 1
fi

if [ ! -d "$MAKEFILE_DIR" ]
then 
    echo "ERROR - ctooGetTemplateForCoding: $MAKEFILE_DIR not available. "
    echo "                                  Please check your MCS environment "
    exit 1
fi

# Input parameters given should be 1 and in the correct format:
if [ $# != 1 ]
then 
    usage
    exit 1
fi 

# Get the input parameters
choice=$1

# Treat the choice

# Test if the choice is not empty
if test -n "$choice"
then
    # Examine the choice
    case $choice in
        c-main|c-procedure)
            TEMPLATE=$CODE_DIR/$choice.template
            FILE_NAME=""
            FILE_SUFFIX=".c"
            MODE=644
            ;;

        c++-small-main|c++-class-definition-file)
            TEMPLATE=$CODE_DIR/$choice.template
            FILE_NAME=""
            FILE_SUFFIX=".cpp"
            MODE=644
            ;;

        h-file|c-h-file|c++-class-interface-file)
            TEMPLATE=$CODE_DIR/$choice.template
            FILE_NAME=""
            FILE_SUFFIX=".h"
            MODE=644
            ;;

        java-main|java-class|java-interface)
            TEMPLATE=$CODE_DIR/$choice.template
            FILE_NAME=""
            FILE_SUFFIX=".java"
            MODE=644
            ;;
 
        script)
            TEMPLATE=$CODE_DIR/$choice.template
            FILE_NAME=""
            FILE_SUFFIX=".sh"
            MODE=755
            ;;
        
        cdf-xml)
            TEMPLATE=$CODE_DIR/$choice.template
            FILE_NAME=""
            FILE_SUFFIX=".cdf"
            MODE=644
            ;;

        Makefile)
            TEMPLATE=$MAKEFILE_DIR/$choice.template
            FILE_NAME="Makefile"
            FILE_SUFFIX=""
            MODE=644
            ;;

        *)  
            TEMPLATE=""
            echo ">>$choice<< is an invalid choice. \c"
            ;;
    esac

    # Test if a template file has been chosen
    if [ "$TEMPLATE" != "" ]
    then
        # test if the template file exists
        if [ ! -f $TEMPLATE ]
        then
            # the template file doesn't exist
            echo -e "ERROR : the template $TEMPLATE file doesn't exist"
        fi
        
        # Test if FILE_NAME is empty : always the case except for the Makefile
        if [ "$FILE_NAME" = "" ]
        then
            # ask the user for a file name
            echo -e "\n-> Enter output file name (without extention) or press"
            echo -e "   <Enter> to quit: \c"
            read FILE_NAME
            # If no file name is given
            if [ "$FILE_NAME" = "" ]
            then 
                exit
            fi 
        fi
        
        # Build the whole file name
        FILE=${FILE_NAME}$FILE_SUFFIX

        # check that output file does not exist
        while [ -f $FILE -o -d $FILE ]
        do
            echo -e "\n-> FILE $FILE ALREADY EXISTS. Enter another file name"
            echo -e "   (without extention) or press <Enter> to quit: \c"
            read FILE_NAME
            if [ "$FILE_NAME" = "" ]
            then 
                exit
            else
                # Build the new whole file name
                FILE=${FILE_NAME}$FILE_SUFFIX
            fi
        done

        # Get template file
        ctooGetTemplateFile $TEMPLATE $FILE

        # Get module name
        MOD_NAME=`ctooGetModuleName`
        if [ $? != 0 ]
        then
            exit 1
        fi

        # Get filename from path
        BASE_NAME=`basename $FILE_NAME`
        if [ $? != 0 ]
        then
            exit 1
        fi

        # For .h
        # -> For .h (h-file, c-h-file or c++-class-interface-file) files
        # insert file name in the pre-processing directives to avoid multiple
        # inclusions
        # -> For .h (c++-class-interface-file) files insert class name in the
        # doxygen header block
        if [ "$FILE_SUFFIX" = ".h" ]
        then
            sed -e "1,$ s/#ifndef _H/#ifndef ${BASE_NAME}_H/g" \
                -e "1,$ s/#define _H/#define ${BASE_NAME}_H/g" \
                -e "1,$ s/#endif \/\*!_H\*\//#endif \/\*!${BASE_NAME}_H\*\//g" \
                -e "1,$ s/<className>/$BASE_NAME/g" \
                $FILE > ${FILE}.BAK

            # Remove the intermediate file ($FILE) and rename the output
            # file
            mv ${FILE}.BAK $FILE 
        fi

        # For .c .cpp .java and Makefile
        # -> For .c (c-main, c-procedure) and  .cpp (c++-small-main,
        # c++-class-definition-file) files insert module name in the
        # pre-processing directives for header inclusion
        # -> For .cpp (c++-class-definition-file) insert class name in the
        # pre-processing directives for header inclusion, in the doxygen header
        # block, and for constructor and destructor replacement
        # -> For .java replace <moduleName> and <className> 
        # -> For Makefile insert module name in file comment header
        if [ "$FILE_SUFFIX" = ".c" -o  "$FILE_SUFFIX" = ".cpp" -o \
             "$FILE_SUFFIX" = ".java" -o "$FILE_NAME" = "Makefile" ]
        then
            sed -e "1,$ s/<moduleName>/${MOD_NAME}/g" \
                -e "1,$ s/<className>/${BASE_NAME}/g" \
            $FILE > ${FILE}.BAK

            # Remove the intermediate file ($FILE) and rename the output
            # file
            mv ${FILE}.BAK $FILE
        fi
        
        # Change permissions of the new created file
        chmod $MODE $FILE
        
        # display the file if EDITOR is set
        if [ "$EDITOR" != "" ]
        then 
            $EDITOR $FILE &
            echo -e "\n>>>  CREATED --> $FILE. Opening $EDITOR on it.\c"
            sleep 1; echo " .\c"; sleep 1; echo -e " .\c"
        else
            echo -e "\n>>>  CREATED --> $FILE\n"
        fi
    else
        # invalid choice
        echo -e "\nInvalid choice."
        usage
        exit
    fi
else
    # No choice, <Enter> was pressed --> exit (up to previous level)
    exit
fi


#
# ___oOo___
