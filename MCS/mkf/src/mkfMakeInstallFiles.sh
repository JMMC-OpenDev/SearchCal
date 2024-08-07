#!/bin/bash
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************
#   NAME
#   mkfMakeInstallFiles - copy the files into target area.
# 
#   SYNOPSIS
#
#   mkfMakeInstallFiles <fileList> <DEST_ROOT> <protectionMask>
#
# 
#   DESCRIPTION
#   Utility used by mkfMakefile to generate the mkfMakefile.install section
#   in charge to copy the files into target area.
#   For every file in <fileList>:
#
#      - get "name" and "dir", i.e.: the filename and the parent directory
#
#      - the rule to copy the file into DEST_ROOT/<dir> is generated. As for any
#        installed file, the rule is executed only if the file is newer. The
#        protection mask is applied to leave the file overwritable by the next
#        installation
#
#
#   It is not intended to be used as a standalone command.
#
#   <fileList>        file to be copied
#   <DEST_ROOT>       starting point for bin, include, etc (MCS|INT-ROOT)
#   <protectionMask>  how to set the protection of created file
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
#   mkfMakefile
#
#   BUGS    
#

if [ $# != 3 ]
then
    echo "" >&2
    echo " ERROR:  mkfMakeInstallFiles: $*" >&2
    echo " Usage:  mkfMakeInstallFiles <fileList> <DEST_ROOT> <protectionMask>" >&2
    echo "" >&2
    exit 1
fi

fileList=${1}
DEST_ROOT=$2
MASK=$3

if [ ! -d $DEST_ROOT ]
then 
    echo "" >&2
    echo " ERROR: mkfMakeInstallFiles: " >&2
    echo "          >>$DEST_ROOT<< not a valid directory " >&2
    echo "" >&2
    exit 1
fi

#
# according to the file currently under ERRORS, if any, produce
# the needed targets:

if [ "$fileList" != "" ]
then     
    target="install_files: files_begin "

    echo -e "files_begin:"
    echo -e "\t-@echo \"\"; echo \"..other files:\""

    for FILE in $fileList
    do
        if [ ! -f $FILE ]
        then 
            echo "" >&2
            echo " ERROR: mkfMakeInstallFiles: " >&2
            echo "          >>$FILE<< file not found " >&2
            echo "" >&2
            exit 1
        fi

        name=`basename $FILE`
        parent=`dirname $FILE`
        dir=`basename $parent`

        if [ -d $DEST_ROOT/$dir ]
        then 
            TOFILE=$DEST_ROOT/$dir/$name
        else
            echo "" >&2
            echo " ERROR: mkfMakeInstallFiles: " >&2
            echo "          >>$dir<< not a standard directory" >&2
            echo "" >&2
            exit 1
        fi  

        echo -e "\t-\$(AT)touch $FILE"
        echo -e "$TOFILE: $FILE"
        echo -e "\t-\$(AT)echo \"\t$name\"; \\"
        echo -e "\t      cp $FILE  $TOFILE; \\"  
        echo -e "\t      chmod $MASK $TOFILE"
        target="$target $TOFILE"
    done

    echo -e "$target"

else

    echo -e "files:"
    echo -e "\t-@echo \"\""

fi

exit 0
#
# ___oOo___

