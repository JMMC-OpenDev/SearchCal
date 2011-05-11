#! /bin/sh
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
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
