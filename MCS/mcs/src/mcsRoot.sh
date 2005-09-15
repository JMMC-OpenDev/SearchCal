#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: mcsRoot.sh,v 1.3 2005-09-15 06:48:29 swmgr Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.2  2005/03/14 10:37:16  mella
# Check if MCSROOT is a directory
#
# Revision 1.1  2005/03/11 15:44:06  mella
# First revision
#
#*******************************************************************************

#/**
# \file
# This script must be sourced to switch to an other MCSROOT. 
#
# \synopsis
# source mcsRoot
#
# \details
# Different versions can  be founded under the /home/MCS directory.
# A common way to use this script is to do an alias.
# alias setMcsRoot='source /home/MCS/DEVELOPMENT/bin/mcsRoot'
#
# \usedfiles
# \filename /home/MCS/<dirs> : used as list of existing revision of MCSROOT.
#
# \warning
# If this script is not sourced, one new shell is open into the old one.
# */


# signal trap (if any)

DIALOG=dialog
TOPDIR=/home/MCS
P=""
for e in `ls $TOPDIR`
do
    if [ -d $TOPDIR/$e ]
    then
        P="$P $TOPDIR/$e o"  
    fi
done 

$DIALOG --backtitle "Choose one existing MCSROOT" \
        --clear --title  \
        "Choose one MCSROOT under $TOPDIR" \
        --menu \
        "Which one ?"\
        16 70 6 \
        ${P[@]} \
        2> /tmp/menu.tmp.$$

retval=$?

# clear terminal
clear
choice=`cat /tmp/menu.tmp.$$`
rm -f /tmp/menu.tmp.$$

case $retval in
  0)
    export MCSROOT=$choice 
    echo "MCSROOT=$MCSROOT"
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
