#!/bin/bash
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************

#/**
# @file
# Create a PNG file showing the map of a catalog sources (both raw and filtered)
#
# @synopsis
# sclcatGenerateMapPlot <inputConfigFile> <inputMapFile> (generationDate> <catalogDir>
#
# @details
# This script creates :
# \li <catalogDir>/index.html for web browsing;
# \li <catalogDir>/result/catalog.png for the map.
#
# @usedfiles
# @filename <catalogDir>/result/catalog0.fits : raw results;
# @filename <catalogDir>/result/final.fits : filtered results;
# */

# Print usage 
function printUsage () {
        echo -e "Usage: sclcatGenerateMapPlot <inputConfigFile> <inputMapFile> <generationDate> <catalogDir>"
        echo -e "\t-h\tprint this help."
        exit 1;
}

#/**
# Generate a PNG map of resulting catalog.
# */
function generatePngPlot () {
	stilts  plot2d \
            in1=${RAW_CATALOG_PATH} \
            xdata1='radiansToDegrees(hmsToRadians(RAJ2000))' \
            ydata1='radiansToDegrees(dmsToRadians(DEJ2000))' \
            colour1=red \
            name1='Raw Catalog' \
            size1=2 \
            shape1=filled_circle \
            in2=${FILTERED_CATALOG_PATH} \
            xdata2='radiansToDegrees(hmsToRadians(RAJ2000))' \
            ydata2='radiansToDegrees(dmsToRadians(DEJ2000))' \
            xlabel='RAJ2000 (degrees)' \
            ylabel='DEJ2000 (degrees)' \
            colour2=green \
            name2='Filtered Catalog' \
            size2=2 \
            shape2=open_circle \
            transparency2=1 \
            xlo=-10 \
            xhi=370 \
            ylo=-95 \
            yhi=+95 \
            xpix=1024 \
            ypix=768 \
            fontstyle=bold \
            title='Calibrators Catalog Plot' \
            omode=out \
            out=$PNG_FILE
}

#/**
# Complete the HTML map file.
# */
function completeHtmlMap () {
    cp $MAP_FILE $HTML_FILE

    echo "Catalog generation date: '${GENERATION_DATE}'.<br/>" >> $HTML_FILE

    echo "The querying parameters are listed below:<br/>" >> $HTML_FILE

    MAG_BAND=`grep 'band=' ${CONFIG_FILE}`
    echo "<li>${MAG_BAND};<br/>" >> $HTML_FILE

    MIN_MAG=`grep 'minMagRange=' ${CONFIG_FILE}`
    echo "<li>${MIN_MAG};<br/>" >> $HTML_FILE

    MAX_MAG=`grep 'maxMagRange=' ${CONFIG_FILE}`
    echo "<li>${MAX_MAG};<br/>" >> $HTML_FILE

    MAX_BASE=`grep 'baseMax=' ${CONFIG_FILE}`
    echo "<li>${MAX_BASE} m;<br/>" >> $HTML_FILE

    WAVELENGTH=`grep 'wlen=' ${CONFIG_FILE}`
    echo "<li>${WAVELENGTH} &mu;m;<br/>" >> $HTML_FILE

    echo "<br/>" >> $HTML_FILE

    FILTERED_CATALOG_SIZE=`du -sh $FILTERED_CATALOG_PATH | awk '{print $1}'`
    echo "<a href='result/${FILTERED_CATALOG}'>Click to Download the Catalog</a> (FITS format, &plusmn;${FILTERED_CATALOG_SIZE}).<br/>" >> $HTML_FILE

    echo "</body>" >> $HTML_FILE
    echo "</html>" >> $HTML_FILE
}

# Parse command-line parameters
while getopts "h" option
do
    case $option in
    h ) # Help option
        printUsage ;
        exit ;;
    [?] ) # Unknown option
        printUsage ;
        exit ;;
    esac
done

CONFIG_FILE=$1
if [ ! -f "${CONFIG_FILE}" ]
then
    echo "Please give a configuration file."
    exit 1
else
    echo "Reading coniguration file from ${CONFIG_FILE}"
fi

MAP_FILE=$2
if [ ! -f "${MAP_FILE}" ]
then
    echo "Please give an HTML map file."
    exit 1
else
    echo "Reading HTML map file from ${MAP_FILE}"
fi

GENERATION_DATE=$3

CATALOG_DIR=$4
if [ ! -d "${CATALOG_DIR}" ]
then
    echo "Please give a valid output directory ${CATALOG_DIR}."
    exit 1
else
    echo "Parsing results from ${CATALOG_DIR}"
fi

RAW_CATALOG=catalog0.fits
RAW_CATALOG_PATH=$CATALOG_DIR/result/$RAW_CATALOG
if [ ! -f "${RAW_CATALOG_PATH}" ]
then
    echo "Please give a raw FITS catalog file."
    exit 1
else
    echo "Reading raw FITS catalog from ${RAW_CATALOG_PATH}"
fi

FILTERED_CATALOG=final.fits
FILTERED_CATALOG_PATH=$CATALOG_DIR/result/$FILTERED_CATALOG
if [ ! -f "${FILTERED_CATALOG_PATH}" ]
then
    echo "Please give a filtered FITS catalog file."
    exit 1
else
    echo "Reading filtered FITS catalog from ${FILTERED_CATALOG_PATH}"
fi

PNG_FILE=$CATALOG_DIR/result/catalog.png
echo "Writing PNG file in ${PNG_FILE}"

HTML_FILE=$CATALOG_DIR/index.html
echo "Writing HTML file in ${HTML_FILE}"

# Launch generation and completion
echo -n "Generating PNG plot ... "
generatePngPlot
echo "DONE."

echo -n "Completing HTML Map ... "
completeHtmlMap
echo "DONE."

#___oOo___
