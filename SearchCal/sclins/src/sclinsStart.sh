#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclinsStart.sh,v 1.2 2005-02-25 15:06:00 scetre Exp $"
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


# Check -h option 
if [ $# == 1 ]
    then  
    if [ "$1" == "-h" ]
    then
        echo -e "Usage: sclinsStart [-h]"
        echo -e "\t-h\tprint this help."
        echo -e "\tThis script start the different servers of the "
        echo -e "\tSearch Calibrator v?.? software :"
        echo -e "\t\t- sclsvrServer"
        echo -e "\t\t- sclguiPanel"
    fi
fi


# Start the environnement
envStart
if [ $? != 0 ]
then
    exit 1
fi

i=0
limit=10
# Ping the server sclsvr. If it is not started, start it
msgSendCommand sclsvrServer PING "" >> /dev/null 2>&1
if [ $? != 0 ]
then 
    sclsvrServer >> /dev/null &
    toto=$?
    while [ "$toto" != "0" -a ""$i" -le "$limit"" ]
    do
        i=`expr $i + 1`
        sclsvrServer >> /dev/null 2>&1 &
        toto=$?
    done
    if [ "$i" -ge "$limit" ]
    then
        echo -e "ERROR: starting 'sclsvrServer' failed" >&2
    fi
    echo -e "'sclsvrServer' started"
else
    echo -e "'sclsvrServer' ALREADY started"
fi


i=0
# Ping the server sclguiPanel. If it is not started, start it
msgSendCommand sclguiPanel PING "" >> /dev/null 2>&1
if [ $? != 0 ]
then 
    sclguiPanel >> /dev/null &
    toto=$?
    while  [ "$toto" != "0" -a ""$i" -le "$limit"" ]
    do
        i=`expr $i + 1`
        sclguiPanel >> /dev/null 2>&1 &
        toto=$?
    done
    if [ "$i" -ge "$limit" ]
    then
        echo -e "\n\t\tERROR: starting 'sclguiPanel' failed\n" >&2
    fi
        echo -e "'sclguiPanel' started"
else
    echo -e "'sclguiPanel' ALREADY started"
fi

exit 0
#___oOo___
