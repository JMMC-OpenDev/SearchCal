# $MCSROOT/etc/profile -*- Mode: shell-script -*-
# (c) jmmc
#------------------------------------------------------------------------------
# File:    $MCSROOT/etc/mcs.sh
#
# Version: $Id: mcs.sh,v 1.3 2005-02-15 16:49:11 gzins Exp $
#
# Purpose: bash configuration file
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.2  2005/02/14 14:20:30  gzins
# Updated test for conditional environment variable setting; test LD_LIBRARY_PATH instead of PATH
#
# Revision 1.1  2005/01/29 13:11:44  gzins
# Renamed bashrc to mcs.sh
#
# gzins     14-05-2004  created
# gzins     10-11-2004  added MCSDATA definition
#
# To use it, added the following line in ~/.bash_profile
#   # Set MCS environment
#   export INTROOT=$HOME/INTROOT
#   export MCSROOT=/home/MCS
#   if [ -f $MCSROOT/etc/mcs.sh ]; then
#           . $MCSROOT/etc/mcs.sh
#   fi
#
#------------------------------------------------------------------------------

# Set LD_LIBRARY_PATH (path for dynamic linked libraries)
if [ "$LD_LIBRARY_PATH" != "" ]
then
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../lib
else
    export LD_LIBRARY_PATH=../lib
fi

# Set MANPATH
if [ "$MANPATH" != "" ]
then
    export MANPATH=$MANPATH:../man
else
    export MANPATH=../man
fi

# Set PATH
if [ "$PATH" != "" ]
then
    export PATH=$PATH:../bin
else
    export PATH=../bin
fi

# Add $INTROOT to LD_LIBRARY_PATH, PATH, MCS_XSLPATH and MANPATH
if [ "$INTROOT" != "" ]
then
    if ! echo ${LD_LIBRARY_PATH} |grep -q $INTROOT
    then
        export PATH="$PATH:$INTROOT/bin"
        export MANPATH="$MANPATH:$INTROOT/man"
        export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$INTROOT/lib
    fi
fi

# Add $MCSROOT to LD_LIBRARY_PATH, PATH, MCS_XSLPATH and MANPATH
if ! echo ${LD_LIBRARY_PATH} |grep -q $MCSROOT 
then
    export PATH="$PATH:$MCSROOT/bin"
    export MANPATH="$MANPATH:$MCSROOT/man"
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MCSROOT/lib
fi

# MCSDATA
export MCSDATA=${MCSROOT}/data

# Aliases
alias psg='ps -aef | grep $*'
alias m=more
alias Pu='rm -f *~ .*~ core'
alias gvim='gvim -geometry 80x45'
alias macam='make clean all man'
alias macami='make clean all man install'

# Functions
# Function to create .h and .cpp files for a C++ class in the right
# directories
ctooGetTemplateForCppClass ()
{
    class=$1;
    class=${class%.*};
    class=`basename "$class"`
    if [ "$class" != "" ]
    then
        echo "Creating source files for $class C++ class ..."
        echo ../include/$class | ctooGetTemplateForCoding c++-h-file | grep CREATED
        echo ../src/$class | ctooGetTemplateForCoding c++-class-file | grep CREATED
        echo "Done."
        echo ""
    else
        echo "ctooGetTemplateForCppClass <className>"
        echo ""
    fi
}

