#! /bin/sh

#*******************************************************************************
# JMMC project
#
# "@(#) $Id: ctooReplace.sh,v 1.4 2005-01-24 15:47:51 gluck Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
# lafrasse  29/07/04    Forked form VLT Software
# lafrasse  11/12/04    Changed documentation from VLT Software style to Doxygen
#
#*******************************************************************************

#/**
# \file
#	ctooReplace - replace strings in files
#
# \synopsis
#	ctooReplace \<old-string\> \<new-string\> \<file1\> [\<file2\> ...]
#
# \details
#	ctooReplace replaces \<old-string\> with \<new-string\> in given files.
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
#	Do not give regular expressions for \<old-prefix\> or \<new-prefix\>.
#
# \n
# \ex
#	Replace `foo' occurences by `bar' in all files of the `mod' module:
#
#		cd mod
#		ctooReplace foo bar `find . -type f`
#
# \sa
#	sed(1), find(1)
#
# */

localpath=`dirname $0`
PATH=/bin:/usr/bin:$localpath

case $# in
    0|1|2) echo $0' (Usage): ctooReplace str1 str2 files' 1>&2; exit 1
esac

# Old and new module prefixes:
left="$1"; right="$2"; shift; shift

for i 
do 
    if test -f $i 
    then
        echo "ctooReplace: $i"
        sed "s@$left@$right@g" $i > $i~~
        mv $i $i~
        mv $i~~ $i
    fi
done

exit 0

# ___oOo___
