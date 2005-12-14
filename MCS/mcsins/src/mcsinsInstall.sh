#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: mcsinsInstall.sh,v 1.14 2005-12-14 22:17:55 gzins Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
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
#   mcsinsInstall - Install/Update MCS modules 
# 
#   SYNOPSIS
#   mcsinsInstall [-h] [-c] [-u] [-t tag]
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
function printUsage () {
        echo -e "Usage: mcsinsInstall [-h] [-c] [-u] [-t tag]" 
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
    echo -e "\nUsage: mcsinsInstall [-c] [-h] [-u] [-t tag]" 
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

# Check that MCSTOP is defined
if [ "$MCSTOP" == "" ]
then
    echo -e "\nWARNING : MCSTOP must be not defined!!"
    echo -e ""
    exit 1
fi

# Check that MCSDATA is defined
if [ "$MCSDATA" == "" ]
then
    echo -e "\nWARNING : MCSDATA must be not defined!!"
    echo -e ""
    exit 1
fi

# Check that MCS configuration file is installed
if [ ! -f $MCSTOP/etc/mcs.sh ]
then
    echo -e "\nWARNING : MCS configuration files not installed!!"
    echo -e "Install mcscfg module first, and restart MCS installation!!"
    echo -e ""
    exit 1
fi

# Determine the MCS release
if [ "$tag" != "" ]
then
    export MCSRELEASE=$tag
else
    export MCSRELEASE=DEVELOPMENT
fi

# Get intallation directory
if [ "$INTROOT" != "" ]
then
    insDirName="INTROOT"
    insDir=$INTROOT
else
    insDirName="MCSTOP"
    insDir=$MCSTOP/$MCSRELEASE
    # Source bash profile to set path 
    source ~/.bash_profile
fi

#
# Check that the home directory differs from installation directory 
if [ $HOME == $insDir ]
then
    echo -e "\nWARNING : $insDirName (installation directory) should differ from '`whoami`' home directory !!"
    echo -e ""
    exit 1
fi

# Create directory from where MCS will be installed 
fromdir=$PWD/$MCSRELEASE
mkdir -p $fromdir

# Display informations
echo -e "\n-> All the MCS modules will be installed"
echo -e "        from     : $fromdir"
echo -e "        into     : $insDir"
if [ -z "$tag" ]
then
    echo -e "        revision : last version (DEVELOPMENT)\n"
else
    echo -e "        revision : $tag\n"
fi
	    
# Propose the user to continue or abort
if [ "$update" == "no" -a  "$retrieve" == "yes" ]
then
    echo -e "    WARNING: modules to be installed will be removed first"
    echo -e "    from the $MCSRELEASE directory. Use '-u' option to only "
    echo -e "    update modules or '-c' to only compile modules.\n"
elif [ "$retrieve" == "yes" ]
then
    echo -e "    WARNING: modules to be installed will be updated in the"
    echo -e "    $MCSRELEASE directory. Use '-c' to only compile modules.\n"
fi
echo -e "    Press enter to continue or ^C to abort "
read choice

# List of MCS modules
mcsModules="mkf mcscfg ctoo mcs log err misc thrd timlog modc modcpp fnd misco env cmd msg sdb evh gwt"

# Log file
mkdir -p $fromdir/INSTALL
logfile="$fromdir/INSTALL/mcsinsInstall.log"
rm -f $logfile

# If modules have to be retrieved from repository; check repository
if [ "$retrieve" == "yes" ]
then
    if [ "$CVSROOT" == "" ]
    then
        echo -e "\nERROR: 'CVSROOT' must be set ...\n";
        exit 1;
    fi
fi

# If modules have to be retrieved from repository
if [ "$retrieve" == "yes" ]
then
    # Delete modules first
    cd $fromdir
    if [ "$update" == "no" ]
    then
        echo -e "Deleting modules..."
        rm -rf $mcsModules
    fi 

    # Retrieve modules from CVS repository
    # When a revision tag is specified, we have first to retrieve module giving
    # this tag, and then to retrieve again to create empty directories which are
    # not created by cvs command when '-r' option is used.
    echo -e "Retrieving modules from repository..."
    cd $fromdir
    if [ "$tag" != "" ]
    then
        cvs co -r $tag $mcsModules > $logfile 2>&1
        if [ $? != 0 ]
        then
            echo -e "\nERROR: 'cvs co -r $tag $mcsModules' failed ... \n"; 
            tail $logfile
            echo -e "See log file '$logfile' for details."
            exit 1;
        fi
    fi

    cvs co $mcsModules > $logfile 2>&1
    if [ $? != 0 ]
    then
        echo -e "\nERROR: 'cvs co $mcsModules' failed ... \n"; 
        tail $logfile
        echo -e "See log file '$logfile' for details."
        exit 1;
    fi
fi

# Check all modules are there
for mod in $mcsModules; do
    cd $fromdir
    if [ ! -d $mod ]
    then
        echo -e "\nERROR: '$mod' must be retrieved from repository first ...\n";
        exit 1
    fi
done

# Compile and install them
echo -e "Building modules..."
for mod in $mcsModules; do
    cd $fromdir
    echo -e "    $mod..."
    cd $mod/src 
    if [ $? != 0 ]
    then
        echo -e "\nERROR: 'cd $mod/src' failed ...\n";
        exit 1
    fi
    make clean all man install >> $logfile 2>&1
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
