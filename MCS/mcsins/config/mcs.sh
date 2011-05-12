#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************
# $MCSROOT/etc/profile -*- Mode: shell-script -*-
# Purpose: BASH configuration file

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
    export MANPATH="/usr/man:/usr/share/man:/usr/local/man:/usr/local/share/man:/usr/X11R6/man:../man"
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
