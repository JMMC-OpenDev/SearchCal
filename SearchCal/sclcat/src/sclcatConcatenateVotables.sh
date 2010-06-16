#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclcatConcatenateVotables.sh,v 1.4 2010-06-16 11:35:03 mella Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.3  2010/04/13 08:41:17  lafrasse
# Enhanced documentation and execution log.
#
# Revision 1.2  2010/04/12 15:25:31  lafrasse
# Enforced exit after usage printing.
#
# Revision 1.1  2010/04/12 13:59:59  lafrasse
# Renamed (and generalyzed) sclcatESOParseResult in sclcatConcatenateVotables.
#
#*******************************************************************************

#/**
# @file
# Create one synthesis file from multiple VOTables.
#
# @synopsis
# sclcatConcatenateVotables \<catalogDir\>
# 
# @details
# This script uses the header of the first VOTable for the output.
#
# @usedfiles
# @filename <catalogDir>/result/catalog.vot : the aggregated resulting file;
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
CATALOG_DIR=$1
if [ ! -d "$CATALOG_DIR" ]
then
    echo "Please give a catalog directory"
    exit 1
else
    echo "Parsing results from '$CATALOG_DIR'."
fi

# Moving to the right directory
cd $CATALOG_DIR

# Creating result path
RESULTPATH=result
RESULTFILE=$RESULTPATH/catalog.vot
echo "Writing aggregation result in '$CATALOG_DIR/$RESULTFILE'."
mkdir -p $RESULTPATH

# Generating VOTable header
firstVotFile="$( ls *.vot |head -1)"
echo -n "Extracting result header from '$firstVotFile'... "
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
        echo -n "Aggregating data from '$file' (${nbOfVOTablesDone} / ${totalNbOfVOTables}) : "

        xml sel  -N VOT=http://www.ivoa.net/xml/VOTable/v1.1 -t -m "/" -c "//VOT:TR" "${file}" >> $RESULTFILE

        nbOfCalibrators=`xml sel  -N VOT=http://www.ivoa.net/xml/VOTable/v1.1 -t -m "/" -v "count(//VOT:TR)" "${file}"`
        let "totalNbOfCalibrators += nbOfCalibrators"

        echo "found '${nbOfCalibrators}' new calibrators for a total of '${totalNbOfCalibrators}' ... DONE."
    else
        echo "ERROR accessing file !"
    fi
done

# Generating VOTable footer
echo -n "Extracting result footer from '$firstVotFile'... "
cat "$firstVotFile" | awk '{if ($1=="</TABLEDATA>")start=1;if(start==1)print;}' >> $RESULTFILE
echo "DONE"

#___oOo___
