#!/bin/bash
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************


#/**
# \file
# Create/check module directory structure.
#
# \synopsis
# ctooGetTemplateForDirectoryStructure \e \<rootName\> <em> [creation] </em>
#
# \param rootName : directory name from which the directory structure starts.
#                   If not existing already, directory/ies are created.
#
# \n
# \opt
# \optname creation : create an empty module directory structure
# 
# \n
# \details
# Utility used to create new or missing part of MCS module directory
# structure. if option is used, no files are added to module subdirectories.
# If not used the following files are added, if not already existing :
#   - module description file (moduleDescription.xml) in the doc subdirectory.
#   - module documentation file (\<moduleName\>.doc) in the src subdirectory.
#   - Makefile in the src subdirectory.
#   - module header file (\<moduleName\>.h) in the include subdirectory.
#   - module private header file (\<moduleName\>Private.h) in the include
#   subdirectory.
# 
#
# \sa ctooGetDirectoryStructure.sh, ctooGetTemplate.sh
# \sa MCS - Programming Standard
# 
# \n 
# 
# */


# signal trap (if any)


# Input parameters given should be 2 or 3 and in the correct format:
if [ $# != 1 -a $# != 2 ]
then 
    echo -e "\n\tUsage: ctooGetTemplateForDirectoryStructure <rootName>"
    echo -e "[creation]\n"
    exit 1
fi 

# Get the input parameters
ROOT_NAME=$1
cvs=$2

# Set templates directories
# Set templates directories
if [ -d ../templates/forMakefile ]
then
    TEMPLATES=../templates
elif [ -d $INTROOT/templates/forMakefile ]
then
    TEMPLATES=$INTROOT/templates
else
    TEMPLATES=$MCSROOT/templates      
fi

# directories that shall be present in any module area
MODROOT_LIST="bin           \
              config        \
              doc           \
              errors        \
              include       \
              lib           \
              object        \
              src           \
              test          \
              tmp           \
             "


# Check validity for parameter ROOT_NAME

# Test existence of a ROOT_NAME file
if [ -f $ROOT_NAME ]
then
    # a file called ROOT_NAME already exists
    echo -e "\n ERROR: I cannot create the starting directory because a file"
    echo "             called >>$ROOT_NAME<< already exists."
    echo "             Use another name or remove the existing file"
    echo ""
    exit 1
fi

# Test existence of a ROOT_NAME directory
if [ ! -d $ROOT_NAME ]
then
    # there is no ROOT_NAME directory
    # Create ROOT_NAME directory
    if mkdir $ROOT_NAME
    then
        # Creation succeeds
        echo "   CREATED >>> |---$ROOT_NAME "
    else
        # Creation failed 
        echo -e "\n ERROR: I cannot create the starting directory"
        echo -e "          >>$ROOT_NAME<<"
        echo "             Please fix the problem and try again."
        echo ""
        exit 1
    fi
else
    # ROOT_NAME  directory already exixts
    echo "               |---$ROOT_NAME "
fi


# Check directory structure

# If not already there, create all the needed subdirectories
for dir in $MODROOT_LIST
do
    if [ ! -d $ROOT_NAME/$dir ]
    then
        mkdir $ROOT_NAME/$dir
        echo "   CREATED >>>     |---$dir "
    else
        echo "                   |---$dir "
    fi
done


# Option handling

# Set permission level for further created files
MODE=644

# check environment : verify that templates directory exists
if [ ! -d "$TEMPLATES" ]
then 
    echo "ERROR - ctooGetTemplateForDirectoryStructure: $TEMPLATES "
    echo "        is not available. Please check your MCS environment"
    exit 1
fi

case $cvs in
    "") # The option has not been chosen
    
        # If not exist, get module description file
        # Check module description file existence
        if [ -f $ROOT_NAME/doc/moduleDescription.xml ]
        then
            # The module description file already exists
            echo -e "\n>>> moduleDescription.xml ALREADY EXISTS."
            echo -e "    => The existing one is left\n"
        else
            # The module description file does not exist
            echo -e "\n>>> Copying module description file\n"
            TEMPLATE="$TEMPLATES/forDocumentation/moduleDescription.template"
            FILE=$ROOT_NAME/doc/moduleDescription.xml

            # Get template file (module description file) in doc
            # directory
            ctooGetTemplateFile $TEMPLATE $FILE

            # Replace module name
            sed -e "1,$ s/<module name=\"module\">/<module name=\"$ROOT_NAME\">/g" \
                    $FILE > ${FILE}.BAK

            # Remove the intermediate file ($FILE) and rename the output
            # file
            mv ${FILE}.BAK $FILE
            
            # Change permissions of the new created file
            chmod $MODE $FILE
        fi

        # If not exist, get module documentation file
        # Check module documentation file existence
        if [ -f $ROOT_NAME/src/$ROOT_NAME.doc ]
        then
            # The module documentation file already exists
            echo -e "\n>>> $ROOT_NAME.doc ALREADY EXISTS."
            echo -e "    => The existing one is left\n"
        else
            # The module documentation file does not exist
            echo -e "\n>>> Copying module documentation file\n"
            TEMPLATE="$TEMPLATES/forDocumentation/moduleDocumentation.template"
            FILE=$ROOT_NAME/src/$ROOT_NAME.doc

            # Get template file (module documentation file) in src
            # directory
            ctooGetTemplateFile $TEMPLATE $FILE

            # Replace module name
            sed -e "1,$ s/<moduleName>/$ROOT_NAME/g" $FILE > ${FILE}.BAK

            # Remove the intermediate file ($FILE) and rename the output
            # file
            mv ${FILE}.BAK $FILE
            
            # Change permissions of the new created file
            chmod $MODE $FILE
        fi
        
        # If not exist, get Makefile
        # Check Makefile existence
        if [ -f $ROOT_NAME/src/Makefile ]
        then
            # a Makefile already exists
            echo -e "\n>>> A makefile ALREADY EXISTS."
            echo -e "    => The existing one is left\n"
        else
            # a Makefile does not exist
            echo -e "\n>>> Copying Makefile template for code\n"
            TEMPLATE=$TEMPLATES/forMakefile/Makefile.template
            FILE=$ROOT_NAME/src/Makefile
            
            # Get template file (Makefile) in src directory
            ctooGetTemplateFile $TEMPLATE $FILE
            
            # Replace module name
            sed -e "1,$ s/<moduleName>/$ROOT_NAME/g" $FILE > ${FILE}.BAK

            # Remove the intermediate file ($FILE) and rename the output
            # file
            mv ${FILE}.BAK $FILE

            # Change permissions of the new created file
            chmod $MODE $FILE
        fi
        
        # If not exist, get module header file
        # Check module header file existence
        if [ -f $ROOT_NAME/include/${ROOT_NAME}.h ]
        then
            # The module header file already exists
            echo -e "\n>>> ${ROOT_NAME}.h ALREADY EXISTS."
            echo -e "    => The existing one is left\n"
        else
            # The module header file does not exist
            echo -e "\n>>> Copying module header file\n"

            # Go to $ROOT_NAME/include directory, which is required by
            # the script below
            cd $ROOT_NAME/include

            # Switch off the environment variable EDITOR, to not
            # trigger the automatic editor pop up
            export EDITOR=""

            # Get module header file in include directory
            ctooGetSpecificHeaderFile module

            # set new created file
            FILE=${ROOT_NAME}.h
            
            # Replace general brief description by a specific one
            briefDescription="$ROOT_NAME general header file."
            sed -e "1,$ s/Brief.*/$briefDescription/g" $FILE > ${FILE}.BAK
            
            # Remove the intermediate file ($FILE) and rename the output
            # file
            mv ${FILE}.BAK $FILE
            
            # Switch on the environment variable EDITOR, to trigger
            # the automatic editor pop up
            export EDITOR=gvim
            
            # Change permissions of the new created file
            chmod $MODE $FILE

            # Go back to the previous directory
            cd ../..
        fi

        # If not exist, get module private header file
        # Check module private header file existence
        if [ -f $ROOT_NAME/include/${ROOT_NAME}Private.h ]
        then
            # The module private header file already exists
            echo -e "\n>>> ${ROOT_NAME}Private.h ALREADY EXISTS."
            echo -e "    => The existing one is left\n"
        else
            # The module private header file does not exist
            echo -e "\n>>> Copying module private header file\n"

            # Go to $ROOT_NAME/include directory, which is required by
            # the script below
            cd $ROOT_NAME/include

            # Switch off the environment variable EDITOR, to not
            # trigger the automatic editor pop up
            export EDITOR=""

            # Get module private header file in include directory
            ctooGetSpecificHeaderFile private

            # set new created file
            FILE=${ROOT_NAME}Private.h
            
            # Replace general brief description by a specific one
            briefDescription="$ROOT_NAME private header file."
            sed -e "1,$ s/Brief.*/$briefDescription/g" $FILE > ${FILE}.BAK
            
            # Remove the intermediate file ($FILE) and rename the output
            # file
            mv ${FILE}.BAK $FILE
            
            # Switch on the environment variable EDITOR, to trigger
            # the automatic editor pop up
            export EDITOR=gvim
            
            # Change permissions of the new created file
            chmod $MODE $FILE

            # Go back to the previous directory
            cd ../..
        fi
        ;;
        
    creation) # The creation option has been chosen
        echo -e "\n>>> No files added in subdirectories of the module\n"
        ;;

    *) # Wrong option value
        echo "ERROR second parameter (option) is not a valid."
        echo -e "\n\tUsage: getTemplateForDirectory <rootName> [creation]\n"
        exit 1
        ;;
esac


#___oOo___
