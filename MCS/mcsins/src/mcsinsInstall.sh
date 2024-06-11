#!/bin/bash
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************
# NAME 
# mcsinsInstall - deploy MCS modules
# 
# SYNOPSIS
# mcsinsInstall [-h] [-c] [-u] [-m] [-t tag]|[-r rtag]|[-b branch]
# 
# DESCRIPTION
# This command retreives all the modules belonging to MCS from the SVN
# repository and install them.
#

# Determine the SW package
export SW_PACKAGE=MCS

# Define package name
package="MCS"

# Modules repository
repos="https://svn.jmmc.fr/jmmc-sw/$package"

# List of modules
modules="mkf mcscfg tat ctoo mcs log err misc thrd timlog mth fnd misco env cmd msg sdb evh gwt jmcs jmal modc modcpp modsh modjava"

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
