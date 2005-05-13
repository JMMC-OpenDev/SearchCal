#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: mcsinsInstall.sh,v 1.8 2005-05-13 15:33:41 gzins Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
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
# a, b, c, d, e, f, and g are the options (flags) expected.
# The : after option 'e' shows it will have an argument passed with it.
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
    echo -e "\nUsage: mcsinsInstall [-h] [-u] [-t tag]" 
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

#
# Check that the home directory differs form MCSROOT 
if [ $HOME == $MCSROOT ]
then
    echo -e "\nWARNING : MCSROOT should differ from '`whoami`' home directory !!"
    echo -e ""
    exit 1
fi

# Get the current directory
dir=$PWD

# Propose the user to continue or abort
echo -e "\n-> All the MCS modules will be installed from"
echo -e "   '$dir' directory\n"
if [ "$update" == "no" -a  "$retrieve" == "yes" ]
then
    echo -e "    WARNING: modules to be installed will be removed first"
    echo -e "    from the current directory. Use '-u' option to only "
    echo -e "    update modules\n"
fi
echo -e "    Press enter to continue or ^C to abort "
read choice

# List of MCS modules
mcs_modules="mkf ctoo mcs log err misc timlog modc modcpp fnd misco env cmd msg evh gwt"

# Log file
mkdir -p INSTALL
logfile="$dir/INSTALL/mcsinsInstall.log"
rm -f $logfile

# If modules have to be retrieved from repository
if [ "$retrieve" == "yes" ]
then
    # Delete modules first
    cd $dir
    if [ "$update" == "no" ]
    then
        echo -e "Deleting modules..."
        rm -rf $mcs_modules
    fi 

    # Retrieve modules from CVS repository
    # When a revision tag is specified, we have first to retrieve module giving
    # this tag, and then to retrieve again to create empty directories which are
    # not created by cvs command when '-r' option is used.
    echo -e "Retrieving modules from repository..."
    cd $dir
    if [ "$tag" != "" ]
    then
        cvs co -r $tag $mcs_modules > $logfile 2>&1
        if [ $? != 0 ]
        then
            echo -e "\nERROR: 'cvs co -r $tag $mcs_modules' failed ... \n"; 
            tail $logfile
            echo -e "See log file '$logfile' for details."
            exit 1;
        fi
    fi

    cvs co $mcs_modules > $logfile 2>&1
    if [ $? != 0 ]
    then
        echo -e "\nERROR: 'cvs co $mcs_modules' failed ... \n"; 
        tail $logfile
        echo -e "See log file '$logfile' for details."
        exit 1;
    fi
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
