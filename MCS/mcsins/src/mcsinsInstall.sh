#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: mcsinsInstall.sh,v 1.3 2004-12-12 09:34:40 gzins Exp $"
#
# who       when         what
# --------  -----------  -------------------------------------------------------
# gzins     04-Dec-2004  Created
# gzins     08-Dec-2004  Moved from mkf module
#                        Added installation of env module
# gzins     08-Dec-2004  Added installation of gwt module
# gzins     09-Dec-2004  Renamed to mcsinsInstall
# gzins     09-Dec-2004  Added '-h' and '-u' options
#                        Added logfile
#                        Deleted modules before retrieving from repository
#
#*******************************************************************************
#   NAME 
#   mcsinsInstall - Install/Update MCS modules 
# 
#   SYNOPSIS
#   mcsinsInstall [-u]
# 
#   DESCRIPTION
#   This command retreives all the modules belonging to MCS from the CVS
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
    echo -e "\nUsage: mcsinsInstall [-h] [-u]" 
    exit 1
fi

# Check -h option 
update="no"
if [ $# == 1 ]
then  
    if [ "$1" == "-h" ]
    then
        echo -e "Usage: mcsinsInstall [-h] [-u]" 
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
echo -e "\n-> All the MCS modules will be installed from"
echo -e "   '$dir' directory\n"
if [ "$update" == "no" ]
then
    echo -e "    WARNING: modules to be installed will be removed first"
    echo -e "    from the current directory. Use '-u' option to only "
    echo -e "    update modules\n"
fi
echo -e "    Press enter to continue or ^C to abort "
read choice

# List of MCS modules
mcs_modules="mkf ctoo mcs log err misc modc modcpp fnd env cmd msg evh gwt"

# Delete modules first
cd $dir
if [ "$update" == "no" ]
then
    echo -e "Deleting modules..."
    rm -rf $mcs_modules
fi 

# Log file
mkdir -p INSTALL
logfile="$dir/INSTALL/mcsinsInstall.log"
rm -f $logfile

# Retrieve modules from CVS repository
echo -e "Retrieving modules from repository..."
cd $dir
cvs co $mcs_modules > $logfile 2>&1
if [ $? != 0 ]
then
    echo -e "\nERROR: 'cvs co $mcs_modules' failed ... \n"; 
    tail $logfile
    echo -e "See log file '$logfile' for details."
    exit 1;
fi

# Compile and install them
echo -e "Building modules..."
for mod in $mcs_modules; do
    cd $dir
    echo -e "    $mod..."
    cd $mod/src 
    if [ $? != 0 ]
    then
        echo -e "\nERROR: 'cd $mod/src' failed ...\n";
        exit 1
    fi
    make all man install >> $logfile 2>&1
    if [ $? != 0 ]
    then
        echo -e "\nERROR: 'make all man install' in $mod failed ...\n";
        tail $logfile
        echo -e "See log file '$logfile' for details."
        exit 1
    fi
done
echo -e "Istallation done."
echo -e "See log file '$logfile' for details."
#___oOo___
