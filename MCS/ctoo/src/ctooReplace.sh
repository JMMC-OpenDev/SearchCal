#! /bin/sh

#*******************************************************************************
# JMMC project
#
# "@(#) $Id: ctooReplace.sh,v 1.6 2007-11-06 05:29:16 mella Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.5  2006/01/09 13:00:46  mella
# Updated documetation
#
# Revision 1.4  2005/01/24 15:47:51  gluck
# Bug correction for log message automatic insertion ($Log: not supported by cvs2svn $
# Bug correction for log message automatic insertion (Revision 1.5  2006/01/09 13:00:46  mella
# Bug correction for log message automatic insertion (Updated documetation
# Bug correction for log message automatic insertion ()
#
# lafrasse  29/07/04    Forked form VLT Software
# lafrasse  11/12/04    Changed documentation from VLT Software style to Doxygen
#
#*******************************************************************************

#/**
# \file
#     ctooReplace - replace strings in files
#
# \synopsis
#     ctooReplace \<old-string\> \<new-string\> \<file1\> [\<file2\> ...]
#
# \details
#       ctooReplace replaces \<old-string\> with \<new-string\> in given files.
#
#   This script returns 0 is everything was OK, otherwise 1 if any error
#   occrured.
#
# \usedfiles
#       All files in the argument list will be read and modified.
#       All originals will be moved to backup files ('~' appended).
#
# \warning
#       For security make a backup of all given files before!
#   Do not give regular expressions for \<old-string> or \<new-string\>. 
#   
#   If your expression does not make any changes, you may have to unescape some
#   special characters.
#   We succeeded to replaced some strings that contains * using \*.
#
# \n
# \ex
#       Replace `foo' occurences by `bar' in all files of the `mod' module:
#
#               cd mod
#               ctooReplace foo bar `find . -type f`
#
# \sa
#   sed(1), find(1)
#
# */

case $# in
    0|1|2) echo $0' (Usage): ctooReplace str1 str2 files' 1>&2; exit 1
esac

# store old and new string prefixes:
left="$1"; right="$2"; shift; shift

# loop over rest of command line arguments (it should be filenames)
# and store modifications in place with a backup file suffixed by ~~
for i
do
    if test -f $i
    then
        echo "ctooReplace: $i"
        sed -i~~ "s@$left@$right@g" $i 
    fi
done

exit 0

# ___oOo___
