#!/bin/bash
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************

#/**
# @file
# Create calibrator catalog with SearchCal.
#
# @synopsis
# \<sclcatCreateCatalog\> <-h|-c|-e>
#
# @opt
# @optname h : show usage help;
# @optname c : generate calibrator catalog for CDS.
# @optname e : generate calibrator catalog for ESO;
#
# @details
# The log files, resulting of the SearchCal execution for each star, are stored
# in \em log sub-directory.
# The results of SearchCal are VOTables which are merged into a single result
# file.
# */

# Print usage 
function printUsage () {
        echo -e "Usage: sclcatCreateCatalog <-h|-c|-e>" 
        echo -e "\t-h\tPrint this help."
        echo -e "\t-c\tGenerate a calibrator catalog for CDS."
        echo -e "\t-e\tGenerate a calibrator catalog for ESO."
        exit 1;
}

# Parse command-line parameters
if [ $# -lt 1 ] # Always at least 1 option specified
then
    printUsage
fi
TARGET="NONE"
FILTER="NONE"
while getopts "hce" option
do
    case $option in
    h ) # Help option
        printUsage ;;
    c ) # CDS catalog
        TARGET="CDS" ;
        FILTER="-c" ;;
    e ) # ESO catalog
        TARGET="ESO" ;
        FILTER="-e" ;;
    * ) # Unknown option
        printUsage ;;
    esac
done

BEGINNING=`date`
echo "Started at: $BEGINNING"

# Generate output directory name
DATE=`date +%Y-%m-%dT%H-%M-%S`
CATALOG_DIR="../tmp/${TARGET}-run-${DATE}";
mkdir -p ${CATALOG_DIR}
cd ${CATALOG_DIR}
echo "Working in directory '${CATALOG_DIR}'."

# define temporary PATH
PATH=$PATH:$PWD/../../bin

# Generate configuration file from header
HEADER_FILE="../../config/sclcat${TARGET}Header.cfg"
mkdir -p conf
CONFIG_FILE="./conf/sclcatCmdBatch.cfg"
MAP_FILE="./conf/sclcatHtmlHeader.map"
if [ ! -f "${HEADER_FILE}" ]; then
    echo "Could not generate configuration file : header file '${HEADER_FILE}' missing."
    exit 1
else
    echo "Generating configuration file from header '${HEADER_FILE}':"
    sclcatGenerateConfigByMozaicing ${HEADER_FILE} ${CONFIG_FILE} ${MAP_FILE}
fi

# Start batch execution of SearchCal queries
cmdBatch "${CONFIG_FILE}" -d log

# Retry failed queries if any
sclcatRetryFailedQueries ./

# Aggregate each resulting VOtable in one big file
sclcatConcatenateVotables ./

# Further filtering the catalog
sclcatFilterCatalog ${FILTER} ./

# Generate HTML index and map plot
sclcatGenerateMapPlot ${CONFIG_FILE} ${MAP_FILE} ${DATE} ./

END=`date`
echo "Finished at: $END"

#___oOo___
