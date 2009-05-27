# $MCSROOT/etc/profile -*- Mode: shell-script -*-
# (c) jmmc
#------------------------------------------------------------------------------
# File:    $MCSROOT/etc/mcs.sh
#
# Version: $Id: mcs.sh,v 1.14 2009-05-27 22:04:58 mella Exp $
#
# Purpose: bash configuration file
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.13  2007/11/19 13:41:31  gzins
# Minor changes
#
# Revision 1.12  2007/03/22 14:10:36  mella
# Add 2 alias
#
# Revision 1.11  2007/03/15 15:24:39  gzins
# Put JAVA_HOME/bin at the first position in PATH
#
# Revision 1.10  2007/03/15 14:59:17  gzins
# Added JAVA_HOME/bin in PATH
#
# Revision 1.9  2007/01/26 14:22:56  lafrasse
# Fixed bug in ipcClean macro
#
# Revision 1.8  2006/04/07 07:01:34  gzins
# Fixed bug in ipcClean macro
#
# Revision 1.7  2006/03/22 19:48:09  gzins
# Fixed 'tat' alias
#
# Revision 1.6  2006/03/22 19:47:41  gzins
# Added 'tat' alias
#
# Revision 1.5  2006/03/06 12:55:53  gzins
# Fixed ipcClean bug
#
# Revision 1.4  2006/03/06 10:47:12  lafrasse
# Added 'ipcClean' function
#
# Revision 1.3  2005/12/14 23:25:37  swmgr
# Fixed wrong setMcsRelease alias
#
# Revision 1.2  2005/12/06 07:47:33  gzins
# Removed mcsShow
#
# Revision 1.1  2005/12/06 06:38:51  gzins
# Moved from mcscfg
#
# Revision 1.10  2005/12/02 14:27:06  swmgr
# replace setMcsRoot by setMcsRelease
#
# Revision 1.9  2005/12/02 14:25:51  gzins
# Changed mcscfgShow to mcsShow
#
# Revision 1.8  2005/12/02 09:59:36  gzins
# Updated for new MCS directory structure; added MCSTOP and MCSRELEASE
#
# Revision 1.7  2005/11/30 13:44:26  gzins
# Updated ctooGetTemplateForCppClass function according to new ctooGetTemplateForCoding interface
#
# Revision 1.6  2005/11/29 08:21:04  mella
# Protect Pu aliases from human error as Pu *
#
# Revision 1.5  2005/03/14 08:32:04  mella
# Add setMcsRoot alias
#
# Revision 1.4  2005/03/07 10:52:53  mella
# Append path to environment variables with individual tests
#
# Revision 1.3  2005/02/15 16:49:11  gzins
# Added psg alias
#
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

# MCSRELEASE
if [ -z "$MCSRELEASE" ]
then
    export MCSRELEASE=DEVELOPMENT
fi

# MCSDATA
export MCSDATA=${MCSTOP}/data

# MCSROOT
export MCSROOT=${MCSTOP}/${MCSRELEASE}

# MCSROOT
if [ -z "$MCSENV" ]
then
    export MCSENV=default
fi

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

# Java
if [ ! -z "$JAVA_HOME" ]
then
    export PATH=$JAVA_HOME/bin:$PATH
fi

# Add $INTROOT to LD_LIBRARY_PATH, PATH and MANPATH
if [ "$INTROOT" != "" ]
then
    if ! echo ${LD_LIBRARY_PATH} |grep -q $INTROOT/lib
    then
        export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$INTROOT/lib"
    fi
    if ! echo ${PATH} |grep -q $INTROOT/bin
    then
        export PATH="$PATH:$INTROOT/bin"
    fi
    if ! echo ${MANPATH} |grep -q $INTROOT/man
    then
        export MANPATH="$MANPATH:$INTROOT/man"
    fi
fi

# Add $MCSROOT to LD_LIBRARY_PATH, PATH and MANPATH
if ! echo ${LD_LIBRARY_PATH} |grep -q $MCSROOT/lib 
then
    export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$MCSROOT/lib"
fi
if ! echo ${PATH} |grep -q $MCSROOT/bin
then
    export PATH="$PATH:$MCSROOT/bin"
fi
if ! echo ${MANPATH} |grep -q $MCSROOT/man 
then
    export MANPATH="$MANPATH:$MCSROOT/man"
fi

# Aliases
alias psg='ps -aef | grep $*'
alias m=more
alias Pu='rm -f *~ .*~ core | echo -en'
alias gvim='gvim -geometry 80x45'
alias givm='gvim -geometry 80x45'
alias maca='make clean all'
alias macam='make clean all man'
alias macami='make clean all man install'
alias setMcsRelease='source $MCSTOP/DEVELOPMENT/bin/mcscfgSetRelease'
alias tat=tatTestDriver

# List of functions
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
        echo ../include/$class | ctooGetTemplateForCoding c++-class-interface-file | grep CREATED
        echo ../src/$class | ctooGetTemplateForCoding c++-class-definition-file | grep CREATED
        echo "Done."
        echo ""
    else
        echo "ctooGetTemplateForCppClass <className>"
        echo ""
    fi
}

# Function to clean shared memories, semaphores, ...
ipcClean ()
{
    ipcs -m | grep $USER | grep 0x00000000 | awk '{ id = $2; cmd = sprintf("ipcrm -m %s",id); system(cmd)}'
    ipcs -s | grep $USER | grep 0x00000000 | awk '{ id = $2; cmd = sprintf("ipcrm -s %s",id); system(cmd)}'
    ipcs -q | grep $USER | grep 0x00000000 | awk '{ id = $2; cmd = sprintf("ipcrm -q %s",id); system(cmd)}'
}

# Function to allow what command onto linux
if [ "$(uname)" == "Linux" ]
then
    # check if it already is associated to something
    type what &> /dev/null   
    if [ $? -ne 0 ]
    then
    what()
    {    
        # try to mimic hpux's what
        # to do support -s option
        files=$*
        oldIFS="$IFS"
        for file in $files
        do
            # check if file is readable
            if [ -r "$file" ]
            then
                # do real job
                echo "${file}:"
                LINES="$(strings $file | grep '@(#)')"
                # change IFS to split according CR only
                IFS=$'\x0A'
                # print every lines
                for line in $LINES
                do
                    tmp="$IFS"
                    iIFS="ai $'\x0A' $'\x3E' "
                    IFS="1"
                    str=($line)
                    echo "    $str"
                    IFS="$tmp"
                done
                IFS="$oldIFS"
            else
                echo "can't open $file (TBD)"
            fi
        done
        IFS="$oldIFS"
    }
    fi
fi

# Fix Darwin pecularities
if [ "$(uname)" == "Darwin" ]
then
    # force -e support on echo command inside Mac OS X 10.5 /bin/sh
    # http://developer.apple.com/releasenotes/Darwin/RN-Unix03Conformance/
    export COMMAND_MODE=legacy
fi
