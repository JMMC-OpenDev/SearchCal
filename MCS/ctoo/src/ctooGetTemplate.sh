#! /bin/bash

#*******************************************************************************
# JMMC project
#
# "@(#) $Id: ctooGetTemplate.sh,v 1.2 2004-09-15 07:46:41 gluck Exp $"
#
# who       when        what
# --------  --------    ------------------------------------------------
# lgluck    23/04/04    Created
#
#*******************************************************************************

#/**
# \file
# Create module directory structure or template files for code.
#
# \synopsis
# ctooGetTemplate
#
# \details
# Allow to create 2 kinds of working structure for development.
# -# module directory structure : a way to organize properly module files
# -# templates for code : following standard templates are available
#                         - shell scripts
#                         - C
#                         - C++
#                         - Makefile
# 
# */


# signal trap (if any)


# Print out the menu
cat <<xyz
--------------------------------------------------------------------------------
Templates are available for :
    1- module directory structure
    2- code
xyz

# Propose the user to enter his choice
echo -e "\n-> Enter the number corresponding to the working structure you need "
echo -e "   or press <Enter> to exit : \c"

# Read the choice
read choice

# Treat the choice

# Test if the choice is not empty
if test -n "$choice"
then
    # A choice is entered

    # Test if the choice is not "directoryStructure"
    if [ $choice != 1 ]
    then
            # check environment
            MCSTEMPLATES=$MCSROOT/templates      

            # verify that MCSTEMPLATES is a directory
            if [ ! -d "$MCSTEMPLATES" ]
            then 
                echo "ERROR - ctooGetTemplate: $MCSTEMPLATES not available."
                echo "                         Please check your MCS"
                echo "                         environment"
                exit 1
            fi
    fi
        # Examine the choice
        case $choice in
            1) ctooGetDirectoryStructure;;
            2) ctooGetCode;;
            *) echo "ERROR : invalid choice";;
        esac
    else
        # <Enter> was pressed
        exit
fi

#
# ___oOo___
