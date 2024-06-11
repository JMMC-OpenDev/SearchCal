#! /bin/bash
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
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
#   - java-interface
#   - java-singleton
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
    echo -e "\tjava-main|java-class|java-interface|java-singleton|script|cdf-xml|xml-file|Makefile"
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

        java-main|java-class|java-interface|java-singleton)
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
 
        xml-file)
            TEMPLATE=$CODE_DIR/$choice.template
            FILE_NAME=""
            FILE_SUFFIX=".xml"
            MODE=644
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
