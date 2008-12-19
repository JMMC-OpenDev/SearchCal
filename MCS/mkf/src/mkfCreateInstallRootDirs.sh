#!/bin/bash

#*******************************************************************************
# JMMC project
#
# "@(#) $Id: mkfCreateInstallRootDirs.sh,v 1.7 2008-12-19 08:14:15 gzins Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.6  2008/05/25 13:39:52  gzins
# Added python directories
#
# Revision 1.5  2005/12/05 14:32:45  gzins
# Fixed wrong MCSDATA directory structure
#
# Revision 1.4  2005/12/02 08:28:06  gzins
# Updated to create directories according to the new MCS directory structure
#
# Revision 1.3  2005/02/15 08:40:15  gzins
# Added CVS log as file modification history
#
# gzins     10-09-2004  Created
# gzins     10-11-2004  Added creation of MCSDATA in MCSROOT
#
#*******************************************************************************
# NAME
#   mkfCreateInstallRootDirs- create/check standard installation directory
#   structure
#
# SYNOPSIS
#   mkfCreateInstallRootDirs [<root name>] 
#
# DESCRIPTION
#   Utility used ot create new or missing part of installation directory
#   structure for INTROOT (Integration area) or MCSROOT (MCS Root directory
#   structure)
#
#       <root name> the name of the directory from which the directory structure
#              starts. If not existing already, directory/ies are created.
#   
#   If <root name> is not given, this script simply print out the list of
#   directories to be created.
#
# FILES
#
# ENVIRONMENT
#
# RETURN VALUES
#
# CAUTIONS
#
# SEE ALSO 
#   MCS - Programming Standard
#
# BUGS    
#
#----------------------------------------------------------------------
#

# signal trap (if any)


# Input parameters given should be 2 or 3 and in the correct format:
if [ $# != 0 -a $# != 1 ]
then 
    echo -e "\n\tUsage: mkfCreateInstallRootDirs [<root name>]\n"
    exit 1
fi 

# Define the content of each area 

# Directories that shall be present in any area 
BASE_DIR_LIST="config                   \
               doc                      \
               bin                      \
               bitmaps                  \
               include                  \
               lib                      \
               lib/python               \
               lib/python/site-packages \
               errors                   \
               man                      \
               man/man1                 \
               man/man2                 \
               man/man3                 \
               man/man4                 \
               man/man5                 \
               man/man6                 \
               man/man7                 \
               man/man8                 \
               templates                \
               "

# Additional directories for MCSDATA
MCSDATA_DIR_LIST="log    \
                  tmp    \
                 "

# If no root directory is given
if [ $# = 0 ]
then
    # Print out directory list
    for dir in $BASE_DIR_LIST
    do
        echo "$dir"
    done
    exit 0
fi

# Get the input parameters
ROOT_NAME=$1

# Check validity for parameter ROOT_NAME
# Test existence of a ROOT_NAME file
if [ -f $ROOT_NAME ]
then
    # a file called ROOT_NAME already exists
    echo -e "\n ERROR: I cannot create the starting directory because a file"
    echo "             called >>$ROOT_NAME<< already exists."
    echo "             Use another name or remove the existing file"
    echo ""
    exit 1
fi

# Test existence of a ROOT_NAME directory
if [ ! -d $ROOT_NAME ]
then
    # there is no ROOT_NAME directory
    # Create ROOT_NAME directory
    if mkdir $ROOT_NAME
    then
        # Creation succeeds
        echo "   CREATED >>> |---$ROOT_NAME "
    else
        # Creation failed 
        echo -e "\n ERROR: I cannot create the starting directory"
        echo -e "          >>$ROOT_NAME<<"
        echo "             Please fix the problem and try again."
        echo ""
        exit 1
    fi
fi


# Check directory structure

# If not already there, create all the needed subdirectories
for dir in $BASE_DIR_LIST
do
    if [ ! -d $ROOT_NAME/$dir ]
    then
        mkdir $ROOT_NAME/$dir
        echo "   CREATED >>>     |---$dir "
    fi
done


# INTROOT case
if [ "$ROOT_NAME" = "$INTROOT" ]
then
    for dir in $BASE_DIR_LIST
    do
        # directories must be writable by other developers
        chmod 777 $ROOT_NAME/$dir
    done
# MCSROOT case
else
    # Test existence of a MCSDATA directory
    if [ ! -d $MCSDATA ]
    then
        echo -e "\nCheck $MCSDATA directory structure"
        # there is no MCSDATA directory
        # Create MCSDATA directory
        if mkdir $MCSDATA
        then
            # Creation succeeds
            echo -e "   CREATED >>> |---$MCSDATA "
        else
            # Creation failed 
            echo -e "\n ERROR: I cannot create the MCSDATA directory"
            echo -e "          >>$MCSDATA<<"
            echo "             Please fix the problem and try again."
            echo ""
            exit 1
        fi
    fi
    # Create MCSDATA additional directories
    for dir in $MCSDATA_DIR_LIST
    do
        if [ ! -d $MCSDATA/$dir ]
        then
            mkdir $MCSDATA/$dir
            echo "   CREATED >>>     |---$dir "
        fi
        # directories must be writable by all
        chmod 777 $MCSDATA/$dir
    done
fi

#
#___oOo___
