#! /bin/sh

#*******************************************************************************
# JMMC project
#
# "@(#) $Id: ctooRename.sh,v 1.3 2005-01-24 14:28:38 gluck Exp $"
#
# History
# -------
# $>-Log-<$
# lafrasse  29/07/04    Forked form VLT Software
# lafrasse  11/12/04    Changed documentation from VLT Software style to Doxygen
#
#*******************************************************************************

#/**
# \file
#	ctooRename - rename files and identifiers in a module
#
# \synopsis
#	ctooRename \<old-prefix\> \<new-prefix\> \<file1\> [\<file2\> ...]
#
# \details
#	This program can be used to rename a module
#	from  \<old-prefix\> to \<new-prefix\>.
#	Both prefixes must consist of all lower case characters,
#	otherwise the result is undefined.
#	The file list can conveniently be given with the find command.
#
#	It will modify the name of all given files \<fileN\>, as well 
#	as all identifiers in them according to the prefixes.
#
#	First in all given files the identifiers are renamed,
#	considering as much as possible the VLT conventions.
#	For instance:
#
#	    oldmod.h     -->	newmod.h\n
#	    oldmodFoo    -->	newmodFoo\n
#	    oldmodfoo    -->	oldmodfoo	(unchanged!)\n
#	    oldmod_foo   -->	newmod_foo\n
#	    oldmodFOO    -->	newmodFOO\n
#	    OLDMOD_FOO   -->	NEWMOD_FOO\n
#	    OLDMODFOO    -->	OLDMODFOO	(unchanged!)\n
#
#	After that all filenames will be changed, replacing
#	literally \<old-prefix\> by \<new-prefix\>.
#
#	Finally one possibly has to do some manual alignment work.
#
#   This script returns 0 is everything was OK, otherwise 1 if any error
#   occrured.
#
# \usedfiles
#	All files in the argument list will be read and modified.
#	All originals will be moved to backup files ('~' appended).
#
# \warning
#	For security make a backup of all given files before!
#	Do not give regular expressions for <old-prefix> or <new-prefix>.
#
# \n
# \ex
#	Modify all files in the module `foo' for the new name `bar':
#
#		cd foo
#		ctooRename foo bar `find . -type f`
#		cd ..
#		mv foo bar
#
# \sa
#	sed(1), find(1)
#
# */

localpath=`dirname $0`
PATH=/bin:/usr/bin:$localpath

case $# in
    0|1|2) echo $0' (Usage): ctooRename str1 str2 files' 1>&2; exit 1
esac

# Old and new module prefixes:
left="$1"; right="$2"; shift; shift

# The same in upper case:
typeset LEFT=$left; typeset RIGHT=$right

for i 
do 
  if test -f $i 
  then
    echo -e "ctooRename: $i\c"
    sed -e "s|$left\([A-Z_.	]\)|$right\1|g"	$i   > $i~~
    sed -e "s|$LEFT\([_]\)|$RIGHT\1|g"		$i~~ > $i~~~
    mv $i $i~
    mv $i~~~ $i
    rm -f $i~~*

    iDirName=`dirname $i`
    iBaseName=`basename $i`
    iNewName=`echo $iBaseName|sed "s|$left|$right|g"`
    iNewName="${iDirName}/""$iNewName"

    if test "$i" !=  "$iNewName"
    then
    	echo "--> $iNewName"
    	mv $i "$iNewName"
    else
    	echo "-- not moved"
    fi
  fi

done

exit 0

# ___oOo___
