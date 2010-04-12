#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclcatConcatenateVotables.sh,v 1.2 2010-04-12 15:25:31 lafrasse Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.1  2010/04/12 13:59:59  lafrasse
# Renamed (and generalyzed) sclcatESOParseResult in sclcatConcatenateVotables.
#
#*******************************************************************************

#/**
# @file
# Create one synthesis file from multiple VOTables.
#
# @synopsis
# sclcatConcatenateVotables \<run-directory\>
# 
# @details
# This script uses the header of the first VOTable for the output.
# */

# Print usage 
function printUsage () {
    echo -e "Usage: sclcatConcatenateVotables [cat-ref] or [cat-run-<YYYY-MM-DDTHH-MM-SS>]" 
    echo -e "\t-h\t\tprint this help."
    exit 1;
}

# Command line options parsing
while getopts "h" option
do
  case $option in
    h ) # Help option
        printUsage ;
        exit ;;
    * ) # Unknown option
        printUsage ;
        exit ;;
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
else
    echo "Parsing results from $dir"
fi

# Moving to the right directory
cd $dir
RESULTPATH=result
mkdir $RESULTPATH

# Generating VOTable header
firstVotFile=( $( ls *.vot ) )

RESULTFILE=$RESULTPATH/catalog.vot

# Generating VOTable header
echo -n "Generating result header (using $firstVotFile)... "
xml fo "$firstVotFile" | awk '{if ($1=="<TR>")end=1;if(end!=1)print;}' &> $RESULTFILE
echo "DONE"

# Loop on every calibrators of every stars, then build calibrator file
nbOfVOTablesDone=0
totalNbOfVOTables=`ls -l *.vot | wc -l`
totalNbOfCalibrators=0
for file in *.vot
do
    # Increment counter
    let "nbOfVOTablesDone += 1"

    if [ -f "$file" ]
    then
        echo -n "Extracting data from '$file' (${nbOfVOTablesDone} / ${totalNbOfVOTables}) : "

        xml sel  -N VOT=http://www.ivoa.net/xml/VOTable/v1.1 -t -m "/" -c "//VOT:TR" "${file}" >> $RESULTFILE

        nbOfCalibrators=`xml sel  -N VOT=http://www.ivoa.net/xml/VOTable/v1.1 -t -m "/" -v "count(//VOT:TR)" "${file}"`
        let "totalNbOfCalibrators += nbOfCalibrators"

        echo "found '${nbOfCalibrators}' new calibrators for a total of '${totalNbOfCalibrators}' ... DONE."
    else
        echo "ERROR accessing file !"
    fi
done

# Generating VOTable footer
echo -n "Generating result footer ... "
cat "$firstVotFile" | awk '{if ($1=="</TABLEDATA>")start=1;if(start==1)print;}' >> $RESULTFILE
echo "DONE"

#___oOo___
