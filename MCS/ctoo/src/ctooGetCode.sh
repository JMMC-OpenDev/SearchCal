#! /bin/bash

#*******************************************************************************
# JMMC project
#
# "@(#) $Id: ctooGetCode.sh,v 1.1 2004-09-10 17:40:27 gzins Exp $"
#
# who       when        what
# --------  --------    ------------------------------------------------
# lgluck    14/05/04    Created
#
#*******************************************************************************
# NAME
#   ctooGetCode -  interactive script to get standard templates for coding
# 
# SYNOPSIS
#   ctooGetCode
# 
# DESCRIPTION
#   Utility to get standard template for :
#       1- c-main
#       2- c-procedure
#       3- h-file
#   
#       4- c++-small-main
#       5- c++-class-file
#       6- c++-h-file
#   
#       7- script
#   
#       8- Makefile
#
# FILES
#
# ENVIRONMENT
#
# RETURN VALUES
#
# CAUTIONS
#
# EXAMPLES
#
# SEE ALSO
#
# BUGS
#
#-------------------------------------------------------------------------------
#

# Print out the menu
cat <<xyz
--------------------------------------------------------------------------------
This menu allows you to copy one of the available templates with a filename of
your choice in the current directory (the output file must not already exist).


Templates are available for:

        1- c-main
        2- c-procedure
        3- h-file

        4- c++-small-main
        5- c++-class-file
        6- c++-h-file
        
        7- script
        
        8- Makefile
xyz

# Propose the user to enter his choice
echo -e "\n-> Enter the number corresponding to the template type you need or"
echo -e "   press <Enter> to exit: \c"

# Read the choice
read choice


# Treat the choice

# Test if the choice is not empty
if test -n "$choice"
then
    # Examine the choice
    case $choice in
        1)  # c-main choice
            ctooGetTemplateForCoding c-main
            ;;
        
        2)  # c-procedure choice
            ctooGetTemplateForCoding c-procedure
            ;;
        
        3)  # h-file choice
            ctooGetTemplateForCoding h-file
            ;;
        
        4)  # c++-small-main choice
            ctooGetTemplateForCoding c++-small-main 
            ;;
        
        5)  # c++-class-file choice
            ctooGetTemplateForCoding c++-class-file 
            ;;
        
        6)  # c++-h-file choice
            ctooGetTemplateForCoding c++-h-file
            ;;
        
        7)  # script file choice
            ctooGetTemplateForCoding script
            ;;
        
        8)  # Makefile choice
            ctooGetTemplateForCoding Makefile
            ;;
        
        *) echo "ERROR : invalid choice";;
        
    esac

else
    # <Enter> was pressed
    exit
fi

#
# ___oOo___
