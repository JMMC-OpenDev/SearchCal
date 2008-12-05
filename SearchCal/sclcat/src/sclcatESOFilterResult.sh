#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclcatESOFilterResult.sh,v 1.3 2008-12-05 15:03:19 lafrasse Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.2  2008/12/03 10:08:11  lafrasse
# Added VOT conversion to FITS.
# Enhanced output traces.
#
# Revision 1.1  2008/12/01 16:21:44  mella
# First revision
#
#
#*******************************************************************************

#/**
# @file
# Clean bad, multiple or untrusted stars from raw catalog.
#
# @synopsis
# sclcatESOFilterResult \<run-directory\>
# 
# @details
# This script build the synthesis file from the collected calibrator lists.
#
# @usedfiles
# @filename ../config/sclcatESO.cfg : list of stars of interest for PRIMA
# */

# Print usage 
function printUsage () {
    echo -e "Usage: sclcatESOFilterResult [eso-ref] or [eso-run-<YYYY-MM-DDTHH-MM-SS>]" 
    echo -e "\t-h\t\tprint this help."
    echo -e "\t<eso-run>\tFilter results in eso-run directory"
    exit 1;
}

# Command line options parsing
while getopts "h" option
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
else
    echo "Filtering results from $dir"
fi

# Moving to the right directory
cd $dir
RESULTPATH=result
cd $RESULTPATH

# Convert raw VOTable catalog to FITS
stilts tcopy catalog.vot catalog0.fits

# First initialization
let PHASE=0
let PREVIOUSPHASE=$PHASE
let PHASE=$PHASE+1
PREVIOUSCATALOG=catalog$PREVIOUSPHASE.fits
CATALOG=catalog$PHASE.fits

SCRIPT=$0
if [ $# -eq 0 ]
then
if [ "$SCRIPT" -nt "$PREVIOUSCATALOG" ]
then
    echo "Script is newer than first catalog (thus regenerating everything...)"
    touch $PREVIOUSCATALOG
fi
else
    for f in $*; do touch $f ; done
fi

# Perform the given command, outputing the given comment, skipping if input file is older
newStep()
{

    ACTIONDESC=$1
    shift
    ACTIONCMD=$*
    echo
    echo -n "Step $PHASE ($PREVIOUSCATALOG -> $CATALOG) : $ACTIONDESC ... "  

    if [ $PREVIOUSCATALOG -nt $CATALOG ]
    then
        echo
        # KEEP quotes arround $@ to ensure a correct arguments process
        "$@"
        if [ $? -eq 0 ]
        then
            echo "DONE."
        else
            echo "FAILED (using previous catalog instead)."
        fi
        if [ ! -e $CATALOG ]
        then
            cp $PREVIOUSCATALOG $CATALOG
        fi
        stilts tpipe in=$CATALOG omode='count'
    else
        echo "SKIPPED."
    fi

    if [ -e "$CATALOG" ]
    then
        #stilts tpipe in=$CATALOG omode='count'
        echo
    fi

    let PREVIOUSPHASE=$PHASE
    let PHASE=$PHASE+1
    PREVIOUSCATALOG=catalog$PREVIOUSPHASE.fits
    CATALOG=catalog$PHASE.fits
}

newStep "Rejecting stars without VIS2" stilts tpipe in=$PREVIOUSCATALOG  cmd='progress ; select !NULL_VIS2' out=$CATALOG

newStep "Clearing query-specific 'dist' column" stilts tpipe in=$PREVIOUSCATALOG  cmd='progress ; delcols dist; addcol -before dist.ORIGIN dist NULL ' out=$CATALOG

# We use radians for ra dec for CoordHashCode to avoid equality problem with different string eg +10 00 00 and 10 00 00
newStep "Adding an hashing-key column, sorting using it" stilts tpipe in=$PREVIOUSCATALOG  cmd='progress ; addcol CoordHashCode concat(toString(hmsToRadians(RAJ2000)),toString(dmsToRadians(DEJ2000)))' cmd='progress ; sort CoordHashCode' out=$CATALOG

newStep "Rejecting fully duplicated lines" stilts tpipe in=$PREVIOUSCATALOG   cmd='progress ; uniq -count' cmd='progress ; colmeta -name DuplicatedLines DupCount' out=$CATALOG

newStep "Setting empty catalog identifiers to NaN" stilts tpipe in=$PREVIOUSCATALOG cmd='progress ; replaceval "" NaN HIP' cmd='progress ; replaceval "" NaN HD' cmd='progress ; replaceval "" NaN DM' out=$CATALOG
newStep "Creating catalog identifier columns as 'double' values" stilts tpipe in=$PREVIOUSCATALOG cmd='progress ; addcol _HIP parseDouble(HIP)' cmd='progress ; addcol _DM parseDouble(DM)' cmd='progress ; addcol _HD parseDouble(HD)' out=$CATALOG
newStep "Flagging duplicated _HIP entries" stilts tmatch1 in=$PREVIOUSCATALOG matcher=exact values='_HIP' out=$CATALOG
newStep "Renaming GroupId and GroupSize to HIPGroupId and HIPGroupSize" stilts tpipe in=$PREVIOUSCATALOG cmd='progress ; colmeta -name HIPGroupID GroupID' cmd='progress ; colmeta -name HIPGroupSize GroupSize' out=$CATALOG
newStep "Flagging duplicated _HD entries" stilts tmatch1 in=$PREVIOUSCATALOG matcher=exact values='_HD' out=$CATALOG
newStep "Renaming GroupId and GroupSize to HDGroupId and HDGroupSize" stilts tpipe in=$PREVIOUSCATALOG cmd='progress ; colmeta -name HDGroupID GroupID' cmd='progress ; colmeta -name HDGroupSize GroupSize' out=$CATALOG
newStep "Flagging duplicated _DM entries" stilts tmatch1 in=$PREVIOUSCATALOG matcher=exact values='_DM' out=$CATALOG
newStep "Renaming GroupId and GroupSize to DMGroupId and DMGroupSize" stilts tpipe in=$PREVIOUSCATALOG cmd='progress ; colmeta -name DMGroupID GroupID' cmd='progress ; colmeta -name DMGroupSize GroupSize' out=$CATALOG

#######################################################
# AT THIS LEVEL THE CATALOG HAS KEPT ALL INFORMATIONS #
#######################################################

# The 3 next actions will remove entries without index catalog -> which should be kept
newStep "Removing duplicated catalog identifiers rows" stilts tpipe in=$PREVIOUSCATALOG cmd='progress; select NULL_HIPGroupSize' cmd='progress; select NULL_HDGroupSize' cmd='progress; select NULL_DMGroupSize' out=$CATALOG
newStep "Removing stars with DEC > 40 " stilts tpipe in=$PREVIOUSCATALOG cmd='progress; select "dmsToRadians(DEJ2000) < degreesToRadians(40)"' out=$CATALOG


# TODO check that no star exists with duplicated coords using one of the next
# filters....
#newStep "Reject stars with duplicated coordinates (first or last is kept)" stilts tpipe in=$PREVIOUSCATALOG cmd='progress ; sort CoordHashCode' cmd='progress ; uniq -count CoordHashCode' out=$CATALOG
#stilts tmatch1 in=catalog3.fits matcher=2d values='_RAJ2000 _DEJ2000' params=0 out=sameCoords2.fits
