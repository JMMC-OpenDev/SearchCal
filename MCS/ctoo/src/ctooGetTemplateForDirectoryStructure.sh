#!/bin/bash

#*******************************************************************************
# JMMC project
#
# "@(#) $Id: ctooGetTemplateForDirectoryStructure.sh,v 1.3 2004-09-14 08:55:51 gluck Exp $"
#
# who       when        what
# --------  --------    ------------------------------------------------
# lgluck    23/04/04    Created
#
#*******************************************************************************
# NAME
#   ctooGetTemplateForDirectoryStructure.sh - create/check standard directory structure
#
# SYNOPSIS
#   ctooGetTemplateForDirectoryStructure MODROOT|INTROOT|MCSROOT|MCSDATA 
#                                        <root name> [creation]
#
# DESCRIPTION
#   Utility used ot create new or missing part of MCS directory structure for:
#       - MODROOT   for a Module directory structure
#       - INTROOT   for an Integration directory structure
#       - MCSROOT   for a MCS Root directory structure
#       - MCSDATA   for a MCS Data directory structure
#
#       <directoryStructureTtype> directory structure type to create.
#                                 It should be one of the 4 following values :
#                                 MODROOT, INTROOT, MCSROOT or MCSDATA
#       <name> the name of the directory from which the directory structure
#              starts. If not existing already, directory/ies are created.
#
#       Remark : for MODROOT directory structure, a Makefile template is added
#                to the src subdirectory, if the option creation is not chosen.
#
#   Option :
#       [creation] : if this option is used, no files are added 
#                    to module subdirectories (ex Makefile in src,
#                    moduleDescription.xml in doc, ...).
#
# FILES
#
# ENVIRONMENT
#
# RETURN VALUES
#
# CAUTIONS
#
# SEE ALSO 
#   ctooGetDirectoryStructure
#   MCS - Programming Standard
#
# BUGS    
#
#----------------------------------------------------------------------
#

# signal trap (if any)


# Input parameters given should be 2 or 3 and in the correct format:
if [ $# != 2 -a $# != 3 ]
then 
    echo -e "\n\tUsage: ctooGetTemplateForDirectoryStructure"
    echo -e "MODROOT|INTROOT|VLTROOT|VLTDATA <root name> [creation]\n"
    exit 1
fi 

# Get the input parameters
directoryStructureTtype=$1
ROOT_NAME=$2
cvs=$3

# Set templates directories
MCSTEMPLATES=$MCSROOT/templates

# check environment : verify that templates directory exist
if [ ! -d "$MCSTEMPLATES" ]
then 
    echo "ERROR - ctooGetTemplateForDirectoryStructure: $MCSTEMPLATES not
                  available. Please check your MCS environment"
    exit 1
fi


# Define the content of each area 

# Directories that shall be present in any area 
BASIC_DIRS="config      \
            doc         \
            bin         \
            include     \
            lib         \
            errors      \
            man         \
            man/man1    \
            man/man2    \
            man/man3    \
            man/man4    \
            man/man5    \
            man/man6    \
            man/man7    \
            man/man8    \
            man/manl    \
            man/mann    \
           "

# directories that shall be present in any module area
MODROOT_LIST="$BASIC_DIRS  \
              src          \
              object       \
              test         \
              tmp          \
             "

# directories that shall be present in both integration and MCS area 
INTROOT_LIST="$BASIC_DIRS templates"
MCSROOT_LIST="$BASIC_DIRS templates etc" 

# MCSDATA directories
MCSDATA_LIST="tmp"


# Define directories list of the structure to build
case $directoryStructureTtype in
    MODROOT)
        echo -e "\nCreating/checking Module directory"
        DIR_LIST="$MODROOT_LIST"
        ;;
    INTROOT)
        echo -e "\nCreating/checking Integration directory"
        DIR_LIST="$INTROOT_LIST"
        ;;
    MCSROOT)
        echo -e "\nCreating/checking MCS Root directory"
        DIR_LIST="$MCSROOT_LIST"
        ;;
    MCSDATA)
        echo -e "\nCreating/checking MCS Data directory"
        DIR_LIST="$MCSDATA_LIST"
        ;;
    *)  
        echo "ERROR >>$directoryStructureTtype<< is not a valid directory "
        echo "structure."
        echo -e "\n\tUsage: getTemplateForDirectory \
                MODROOT|INTROOT|VLTROOT|VLTDATA <root name> [creation]\n"
        exit 1
        ;;
esac


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
for dir in $DIR_LIST
do
    if [ ! -d $ROOT_NAME/$dir ]
    then
        mkdir $ROOT_NAME/$dir
        echo "   CREATED >>>     |---$dir "
    else
        echo "                   |---$dir "
    fi
done


# Additional case specific actions:
MODE=644
case $directoryStructureTtype in
    MODROOT)
        case $cvs in
            "")
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
                    TEMPLATE="$MCSTEMPLATES/forDocumentation/moduleDescription.template"
                    FILE=$ROOT_NAME/doc/moduleDescription.xml

                    # Get template file (module description file) in doc
                    # directory
                    ctooGetTemplateFile.sh $TEMPLATE $FILE

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
                    TEMPLATE="$MCSTEMPLATES/forDocumentation/moduleDocumentation.template"
                    FILE=$ROOT_NAME/src/$ROOT_NAME.doc

                    # Get template file (module documentation file) in src
                    # directory
                    ctooGetTemplateFile.sh $TEMPLATE $FILE

                    # Replace module name
                    sed -e "1,$ s/moduleName/$ROOT_NAME/g" $FILE > ${FILE}.BAK

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
                    TEMPLATE=$MCSTEMPLATES/forMakefile/Makefile.template
                    FILE=$ROOT_NAME/src/Makefile
                    
                    # Get template file (Makefile) in src directory
                    ctooGetTemplateFile.sh $TEMPLATE $FILE
                    
                    # Change permissions of the new created file
                    chmod $MODE $FILE
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
                    ctooGetPrivateHeaderFile

                    # Switch on the environment variable EDITOR, to trigger
                    # the automatic editor pop up
                    export EDITOR=gvim
                    
                    # Change permissions of the new created file
                    chmod $MODE ${ROOT_NAME}Private.h

                    # Go back to the previous directory
                    cd ../..
                fi
                ;;
                
            creation)
                echo -e "\n>>> No files added in subdirectories of the module\n"
                ;;

            *)
                # Wrong parameter value
                echo "ERROR third option is not a valid."
                echo -e "\n\tUsage: getTemplateForDirectory MODROOT|INTROOT|"
                echo -e "         VLTROOT|VLTDATA <root name> [creation]\n"
                exit 1
                ;;
        esac
        ;;
    
    INTROOT)
        for dir in $INTROOT_LIST
        do
            # directories must be writable by other developers
            chmod 777 $ROOT_NAME/$dir
        done
        echo -e "\n Remember to define \$INTROOT to make this area accessible."
        echo -e "\n"
        ;;
    
    MCSROOT)
        echo -e "\n Remember to define \$MCSROOT to make this area accessible."
        echo -e "\n"
        ;;
    
    MCSDATA)
        for dir in $MCSDATA_LIST
        do
            chmod 777 $ROOT_NAME/$dir
        done
        echo -e "\n Remember to define \$MCSDATA to make this area accessible."
        echo -e "\n"
        ;;
    
    *)
        echo " INTERNAL ERROR"
        exit 1
        ;;

esac


#
#___oOo___
