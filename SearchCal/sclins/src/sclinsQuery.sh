#!/bin/bash
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
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

# Start the servers
sclinsStart >> /dev/null
if [ $? != 0 ]
then
    exit 1
fi

# Check arguments
if [ $# == 1 ]
then
    if [ "$1" == "-h" ]
    then
        msgSendCommand sclsvrServer HELP "-command GETCAL" 
        exit $?
    else
        echo -e "Invalid parameter, see [-h] option"
        exit 1
    fi
fi

# Check if SW is IDLE, if not exit 
state=`msgSendCommand sclguiControl STATE ""` >> /dev/null
if [ $? != 0 ]
then 
    echo "ERROR: could not send command to 'sclguiControl'"  >&2
    exit 1
fi

subState=${state##*/}
if [ "$subState" != "IDLE" ]
then
    echo -e "ERROR: Server is busy. Could not handle your request." 2>&1
    exit 1
fi

# Send Command to sclguiControl
msgSendCommand sclguiControl GETCAL "$*"
if [ $? != 0 ]
then
    echo -e "ERROR: Request failed." 2>&1
    exit 1
else
    echo -e "Request succeed."
    exit 0
fi

#___oOo___
