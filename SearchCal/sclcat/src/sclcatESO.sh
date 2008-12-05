#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclcatESO.sh,v 1.7 2008-12-05 16:10:40 lafrasse Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.6  2008/12/01 10:45:52  lafrasse
# Streamlined output.
#
# Revision 1.5  2008/12/01 09:38:22  lafrasse
# Enhanced documentation and inline help.
#
# Revision 1.4  2008/07/23 22:31:07  lafrasse
# Code polishing.
#
# Revision 1.3  2008/07/17 07:12:35  lafrasse
# Added an option to generate config file and run commands.
#
# Revision 1.2  2008/07/11 16:15:02  lafrasse
# Moved generation directo from src/ to tmp/.
# Added exectution timestamps.
# Code syntax polishing.
#
# Revision 1.1  2008/07/11 12:56:55  lafrasse
# Added ESO Calibrator catalog generation scripts.
#
#*******************************************************************************

#/**
# @file
# Create calibrator catalog for ESO
#
# @synopsis
# \<sclcatESO\> [\e \<-g\> \<-c\> \<-r\>] [\e \<instrumentalConfigurationFile\>]
#
# @param instrumentalConfigurationFile : file containing the ESO instrumental
# configuration the calibrator catalog should be generated for.
#
# @opt
# @optname h : show usage help
# @optname g : generate a reference catalog
# @optname c : generate the configuration file and exit
# @optname r : generate the configuration file and run
# @optname i : generate HTML output
# 
# @details
# This script creates catalog of calibrator for ESO, using SearchCal tool. The
# result is either placed in :
# \li \em eso-ref directory if \em -g option is specified
# \li \em eso-\<date\> directory otherwise
#
# The log files, resulting of the SearchCal execution for each star, are stored
# in \em log sub-directory.
#
# The results of SearchCal are VOTables which are merged into a single result
# file.
# 
# */

# Print usage 
function printUsage () {
        echo -e "Usage: sclcatESO [-h] [-g] [-c] [-r]" 
        echo -e "\t-h\tprint this help."
        echo -e "\t-g\tgenerate reference."
        echo -e "\t-c\tgenerate the configuration file and exit."
        echo -e "\t-r\tgenerate the configuration file and run."
        echo -e "\t-i\tgenerate HTML output."
        exit 1;
}

#/**
# Parse result
# */
function parseResult () {
        sclcatESOParseResult $1
}

#/**
# Filter result
# */
function filterResult () {
        sclcatESOFilterResult $1
}

#/**
# Generate HTML Output
# */
function generateHtmlOutput () {
pwd
    echo -n "Generating PNG plot ... "
    FIRST_CATALOG=catalog1.fits
    FIRST_CATALOG_PATH=$1/result/$FIRST_CATALOG
    SECOND_CATALOG=catalog14.fits
    SECOND_CATALOG_PATH=$1/result/$SECOND_CATALOG
    OUTPUT_PLOT=$1/result/catalog.png
	stilts  plot2d \
            in1=$FIRST_CATALOG_PATH \
            xdata1='radiansToDegrees(hmsToRadians(RAJ2000))' \
            ydata1='radiansToDegrees(dmsToRadians(DEJ2000))' \
            colour1=red \
            name1='Raw Catalog' \
            size1=2 \
            shape1=filled_circle \
            in2=$SECOND_CATALOG_PATH \
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
            out=$OUTPUT_PLOT
    echo "DONE."

    echo -n "Completing HTML file ... "
    OUTPUT_FILE=$1/result/index.html
    mv $1/../../config/sclcatESOMap.html $OUTPUT_FILE

    END=`date`
    echo "The generation started on '$BEGINNING' and finished on '$END'.<br/>" >> $OUTPUT_FILE

    CONFIG_FILE=../config/sclcatESO.cfg
    echo "The querying parameters are listed below:<br/>" >> $OUTPUT_FILE

    MAG_BAND=`grep 'band=' $CONFIG_FILE`
    echo "<li>$MAG_BAND;<br/>" >> $OUTPUT_FILE

    MIN_MAG=`grep 'minMagRange=' $CONFIG_FILE`
    echo "<li>$MIN_MAG;<br/>" >> $OUTPUT_FILE

    MAX_MAG=`grep 'maxMagRange=' $CONFIG_FILE`
    echo "<li>$MAX_MAG;<br/>" >> $OUTPUT_FILE

    MAX_BASE=`grep 'baseMax=' $CONFIG_FILE`
    echo "<li>$MAX_BASE m;<br/>" >> $OUTPUT_FILE

    WAVELENGTH=`grep 'wlen=' $CONFIG_FILE`
    echo "<li>$WAVELENGTH &mu;m;<br/>" >> $OUTPUT_FILE

    echo "<br/>" >> $OUTPUT_FILE

    echo "You can download the catalogs below:<br/>" >> $OUTPUT_FILE
    RAW_CATALOG_SIZE=`du -sh $FIRST_CATALOG_PATH | awk '{print $1}'`
    echo "<li><a href='$FIRST_CATALOG'>Raw Catalog</a> (FITS format, &plusmn;$RAW_CATALOG_SIZE);<br/>" >> $OUTPUT_FILE
    FILTERED_CATALOG_SIZE=`du -sh $SECOND_CATALOG_PATH | awk '{print $1}'`
    echo "<li><a href='$SECOND_CATALOG'>Filtered Catalog</a> (FITS format, &plusmn;$FILTERED_CATALOG_SIZE);<br/>" >> $OUTPUT_FILE

    echo "</body>" >> $OUTPUT_FILE
    echo "</html>" >> $OUTPUT_FILE
    echo "DONE."
}

BEGINNING=`date`
echo "Started at: $BEGINNING"

# Generate output directory name
dir="../tmp/eso-run-`date +%Y-%m-%dT%H-%M-%S`";

# Parse command-line parameters
while getopts "hrgci" option
do
    case $option in
    h ) # Help option
        printUsage ;;
    g ) # generation reference option
        dir="eso-ref" ;;
    c ) # generate the configuration file and exit
        sclcatESOGenerateConfig ;
        exit ;;
    i ) # generate HTML output
        # Parse command-line parameters
        eval last=\${$#} # Get last 'command-line' parameter
        givenRunDirectory=$last
        if [ ! -d "${givenRunDirectory}" ]
        then
            printUsage
        fi
        generateHtmlOutput $givenRunDirectory;
        exit ;;
    r ) # generate the configuration file and run
        sclcatESOGenerateConfig ;;
    [?] ) # Unknown option
        printUsage ;
        exit ;;
    esac
done


# Prepare an empty directory 
if [ $dir == "eso-ref" ]; then
    rm -f $dir/*
    rm -rf $dir/log
else
    rm -rf $dir
    mkdir $dir
fi

# Start program
cd $dir
cmdBatch ../../config/sclcatESO.cfg -d log

cd ..
parseResult $dir
filterResult $dir

END=`date`
echo "Finished at: $END"

generateHtmlOutput ./

#___oOo___
