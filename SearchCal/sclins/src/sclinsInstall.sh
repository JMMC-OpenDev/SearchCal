#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclinsInstall.sh,v 1.4 2005-02-15 16:02:02 gzins Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
# gzins     04-Dec-2004  Created
# gzins     09-Dec-2004  Added '-h' and '-u' options
#                        Added logfile
#                        Deleted modules before retrieving from repository
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

# Check number of argument 
if [ $# != 0 -a $# != 1 ]
then 
    echo -e "\nUsage: sclinsInstall [-h] [-u]" 
    exit 1
fi

# Check -h option 
update="no"
if [ $# == 1 ]
then  
    if [ "$1" == "-h" ]
    then
        echo -e "Usage: sclinsInstall [-h] [-u]" 
        echo -e "\t-h\tprint this help."
        echo -e "\t-u\tdo not delete modules to be installed from the "
        echo -e "\t\tcurrent directory; they are just updated.\n"
        exit 1;
    elif [ "$1" == "-u" ]
    then
        update="yes"
    else
        echo -e "\nERROR : '$1' unknown option.\n" 
        exit 1
    fi
fi

# Get the current directory
dir=$PWD

# Propose the user to continue or abort
echo -e "\n-> All the SCALIB modules will be installed (or just updated) from"
echo -e "   '$dir' directory\n"
if [ "$update" == "no" ]
then
    echo -e "    WARNING: modules to be installed will be removed first"
    echo -e "    from the current directory. Use '-u' option to only "
    echo -e "    update modules\n"
fi
echo -e "    Press enter to continue or ^C to abort "
read choice

# List of SCALIB modules
scalib_modules="simcli alx vobs sclsvr sclgui"

# Delete modules first
cd $dir
if [ "$update" == "no" ]
then
    echo -e "Deleting modules..."
    rm -rf $scalib_modules
fi 

# Log file
mkdir -p INSTALL
logfile="$dir/INSTALL/sclinsInstall.log"
rm -f $logfile

# Retrieve modules from CVS repository
cd $dir
echo -e "Retrieving modules from repository..."
cvs co $scalib_modules > $logfile 2>&1
if [ $? != 0 ]
then
    echo -e "\nERROR: 'cvs co $scalib_modules' failed ... \n"; 
    tail $logfile
    echo -e "See log file '$logfile' for details."
    exit 1;
fi

# Compile and install them
echo -e "Building modules..."
for mod in $scalib_modules; do
    cd $dir
    echo -e "    $mod..."
    cd $mod/src 
    if [ $? != 0 ]
    then
        echo -e "\nERROR: 'cd $mod/src' failed ...\n";
        exit 1
    fi
    make all man install  >> $logfile 2>&1
    if [ $? != 0 ]
    then
        echo -e "\nERROR: 'make all man install' in $mod failed ...\n";
        tail $logfile
        echo -e "See log file '$logfile' for details."
        exit 1
    fi
done

echo -e "Installation done."
echo -e "See log file '$logfile' for details."
#___oOo___
