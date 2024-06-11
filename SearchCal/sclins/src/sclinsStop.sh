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

# For each process of search calibrators software
for process in sclsvrServer sclguiControl
do
    # If proces is running 
    msgSendCommand $process PING "" >> /dev/null 2>&1
    if [ $? == 0 ]
    then 
        # Send EXIT command
        msgSendCommand $process EXIT "" >> /dev/null
        if [ $? == 0 ]
        then 
            echo "'$process' stopped"
        else
            echo "ERROR: could not stop '$process'"  >&2
        fi
    else
        echo "'$process' ALREADY stopped"
    fi
done

exit 0

#___oOo___
