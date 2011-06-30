#!/bin/bash
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
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
maxNbOfCalibratorsPerVOTable=0
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
				if [ $nbOfCalibrators -gt $maxNbOfCalibratorsPerVOTable ]
				then
				    let "maxNbOfCalibratorsPerVOTable = nbOfCalibrators"
				fi
        let "totalNbOfCalibrators += nbOfCalibrators"

        echo "found '${nbOfCalibrators}' new calibrators for a total of '${totalNbOfCalibrators}' (max. per VOTable : '${maxNbOfCalibratorsPerVOTable}')... DONE."
    else
        echo "ERROR accessing file !"
    fi
done

# Generating VOTable footer
echo -n "Extracting result footer from '$firstVotFile'... "
cat "$firstVotFile" | awk '{if ($1=="</TABLEDATA>")start=1;if(start==1)print;}' >> $RESULTFILE
echo "DONE"

#___oOo___
