#! /bin/sh
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: mkfMakeScript.sh,v 1.1 2004-09-10 13:40:57 gzins Exp $" 
#
# who       when         what
# --------  --------     ----------------------------------------------
# gzins     26-Aug-2004  Adapted from VLT

#************************************************************************
#   NAME
#   mkfMakeScript - create a script
# 
#   SYNOPSIS
#
#   mkfMakeScript <exeName>
# 
#   DESCRIPTION
#   Utility used by mkfMakefile to copy a script from the local directory to
#   ../bin. The name of the script file in the local directory must be
#   <exeName> with one of the one of these following extensions :
#       .sh   : for shell scripts,
#       .py   : for Python scripts.
#
#   If the script is a /bin/sh script, then an instruction to disable the
#   build-in echo command under Linux is added to the script.
#
#   <exeName>     The name of the  executable. The output is named 
#                 ../bin/<exeName>
#
#   FILES
#   $MCSROOT/include/mkfMakefile   
#
#   ENVIRONMENT
#
#   RETURN VALUES
#
#   SEE ALSO 
#   mkfMakefile, Makefile, (GNU) make
#
#   BUGS    
#
#----------------------------------------------------------------------

if [ $# -ne 1 ]
then
    echo "" >&2
    echo "mkfMakeScript <exeName>" >&2
    echo "" >&2
    exit 1
fi

#
# set up more readable variables:
exeName=$1

OUTPUT=../bin/${exeName}

# Look up for the source file
# Shell programs (.sh) or python program (.py)
extList=".sh .py"
scriptFile=""
for ext in $extList
do 
    if [ -f ${exeName}${ext} ]
    then
        scriptFile="${exeName}${ext}"
    fi
done 

if [ "X$scriptFile" == "X" ]
then
    echo "No file found for script '${exeName}'" 
    exit 1
fi

# add the header!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
if [ "`head -n 1 ${scriptFile} | grep '^#! */bin/sh'`" != "" -a \
    "`head -n 2 ${scriptFile} | grep '\\\\$'`" = "" ]
then
    head -n 1 ${scriptFile} >$OUTPUT
    echo 'if [ "`uname`" = "Linux" ]; then enable -n echo; fi' >>$OUTPUT
    sed 1d ${scriptFile} >>$OUTPUT
else
    # not a /bin/sh script: just copy the script
    cp ${scriptFile} $OUTPUT
fi
#
# make output file executable
chmod +wx $OUTPUT

#
# ___oOo___
