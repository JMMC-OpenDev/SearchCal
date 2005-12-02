#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclinsInstall.sh,v 1.8 2005-12-02 13:54:25 gzins Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.7  2005/09/15 07:18:08  swmgr
# Display information of installation directory
#
# Revision 1.6  2005/09/14 22:04:06  gzins
# Added -c and -t options + improved checks
#
# Revision 1.5  2005/02/17 14:05:39  gzins
# Changed 'make all man install' to 'make clean all man install'
#
# Revision 1.4  2005/02/15 16:02:02  gzins
# Added CVS log as file modification history
#
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

# Print usage 
function printUsage () {
        echo -e "Usage: sclinsInstall [-h] [-c] [-u] [-t tag]" 
        echo -e "\t-h\tprint this help."
        echo -e "\t-c\tonly compile; i.e. do not retrieve modules from "
        echo -e "\t\trepository."
        echo -e "\t-u\tdo not delete modules to be installed from the "
        echo -e "\t\tcurrent directory; they are just updated."
        echo -e "\t-t tag\tuse revision 'tag' when retrieving modules.\n"
        exit 1;
}

# Parse command-line parameters
update="no";
retrieve="yes";
tag="";
while getopts "chut:" option
# Initial declaration.
# c, h, u and t are the options (flags) expected.
# The : after option 't' shows it will have an argument passed with it.
do
  case $option in
    h ) # Help option
        printUsage ;;
    u ) # Update option
        update="yes";;
    c ) # Update option
        retrieve="no";;
    t ) # Update option
        tag="$OPTARG";;
    * ) # Unknown option
        printUsage ;;
    esac
done

# Check that all options have been parsed 
if [ $# -ge $OPTIND ]
then 
    echo -e "\nUsage: sclinsInstall [-h] [-u]" 
    exit 1
fi

#
# Check that the script is not run by 'root'
if [ `whoami` == "root" ]
then
    echo -e "\nERROR : MCS installation MUST NOT BE done as root !!" 
    echo -e "\n  ->  Please log in as swmgr, and start again.\n" 
    exit 1
fi


# Check that MCSROOT is defined
if [ "$MCSROOT" == "" ]
then
    echo -e "\nWARNING : MCSROOT is not defined!!"
    echo -e ""
    exit 1
fi

# Get intallation directory
if [ "$INTROOT" != "" ]
then
    insDirName="INTROOT"
    insDir=$INTROOT
else
    insDirName="MCSROOT"
    insDir=$MCSROOT
fi

#
# Get the current directory
dir=$PWD

# Propose the user to continue or abort
echo -e "\n-> All the SCALIB modules will be installed (or just updated)"
echo -e "        from     : $dir"
echo -e "        into     : $insDir"
if [ "$update" == "no" -a  "$retrieve" == "yes" ]
then
    echo -e "    WARNING: modules to be installed will be removed first"
    echo -e "    from the current directory. Use '-u' option to only "
    echo -e "    update modules\n"
fi
echo -e "    Press enter to continue or ^C to abort "
read choice

# List of SCALIB modules
scalibModules="simcli alx vobs sclsvr sclgui"

# Log file
mkdir -p INSTALL
logfile="$dir/INSTALL/sclinsInstall.log"
rm -f $logfile

# If modules have to be retrieved from repository
if [ "$retrieve" == "yes" ]
then
    # Delete modules first
    cd $dir
    if [ "$update" == "no" ]
    then
        echo -e "Deleting modules..."
        rm -rf $scalibModules
    fi 

    # Retrieve modules from CVS repository
    # When a revision tag is specified, we have first to retrieve module giving
    # this tag, and then to retrieve again to create empty directories which are
    # not created by cvs command when '-r' option is used.
    echo -e "Retrieving modules from repository..."
    cd $dir
    if [ "$tag" != "" ]
    then
        cvs co -r $tag $scalibModules > $logfile 2>&1
        if [ $? != 0 ]
        then
            echo -e "\nERROR: 'cvs co -r $tag $scalibModules' failed ... \n"; 
            tail $logfile
            echo -e "See log file '$logfile' for details."
            exit 1;
        fi
    fi

    cvs co $scalibModules > $logfile 2>&1
    if [ $? != 0 ]
    then
        echo -e "\nERROR: 'cvs co $scalibModules' failed ... \n"; 
        tail $logfile
        echo -e "See log file '$logfile' for details."
        exit 1;
    fi
fi

# Check all modules are there
for mod in $scalibModules; do
    cd $dir
    if [ ! -d $mod ]
    then
        echo -e "\nERROR: '$mod' must be retrieved from repository first ...\n";
        exit 1
    fi
done

# Compile and install them
echo -e "Building modules..."
for mod in $scalibModules; do
    cd $dir
    echo -e "    $mod..."
    cd $mod/src 
    if [ $? != 0 ]
    then
        echo -e "\nERROR: 'cd $mod/src' failed ...\n";
        exit 1
    fi
    make clean all man install  >> $logfile 2>&1
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
