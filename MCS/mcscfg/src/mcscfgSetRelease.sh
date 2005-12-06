#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: mcscfgSetRelease.sh,v 1.1 2005-12-06 07:56:19 gzins Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.3  2005/12/02 15:04:37  mella
# Finish to make good parsing
#
# Revision 1.2  2005/12/02 14:55:57  mella
# Just use last meber of path for mcsRelease
#
# Revision 1.1  2005/12/02 14:19:42  swmgr
# Replace old mcsRoot
#
# Revision 1.3  2005/09/15 06:48:29  swmgr
# Make change retrieve JAVA into the Path and retrieve good setup as real login
#
# Revision 1.2  2005/03/14 10:37:16  mella
# Check if MCSROOT is a directory
#
# Revision 1.1  2005/03/11 15:44:06  mella
# First revision
#
#*******************************************************************************

#/**
# \file
# This script must be sourced to switch to an other MCSRELEASE. 
#
# \synopsis
# mcscfgSetRelease
#
# \details
# Different versions can  be founded under the /home/MCS directory.
# A common way to use this script is to do an alias.
# setMcsRelease='source /home/MCS/DEVELOPMENT/bin/mcscfgSetRelease'
#
# \usedfiles
# \filename /home/MCS/<dirs> : used as list of existing revision of MCSRELEASE.
#
# \warning
# If this script is not sourced, one new shell is open into the old one.
# */


# signal trap (if any)

DIALOG=dialog
TOPDIR=$MCSTOP
RELEASE_LIST=""
for dir in `ls $TOPDIR`
do
    if [ -e $TOPDIR/$dir/include/mcs.h ]
    then
        RELEASE_LIST="$RELEASE_LIST $TOPDIR/$dir o"  
    fi
done 

$DIALOG --backtitle "Choose one existing MCSRELEASE" \
        --clear --title  \
        "Choose one MCSRELEASE under $TOPDIR" \
        --menu \
        "Which one ?"\
        16 70 6 \
        ${RELEASE_LIST[@]} \
        2> /tmp/menu.tmp.$$

retval=$?

# clear terminal
clear
choice=`cat /tmp/menu.tmp.$$`
rm -f /tmp/menu.tmp.$$

case $retval in
  0)
    tmp="${choice%*/}"
    export MCSRELEASE=${tmp##*/} 
    echo "MCSRELEASE=$MCSRELEASE" 
    unset PATH
    unset JAVA_HOME
    unset LD_LIBRARY_PATH
    unset MAN_PATH
    exec /bin/bash --login
    ;;
  1)
    echo "Cancel pressed.";;
  255)
    echo "ESC pressed.";;
esac

#___oOo___
