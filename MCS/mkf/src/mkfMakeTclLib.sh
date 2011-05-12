#! /bin/sh
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************
#   NAME
#   mkfMakeTclLib - create a Tcl/Tk procedure library
# 
#   SYNOPSIS
#
#   mkfMakeTclLib <tclChecker> <libName> <objectList> 
# 
#   DESCRIPTION
#   Utility used by mkfMakefile to create an library of Tcl/Tk procedures.
#   It is not intended to be used as a standalone command.
#
#   An Tcl/Tk procedure is obtained starting from one or more tcl/tk 
#   script files in the src/ directory, copying them into a separate
#   directory called ../lib/lib<libName>.tcl and generating the 
#   tclIndex by using the auto_mkindex command. The use of auto_mkindex
#   allows ro treat both normal tcl and [incr Tcl] libraries.
#
#   (1) see also Tcl/Tk manual, 13.7 Autoloading, pag.138.
#
#
#   <tclChecker>  the program to be used as syntax checek for tcl files
#
#   <libName>     The name of the library directory. The output is named
#                 ../lib/lib<libName>.tcl
#
#   <objectList>  The list of the script files in the src/ directory.
#                 (Without neither directory nor .a suffix)
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

if [ $# -ne 3 ]
then
    echo "" >&2
    echo "mkfMakeTclLib <tclChecker> <libName> <objectList>" >&2
    echo "" >&2
    exit 1
fi

#
# set up more readable variables:
tclChecker=$1
libName=$2
objectList=$3

LIBRARY=../lib/lib${libName}.tcl

rm -rf $LIBRARY

mkdir $LIBRARY


for member in ${objectList}
do
    #
    # run the tcl checker on each tcl-file
    $tclChecker ${member}.tcl 1>&2

    #
    # copy it into the library directory
    cp ${member}.tcl $LIBRARY
done

# We need to determine the Tcl-shell to use for the tclIndex generation.
# itclsh does no longer exist for Tcl > 8.0; use tclsh in this case (only).
# Remark that arithmetic expressions ("-le" etc) only work on integers; 
# let's therefore do the arithmetic work in Tcl itself.
# This can be hardcoded once we're sure this version of the "vlt" module
# will only be used with a Tcl-versions > 8.0.
TCL_SHELL=`echo 'if {$tcl_version > 8.0} {puts tclsh} else {puts itclsh}' | tclsh`

echo "auto_mkindex $LIBRARY *.tcl; exit" | $TCL_SHELL

chmod -R 775 $LIBRARY

echo "           $LIBRARY  created"
#
# ___oOo___
