#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclinsInstall.sh,v 1.12 2008-09-22 14:04:58 lafrasse Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.11  2007/02/14 10:22:09  lafrasse
# Added 'sclws' & 'jmcs' modules auto-installation.
#
# Revision 1.10  2006/10/10 10:54:49  lafrasse
# Added '-m' option to deactivate man pages and documentation generation.
#
# Revision 1.9  2006/02/20 13:21:25  swmgr
# Updated to be similar to mcsinsInstall
#
# Revision 1.8  2005/12/02 13:54:25  gzins
# Remove MCS installation check
#
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
        echo -e "Usage: sclinsInstall [-h] [-c] [-u] [-m] [-t tag]|[-b branch]" 
        echo -e "\t-h\tPrint this help."
        echo -e "\t-c\tOnly compile; i.e. do not retrieve modules from "
        echo -e "\t\trepository."
        echo -e "\t-u\tDo not delete modules to be installed from the "
        echo -e "\t\tcurrent directory; they are just updated."
        echo -e "\t-m\tDo not create man pages."
        echo -e "\t-t tag\tUse revision 'tag' when retrieving modules."
        echo -e "\t-b brch\tUse 'brch' branch when retrieving modules."
        echo
        exit 1;
}

# Parse command-line parameters
update="no";
retrieve="yes";
manpages="yes";
tag="";
while getopts "chumt:b:" option
# Initial declaration.
# c, h, u, m, t and b are the options (flags) expected.
# The : after options 't' and 'b' shows it will have an argument passed with it.
do
  case $option in
    h ) # Help option
        printUsage ;;
    u ) # Update option
        update="yes";;
    c ) # Update option
        retrieve="no";;
    t ) # Tag option
        tag="$OPTARG";;
    b ) # Branch option
        branch="$OPTARG";;
    m ) # No man pages creation
        manpages="no";;
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
    echo -e "\nERROR : SCALIB installation MUST NOT BE done as root !!" 
    echo -e "\n  ->  Please log in as swmgr, and start again.\n" 
    exit 1
fi

# Determine the SW package
export SW_PACKAGE=SCALIB

# Determine the SW release
if [ "$tag" != "" ]
then
    export SW_RELEASE=$tag
elif [ "$branch" != "" ]
then
    export SW_RELEASE=$branch
else
    export SW_RELEASE=DEVELOPMENT
fi

# Check that MCSROOT is defined
if [ "$MCSROOT" == "" ]
then
    echo -e "\nWARNING : MCSROOT is not defined!!"
    echo -e ""
    exit 1
fi

# Set directory from where SCALIB will be installed 
fromdir=$PWD/$SW_PACKAGE/$SW_RELEASE

# Get intallation directory
if [ "$INTROOT" != "" ]
then
    insDirName="INTROOT"
    insDir=$INTROOT
else
    insDirName="MCSROOT"
    insDir=$MCSROOT
fi

# Propose the user to continue or abort
echo -e "\n-> All the SCALIB modules will be installed (or just updated)"
echo -e "        from     : $fromdir"
echo -e "        into     : $insDir"
if [ "$manpages" == "no" ]
then
    echo -e "    WARNING: man pages and documentation will not be generated\n"
fi
if [ "$update" == "no" -a  "$retrieve" == "yes" ]
then
    echo -e "    WARNING: modules to be installed will be removed first"
    echo -e "    from the $SW_PACKAGE/$SW_RELEASE directory. Use '-u' option "
    echo -e "    to only update modules\n"
elif [ "$retrieve" == "yes" ]
then
    echo -e "    WARNING: modules to be installed will be updated in the"
    echo -e "    $SW_PACKAGE/$SW_RELEASE directory. Use '-c' to only compile\n"
    echo -e "    modules.\n"
fi
echo -e "    Press enter to continue or ^C to abort "
read choice

# Create directory from where SCALIB will be installed 
mkdir -p $fromdir
if [ $? != 0 ]
then
    exit 1
fi

# List of SCALIB modules
scalibModules="simcli alx vobs sclsvr sclws sclgui"

# Log file
mkdir -p $fromdir/INSTALL
logfile="$fromdir/INSTALL/sclinsInstall.log"
rm -f $logfile

# If modules have to be retrieved from repository
if [ "$retrieve" == "yes" ]
then
    # Delete modules first
    cd $fromdir
    if [ "$update" == "no" ]
    then
        echo -e "Deleting modules..."
        rm -rf $scalibModules
    fi 

    echo -e "Retrieving modules from repository..."
    cd $fromdir

    # Forging repository URL
    repos="https://svn.jmmc.fr/jmmc-sw/SearchCal"
    if [ "$tag" != "" ]
    then
        repos="$repos/tags/$tag"
    elif [ "$branch" != "" ]
    then
        repos="$repos/branches/$branch"
    else
        repos="$repos/trunk"
    fi

    # Retrieve each module from SVN repository
    for module in $scalibModules
    do
        path="$repos/$module"
        svn co $path > $logfile 2>&1
        if [ $? != 0 ]
        then
            echo -e "\nERROR: 'svn co $path' failed ... \n";
            tail $logfile
            echo -e "See log file '$logfile' for details."
            exit 1;
        fi
    done
fi

# Check all modules are there
for mod in $scalibModules
do
    cd $fromdir
    if [ ! -d $mod ]
    then
        echo -e "\nERROR: '$mod' must be retrieved from repository first ...\n";
        exit 1
    fi
done

# Compile and install them
echo -e "Building modules..."
for mod in $scalibModules; do
    cd $fromdir
    echo -e "    $mod..."
    cd $mod/src 
    if [ $? != 0 ]
    then
        echo -e "\nERROR: 'cd $mod/src' failed ...\n";
        exit 1
    fi
    if [ "$manpages" == "no" ]
    then
        make clean all install  >> $logfile 2>&1
    else
        make clean all man install  >> $logfile 2>&1
    fi
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
