#! /bin/sh
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************
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

# signal trap (if any)

#
# compute the value of CLASSPATH, taking
# $INTROOT, $MCSROOT and $MODROOT into account
#
#
mod_list=`/bin/ls -1 ../lib/*.jar 2>/dev/null` 
int_list=`/bin/ls -1 $INTROOT/lib/*.jar 2>/dev/null`
mcs_list=`/bin/ls -1 $MCSROOT/lib/*.jar 2>/dev/null`

### echo ==== MOD ==== 
### echo $mod_list
### echo ==== INT ==== 
### echo $int_list
### echo ==== MCS ==== 
### echo $mcs_list

#
# The first step is to put in the list
# all jar files in <MODROOT>/lib
#
### echo 1
for file in $mod_list
do
  complete_jar_list="$complete_jar_list:$file"
  ### echo 1.1 $complete_jar_list  
done

#
# Now we add the files in $INTROOT/lib (i.e. $int_list)
# that are not already in $complete_jar_list
#
### echo 2
for file in $int_list
do
    ### echo 2.1 $file
    file_name=`basename $file` 
    echo $complete_jar_list | grep "\/$file_name" 1>/dev/null
    echo $complete_jar_list | grep `basename $file` 1>/dev/null
    if [ $? != 0 ]
    then
        complete_jar_list="$complete_jar_list:$file"
        ### echo 2.1.1 $complete_jar_list  
    fi
done

#
# Now we add the files in $ACSROOT/lib (i.e. $mcs_list)
# that are not already in $complete_jar_list
#
### echo 3
for file in $mcs_list
do
###     echo 3.1 $file
###     echo 3.1.1
    echo $complete_jar_list | grep `basename $file` 1>/dev/null
    if [ $? != 0 ]
    then
###         echo 3.1.2
        complete_jar_list="$complete_jar_list:$file"
###         echo 3.1.3 $complete_jar_list  
    fi
done

### echo 4

###echo "Generating classpath for following JARs: "
export CLASSPATH=$complete_jar_list:$CLASSPATH
### echo 5

echo $CLASSPATH

#
# ___oOo___
