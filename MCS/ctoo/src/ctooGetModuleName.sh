#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: ctooGetModuleName.sh,v 1.2 2004-09-15 07:03:29 gluck Exp $"
#
# who       when         what
# --------  -----------  -------------------------------------------------------
# gluck     14-Sep-2004  Created
#
#
#*******************************************************************************

#/**
# \file
# Get module name.
#
# \synopsis
# ctooGetModuleName 
#
# \param param1 : description of parameter 1, if it exists
# \param paramn : description of parameter n, if it exists
#
# \n
# \opt
# \optname option1 : description of option 1, if it exists
# \optname optionn : description of option n, if it exists
# 
# \n
# \details
# optional detailed description of the shell script follows here.
# 
# \usedfiles
# optional. if files are used, for each one, name, and usage description.
# \filename filename1 :  usage description of filename1
# \filename filename2 :  usage description of filename2
#
# \n
# \env
# optional. if needed, environmental variables accessed by the program. for
# each variable, name, and usage description, as below.
# \envvar envvar1 :  usage description of envvar1
# \envvar envvar2 :  usage description of envvar2
# 
# \n
# \warning optional. warning if any (software requirements, ...)
#
# \n
# \ex
# optional. command example if needed
# \n brief example description.
# \code
# insert your command example here
# \endcode
#
# \sa optional. see also section, in which you can refer other documented
# entities. doxygen will create the link automatically. for example, 
# \sa <entity to refer>
# 
# \bug optional. known bugs list if it exists.
# \bug bug 1 : bug 1 description
#
# \todo optional. things to forsee list.
# \todo action 1 : action 1 description
# 
# */


# signal trap (if any)


# test doc directory existence
dir=../doc
if [ ! -d $dir ]
then
    echo "error - ctooGetmoduleName: $dir directory does not exist."
    echo "        please check your module directory structure"
    exit 1
fi

# test moduledescription.xml file existence
file=../doc/moduleDescription.xml
if [ ! -f $file ]
then
    echo "error - ctooGetmoduleName: $file file does not exist."
    echo "        please check your module directory structure"
    exit 1
fi
    
# get line containing the name of the module in moduledescription.xml file
# => <module name="modulename">
lineContainingModuleName=`grep "^<module name=\".*\">$" ../doc/moduleDescription.xml`

# trim left the above extracted line => modulename">
rightSideOfModuleName=${lineContainingModuleName##<module name=\"}

# trim right the above extracted string to get module name => modulename
ROOT_NAME=${rightSideOfModuleName%%\">}

# If no module name can be found => error
if [ "$ROOT_NAME" = "" ]
then
    echo "error - ctooGetModuleName: Could not get module name."
    exit 1
else
    # Export module name
    export moduleName=$ROOT_NAME
fi

# return with success
return 0

#___ooo___
