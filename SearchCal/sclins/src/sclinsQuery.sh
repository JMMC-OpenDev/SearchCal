#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclinsQuery.sh,v 1.2 2005-02-25 13:00:22 scetre Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.1  2005/02/25 11:16:50  scetre
# Added script to star, stop and run Search Calibrators
#
#*******************************************************************************

#/**
# \file
# brief description of the shell script, which ends at this dot.
#
# \synopsis
# \<Command Name\> [\e \<param1\> ... \e \<paramN\>] 
#                     [\e \<option1\> ... \e \<optionN\>] 
#
# \param param1 : description of parameter 1, if it exists
# \param paramN : description of parameter N, if it exists
#
# \n
# \opt
# \optname option1 : description of option 1, if it exists
# \optname optionN : description of option N, if it exists
# 
# \n
# \details
# OPTIONAL detailed description of the shell script follows here.
# 
# \usedfiles
# OPTIONAL. If files are used, for each one, name, and usage description.
# \filename fileName1 :  usage description of fileName1
# \filename fileName2 :  usage description of fileName2
#
# \n
# \env
# OPTIONAL. If needed, environmental variables accessed by the program. For
# each variable, name, and usage description, as below.
# \envvar envVar1 :  usage description of envVar1
# \envvar envVar2 :  usage description of envVar2
# 
# \n
# \warning OPTIONAL. Warning if any (software requirements, ...)
#
# \n
# \ex
# OPTIONAL. Command example if needed
# \n Brief example description.
# \code
# Insert your command example here
# \endcode
#
# \sa OPTIONAL. See also section, in which you can refer other documented
# entities. Doxygen will create the link automatically. For example, 
# \sa \<entity to refer\>
# 
# \bug OPTIONAL. Known bugs list if it exists.
# \bug Bug 1 : bug 1 description
#
# \todo OPTIONAL. Things to forsee list.
# \todo Action 1 : action 1 description
# 
# \n 
# 
# */


# Check arguments
if [ $# -lt 30 ]
then
    if [ $# == 1 ]
    then
        if [ "$1" == "-h" ]
        then
            echo -e "Help ..."
            msgSendCommand sclsvrServer HELP "-command GETCAL"
            exit 1
        fi
    else
        echo -e "Bad parameter, see [-h] option"
        echo -e "Usage: 'sclinsQuery -h'"
        exit 1
    fi
fi

# Start the servers
./sclinsStart.sh 
if [ $? == 0 ]
then
    echo "Failed to start Search Calibrators..." >&2
    exit 1
fi
# test ok

# Check if server sclgui is IDLE, if not exit 
sclguiPanelState=`msgSendCommand sclguiPanel STATE ""` >> /dev/null 2>&1
sclguiPanelSubState=${sclguiPanelState##*/}
if [ "$sclguiPanelSubState" != "IDLE" ]
then
    echo -e "\tsclguiPanel is not $sclguiPanelSubState"
    exit 1
fi

echo -e "Search Calibrators is running..."
# Send Command to sclguiPanel
msgSendCommand sclguiPanel GETCAL "$*" >> /dev/null 2>&1
if [ $? != 0 ]
then
    echo -e "\t Search Calibrators failed."
else
    echo -e "\t Search Calibrators succeed"
fi
#test ok

#___oOo___
