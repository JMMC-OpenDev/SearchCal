#! /bin/sh

#*******************************************************************************
# JMMC project
#
# "@(#) $Id: ctooReplace.sh,v 1.1 2004-09-10 17:40:27 gzins Exp $"
#
# who       when        what
# --------  --------    ------------------------------------------------
# lafrasse  29/07/04    Forked form VLT Software
#
#*******************************************************************************
# NAME
#	ctooReplace - replace strings in files
#
# SYNOPSIS
#	ctooReplace str1 str2 files ...
#
# DESCRIPTION
#	ctooReplace replaces str1 with str2 in given files.
#
# FILES
#	All files in the argument list will be read and modified.
#	All originals will be moved to backup files (`~' appended).
#
# ENVIRONMENT
#
# RETURN VALUES
#	0=OK, 1=Error
#
# CAUTIONS
#	For security make a backup of all given files before!
#	Do not give regular expressions for <old-prefix> or <new-prefix>.
#
# EXAMPLES
#	Replace `foo' occurences by `bar' in all files of the `mod' module:
#
#		cd mod
#		ctooReplace foo bar `find . -type f`
#
# SEE ALSO
#	sed(1), find(1)
#
# BUGS
#
#-------------------------------------------------------------------------------
#

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
