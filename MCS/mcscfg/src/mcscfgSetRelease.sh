#!/bin/bash
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
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
# \filename /home/MCS/\<dirs\> : used as list of existing revision of MCSRELEASE.
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
