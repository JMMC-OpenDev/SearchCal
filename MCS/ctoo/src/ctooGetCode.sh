#! /bin/bash
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************

#/**
# \file
# Interactive script to get standard templates for coding.
#
# \synopsis
# ctooGetCode 
#
# \details
# Interactive utility to get template files for code. Following templates are
# available :
#   -# c-main
#   -# c-procedure
#   -# h-file
#   -# c++-small-main
#   -# c++-class-file
#   -# c++-h-file
#   -# script
#   -# Makefile
#   -# java-class
#   -# java-interface
#   -# java-singleton
# 
# \sa ctooGetTemplateForCoding.sh, ctooGetTemplateFile.sh, ctooGetTemplate.sh
#  
# \n 
# 
# */


# signal trap (if any)


# Print out the menu
cat <<xyz
--------------------------------------------------------------------------------
This menu allows you to copy one of the available templates with a filename of
your choice in the current directory (the output file must not already exist).

Templates are available for:

        1- c-main                         8- script                
        2- c-procedure                    
        3- c-h-file                       9- Makefile
                                          
        4- c++-small-main                10- java-main
        5- c++-class-definition-file     11- java-class
        6- c++-class-interface-file      12- java-interface
                                         13- java-singleton

        7- command-definition-file       14- xml-file
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
            ctooGetTemplateForCoding c-h-file
            ;;
        
        4)  # c++-small-main choice
            ctooGetTemplateForCoding c++-small-main 
            ;;
        
        5)  # c++-class-file choice
            ctooGetTemplateForCoding c++-class-definition-file 
            ;;
        
        6)  # c++-h-file choice
            ctooGetTemplateForCoding c++-class-interface-file
            ;;
            
        7)  # command-definition-file 
            ctooGetTemplateForCoding cdf-xml
            ;;

        8)  # script file choice
            ctooGetTemplateForCoding script
            ;;
        
        9)  # Makefile choice
            ctooGetTemplateForCoding Makefile
            ;;
 
        10) # java-main choice
            ctooGetTemplateForCoding java-main
            ;;
        
        11) # java-class choice
            ctooGetTemplateForCoding java-class
            ;;
 
        12) # java-interface choice
            ctooGetTemplateForCoding java-interface
            ;;
                
        13) # java-singleton choice
            ctooGetTemplateForCoding java-singleton
            ;;
                
        14) # xml-file choice
            ctooGetTemplateForCoding xml-file
            ;;
        
        *) echo "ERROR : invalid choice";;
        
    esac

else
    # <Enter> was pressed
    exit
fi

#
# ___oOo___
