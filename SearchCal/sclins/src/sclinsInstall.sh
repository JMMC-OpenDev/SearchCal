#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclinsInstall.sh,v 1.2 2004-12-09 06:25:55 gzins Exp $"
#
# who       when         what
# --------  -----------  -------------------------------------------------------
# gzins     04-Dec-2004  Created
#
#*******************************************************************************
#   NAME 
#   sclinsInstall - Install/Update SCALIB modules
# 
#   SYNOPSIS
#   sclinsInstall
# 
#   DESCRIPTION
#   This command retreives all the modules belonging to SCALIB from the CVS
#   repository and install them.
#
#   FILES
#
#   ENVIRONMENT
#
#   RETURN VALUES
#
#   CAUTIONS
#
#   EXAMPLES
#
#   SEE ALSO
#
#   BUGS     
#
#-------------------------------------------------------------------------------
#

# Get the current directory
dir=$PWD

# Propose the user to continue or abort
echo -e "\n-> All the SCALIB modules will be installed (or just updated) from"
echo -e "   '$dir' directory\n"
echo -e "   Press enter to continue or ^C to abort "
read choice

# List of SCALIB modules
scalib_modules="simcli alx vobs sclsvr sclgui"

# Retrieve modules from CVS repository
cd $dir
cvs co $scalib_modules
if [ $? != 0 ]
then
    echo -e "\nERROR: 'cvs co $scalib_modules' failed ... \n"; 
    exit 1;
fi

# Compile and install them
for mod in $scalib_modules; do
    cd $dir
    cd $mod/src 
    if [ $? != 0 ]
    then
        echo -e "\nERROR: 'cd $mod/src' failed ...\n";
        exit 1
    fi
    make all man install 
    if [ $? != 0 ]
    then
        echo -e "\nERROR: 'make all man install' in $mod failed ...\n";
        exit 1
    fi
    echo ""
done
#___oOo___
