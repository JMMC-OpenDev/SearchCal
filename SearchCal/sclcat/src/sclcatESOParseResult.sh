#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclcatESOParseResult.sh,v 1.5 2008-07-23 23:06:23 lafrasse Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.4  2008/07/23 22:32:53  lafrasse
# Changed calibrator couting method.
#
# Revision 1.3  2008/07/21 15:45:33  lafrasse
# Corrected way to list vot files to handle large collections to remove "argument
# list too long" errors.
# Removed unneeded VOtable copying process.
# Changed default path of generated catalog file.
# Added output log to better monitor progression.
# Removed HTML resume generation.
# Disabled calibrator numbering due to memory errors on large catalogs.
#
# Revision 1.2  2008/07/11 15:55:22  lafrasse
# Removed 'Finished' timestamp.
#
# Revision 1.1  2008/07/11 12:56:56  lafrasse
# Added ESO Calibrator catalog generation scripts.
#
#*******************************************************************************

#/**
# @file
# Create synthesis files for the ESO calibrator catalog
#
# @synopsis
# sclcatESOParseResult \<run-directory\>
# 
# @details
# This script build the synthesis file from the collected calibrator lists.
#
# @usedfiles
# @filename ../config/sclcatESO.cfg : list of stars of interest for PRIMA
# */

# Print usage 
function printUsage () {
    echo -e "Usage: sclcatESOParseResult [eso-ref] or [eso-run-<YYYY-MM-DDTHH-MM-SS>]" 
    echo -e "\t-h\t\tprint this help."
    echo -e "\t<eso-run>\tParse results in eso-run directory"
    exit 1;
}

while getopts "h" option
# Initial declaration.
# c, h, u and t are the options (flags) expected.
# The : after option 't' shows it will have an argument passed with it.
do
  case $option in
    h ) # Help option
        printUsage ;;
    * ) # Unknown option
        printUsage ;;
    esac
done

# Define temporary PATH # change it if the script becomes extern
PATH=$PATH:$PWD/../bin

# Parse command-line parameters
dir=$1
if [ ! -d "$dir" ]
then
    echo "Please give one run directory"
    exit 1
fi

# Moving to the right directory
cd $dir
RESULTPATH=result
mkdir $RESULTPATH

# Generating VOTable header
tmp=( $(ls | grep ".vot") )
RESULTFILE=$RESULTPATH/catalog.vot
echo -n "Generating result header ... "
cat $tmp | awk '{if ($1=="<TR>")end=1;if(end!=1)print;}' &> $RESULTFILE
echo "DONE"

# Loop on every calibrators of every stars, then build calibrator file
nbOfVOTablesDone=0
totalNbOfVOTables=`ls -l | grep ".vot"| wc | awk '{print $1}'`
totalNbOfCalibrators=0
for i in *.vot
do
    # Increment counter
    let "nbOfVOTablesDone += 1"

    if [ -f "$i" ]
    then
        echo -n "Analyzing file $i ... "
        xml sel  -N VOT=http://www.ivoa.net/xml/VOTable/v1.1 -t -m "/" -c "//VOT:TR" $i >> $RESULTFILE
        nbOfCalibrators=`xml sel  -N VOT=http://www.ivoa.net/xml/VOTable/v1.1 -t -m "/" -v "count(//VOT:TR)" $i`
        let "totalNbOfCalibrators += nbOfCalibrators"
        echo " DONE (${nbOfVOTablesDone} / ${totalNbOfVOTables}, found '$nbOfCalibrators' new calibrators for a total of '$totalNbOfCalibrators')."
    else
        echo "ERROR accessing file $i !"
    fi
done

# Generating VOTable footer
echo -n "Generating result footer ... "
cat $tmp | awk '{if ($1=="</TABLEDATA>")start=1;if(start==1)print;}' >> $RESULTFILE
echo "DONE"

#___oOo___
