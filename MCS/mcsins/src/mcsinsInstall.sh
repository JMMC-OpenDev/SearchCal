#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: mcsinsInstall.sh,v 1.25 2010-10-22 09:24:32 lafrasse Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.24  2009/05/27 21:32:59  mella
# fix error messages
#
# Revision 1.23  2009/05/27 21:31:08  mella
# fix error messages
#
# Revision 1.22  2008/12/17 10:32:53  ccmgr
# Add option to remotly tag the list of modules
#
# Revision 1.21  2008/09/22 14:04:13  ccmgr
# Added 'jmcs' module installation.
#
# Revision 1.20  2006/10/14 10:39:24  gzins
# Added again mcscfg module
#
# Revision 1.19  2006/09/26 07:26:10  gzins
# Supressed mcscfg from MCS module list
#
# Revision 1.18  2006/03/23 07:26:41  swmgr
# Added modjava module
#
# Revision 1.17  2006/03/23 07:11:24  swmgr
# Added tat module
#
# Revision 1.16  2006/02/22 10:46:20  gzins
# Added MCSRELEASE setting
#
# Revision 1.15  2006/02/20 12:54:34  swmgr
# Added mth module
# Install SW from MCS sub-directory
#
# Revision 1.14  2005/12/14 22:17:55  gzins
# Added thrd and sdb modules
#
# Revision 1.13  2005/12/06 08:52:38  gzins
# Source .bash_profile when specifying tag
#
# Revision 1.12  2005/12/06 07:15:56  gzins
# Added mcscfg module
#
# Revision 1.11  2005/12/02 09:51:56  gzins
# Updated for new MCS directory structure
#
# Revision 1.10  2005/09/15 07:07:08  swmgr
# Add revision to given informations
#
# Revision 1.9  2005/09/14 22:05:13  gzins
# Improved checks
#
# Revision 1.8  2005/05/13 15:33:41  gzins
# Added -c and -t options
# Checked $HOME and $MCSROOT differs
#
# Revision 1.7  2005/02/11 09:45:11  gzins
# Added installation of misco
#
# Revision 1.6  2005/01/29 13:49:36  gzins
# Added CVS log as modification history
# Forbid MCS installation as root
#
# gzins     04-Dec-2004  Created
# gzins     08-Dec-2004  Moved from mkf module
#                        Added installation of env module
# gzins     08-Dec-2004  Added installation of gwt module
# gzins     09-Dec-2004  Renamed to mcsinsInstall
# gzins     09-Dec-2004  Added '-h' and '-u' options
#                        Added logfile
#                        Deleted modules before retrieving from repository
# gzins     17-Dec-2004  Added installation of timlog module
#                        Added 'clean' as first target of the make command 
#
#*******************************************************************************
#   NAME 
#   mcsinsInstall - deploy MCS modules
# 
#   SYNOPSIS
#   mcsinsInstall [-h] [-c] [-u] [-m] [-t tag]|[-r rtag]|[-b branch]
# 
#   DESCRIPTION
#   This command retreives all the modules belonging to MCS from the SVN
#   repository and install them.
#-------------------------------------------------------------------------------
#

# Determine the SW package
export SW_PACKAGE=MCS

# Define package name
package="MCS"

# Modules repository
repos="https://svn/jmmc.fr/jmmc-sw/$package"

# List of modules
modules="mkf mcscfg tat ctoo mcs log err misc thrd timlog mth fnd misco env cmd msg sdb evh gwt jmcs modc modcpp modsh modjava"

# Print script usage and exits with an error code
function printUsage () {
    scriptName=`basename $0 .sh`
    echo -e "Usage: $scriptName [-h] [-c] [-u] [-m] [-t tag]|[-r rtag]|[-b branch]"
    echo -e "\t-h\tPrint this help."
    echo -e "\t-c\tOnly compile; i.e. do not retrieve modules from "
    echo -e "\t\trepository."
    echo -e "\t-u\tDo not delete modules to be installed from the "
    echo -e "\t\tcurrent directory; they are just updated."
    echo -e "\t-m\tDo not create man pages."
    echo -e "\t-b brch\tUse 'brch' branch when retrieving modules."
    echo -e "\t-t tag\tUse revision 'tag' when retrieving modules."
    echo -e "\t-r rtag\tTag head repository version with 'rtag'."
    echo
    exit 1;
}

# Parse command-line parameters
update="no";
retrieve="yes";
manpages="yes";
tag="";
rtag="";
branch=""
while getopts "chumt:r:b:" option
# Initial declaration.
# c, h, u, m, t, b, r are the options (flags) expected.
# The : after options shows it will have an argument passed with it.
do
  case $option in
    c ) # Update option
        retrieve="no";;
    h ) # Help option
        printUsage ;;
    u ) # Update option
        update="yes";;
    m ) # No man pages creation
        manpages="no";;
    t ) # Tag option
        tag="$OPTARG";;
    r ) # Rtag option
        rtag="$OPTARG";;
    b ) # Branch option
        branch="$OPTARG";;
    * ) # Unknown option
        printUsage ;;
    esac
