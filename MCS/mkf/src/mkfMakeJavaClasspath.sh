#! /bin/ksh
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: mkfMakeJavaClasspath.sh,v 1.1 2004-09-10 13:40:57 gzins Exp $"
#
# who       when         what
# --------  --------     ----------------------------------------------
# gzins     26-Aug-2004  Adapted from VLT
#

#************************************************************************
#   NAME
#   mkfMakeJavaClasspath - create the makefile to build an executable
# 
#   SYNOPSIS
#
#   mkfMakeJavaClasspath 
# 
#   DESCRIPTION
#   Utility used by mkfMakefile to dynamically generate the
#   CLASSPATH for Java stuff, prior to compilation.
#
#
#   It is not intended to be used as a standalone command,
#   and it must be executed from the module's src directory
#
#   (1) see also GNU Make 3.64, 4.3.5 pag 37
#
#   The rules is written to standard output.
#
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

# signal trap (if any)

#
# compute the value of CLASSPATH, taking
# $INTROOT, $MCSROOT and $MODROOT into account
#
#
mod_list=`/bin/ls -1 ../lib/*.jar 2>/dev/null` 
int_list=`/bin/ls -1 $INTROOT/lib/*.jar 2>/dev/null`
mcs_list=`/bin/ls -1 $MCSROOT/lib/*.jar 2>/dev/null`

int_mod_list=`echo $mod_list`

for file in `echo $int_list`
do
    file_name=`basename $file` 
    echo $mod_list | grep "\/$file_name" 1>/dev/null
    if [ $? != 0 ]
    then
        int_mod_list=`echo $file; echo $int_mod_list`
    fi
done

mcs_list=`echo $int_mod_list`
for file in `echo $mcs_list`
do
    file_name=`basename $file`
    echo $int_mod_list | grep "\/$file_name" 1>/dev/null
    if [ $? != 0 ]
    then
        mcs_list=`echo $file; echo $mcs_list`
    fi
done

tot_list=`echo $mcs_list`

#echo "Generating classpath for following JARs: "
for file in $tot_list
do
    export CLASSPATH=$file:$CLASSPATH
done

echo $CLASSPATH

#
# ___oOo___