done

# Check that all options have been parsed 
if [ $# -ge $OPTIND ]
then 
    printUsage
fi

# Tag the package trunk
if [ "$rtag" != "" ]
then
	echo "Tagging Subversion trunk with tag = '$rtag' :"
	echo -e "    Press enter to continue or ^C to abort "
	read choice
	svn cp "$repos/trunk" "$repos/tags/$rtag" -m "Tagged $package trunk as '$rtag'."
	exit
fi

# Check that the script is not run by 'root'
if [ `whoami` == "root" ]
then
    echo -e "\nERROR : $package installation MUST NOT BE done as root !!" 
    echo -e "\n  ->  Please log in as swmgr, and start again.\n" 
    exit 1
fi

# Check that MCSTOP is defined
if [ "$MCSTOP" == "" ]
then
    echo -e "\nWARNING : MCSTOP must be defined!!"
    echo -e ""
    exit 1
fi

# Check that MCSDATA is defined
if [ "$MCSDATA" == "" ]
then
    echo -e "\nWARNING : MCSDATA must be defined (you may have forgot to source ~/.bash_profile)!!"
    echo -e ""
    exit 1
fi

# Determine the package release
if [ "$tag" != "" ]
then
    export SW_RELEASE=$tag
elif [ "$branch" != "" ]
then
    export SW_RELEASE=$branch
else
    export SW_RELEASE=DEVELOPMENT
fi
export MCSRELEASE=$SW_RELEASE

# Get intallation directory
if [ "$INTROOT" != "" ]
then
    insDirName="INTROOT"
    insDir=$INTROOT
else
    insDirName="MCSTOP"
    insDir=$MCSTOP/$SW_RELEASE
    # Source bash profile to set path according to MCSRELEASE
    source ~/.bash_profile
fi

# Check that the installation directory differs from home directory 
if [ $HOME == $insDir ]
then
    echo -e "\nWARNING : $insDirName (installation directory) should differ from '`whoami`' home directory !!"
    echo -e ""
    exit 1
fi

# Set directory from where package will be installed 
fromdir=$PWD/$SW_PACKAGE/$SW_RELEASE

# Display informations
echo -e "\n-> All the $package modules will be installed"
echo -e "        from     : $fromdir"
echo -e "        into     : $insDir"
if [ "$tag" != "" ]
then
    echo -e "        tagged revision : $tag\n"
elif [ "$branch" != "" ]
then
    echo -e "        branched revision : $branch\n"
else
    echo -e "        trunk revision : last version (DEVELOPMENT)\n"
fi
if [ "$manpages" == "no" ]
then
    echo -e "    WARNING: man pages and documentation will not be generated."
fi
if [ "$update" == "no" -a  "$retrieve" == "yes" ]
then
    echo -e "    WARNING: modules to be installed will be removed first"
    echo -e "    from the $SW_PACKAGE/$SW_RELEASE directory."
    echo -e "    Use '-u' option to only update modules."
elif [ "$retrieve" == "yes" ]
then
    echo -e "    WARNING: modules to be installed will be updated in the"
    echo -e "    $SW_PACKAGE/$SW_RELEASE directory."
fi
echo -e "    Use '-c' to only compile modules.\n"

# Propose the user to continue or abort
echo -e "    Press enter to continue or ^C to abort "
read choice

# Create directory in which everything will be installed 
mkdir -p $fromdir
if [ $? != 0 ]
then
    exit 1
fi

# Log file
mkdir -p $fromdir/INSTALL
logfile="$fromdir/INSTALL/packageInstall.log"
rm -f $logfile

# If modules have to be retrieved from repository
if [ "$retrieve" == "yes" ]
then
    # Delete modules first
    cd $fromdir
    if [ "$update" == "no" ]
    then
        echo -e "Deleting modules..."
        rm -rf $modules
    fi 

    echo -e "Retrieving modules from repository..."
    cd $fromdir

    # Forging repository URL
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
    svn co $repos ./ > $logfile 2>&1
    if [ $? != 0 ]
    then
        echo -e "\nERROR: 'svn co $repos' failed ... \n";
        tail $logfile
        echo -e "See log file '$logfile' for details."
        exit 1;
    fi
fi

# Check all modules are there
for mod in $modules
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
for mod in $modules; do
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
        echo -e "\nERROR: 'make clean all man install' in $mod failed ...\n";
        tail $logfile
        echo -e "See log file '$logfile' for details."
        exit 1
    fi
done

echo -e "Installation done."
echo -e "See log file '$logfile' for details."
#___oOo___
