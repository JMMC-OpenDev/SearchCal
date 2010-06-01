#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclcatFilterCatalog.sh,v 1.2 2010-06-01 13:22:02 lafrasse Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.1  2010/04/13 14:53:34  lafrasse
# Renamed (and generalyzed) sclcatESO in sclcatiCreateCatalog.
# Renamed (and generalyzed) sclcatESOiFilterResult in sclcatFilterCatalog.
#
#*******************************************************************************

#/**
# @file
# Clean bad, multiple or untrusted stars from raw catalog.
#
# @synopsis
# sclcatFilterCatalog <h|c|e> \<run-directory\>
# 
# @details
# This script build the synthesis file from the collected calibrator lists.
#
# @opt
# @optname h : show usage help.
# @optname c : CDS catalog filtering.
# @optname e : ESO catalog filtering.
# */

# Print usage 
function printUsage () {
    echo -e "Usage: sclcatFilterCatalog <h|c|e> <xxx-ref|xxx-run-YYYY-MM-DDTHH-MM-SS>" 
    echo -e "\t-h\t\tprint this help."
    echo -e "\t-c\t\tCDS filtering."
    echo -e "\t-e\t\tESO filtering."
    echo -e "\t<eso-run>\tFilter results in eso-run directory"
    exit 1;
}

STILTS_JAVA_OPTIONS=" -Xmx2048m "

# Perform the given command, outputing the given comment, skipping if input file is older
newStep()
{
    ACTIONDESC=$1
    shift
    ACTIONCMD=$*
    echo
    echo -n "Step $PHASE ($PREVIOUSCATALOG -> $CATALOG) : $ACTIONDESC ... "  

    # Perform the given command only if previous catalog has changed since last computation
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
        stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CATALOG omode='count'
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

# Command line options parsing
if [ $# -lt 1 ] # Always at least 1 option specified
then
    printUsage
fi
FILTERING_STYLE="NONE"
while getopts "hce" option
do
  case $option in
    h ) # Help option
        printUsage ;;
    c ) # CDS filtering option
        FILTERING_STYLE="CDS" ;;
    e ) # ESO filtering option
        FILTERING_STYLE="ESO" ;;
    * ) # Unknown option
        printUsage ;;
    esac
done

# Define temporary PATH # change it if the script becomes extern
PATH=$PATH:$PWD/../bin

# Parse command-line parameters
dir=$2
if [ ! -d "$dir" ]
then
    printUsage
else
    echo "Filtering results from $dir"
fi

# Moving to the right directory
cd "${dir}/result/"

# First initialization
let PHASE=0
PREVIOUSCATALOG=catalog.vot
CATALOG=catalog0.fits
newStep "Convert raw VOTable catalog to FITS" stilts ${STILTS_JAVA_OPTIONS} tcopy $PREVIOUSCATALOG $CATALOG

#####################################
# PRELIMINARY CALIBRATORS SELECTION #
#####################################
newStep "Rejecting stars without VIS2" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG  cmd='progress ; select !NULL_VIS2' out=$CATALOG

#################################
# COMPUTING DUPLICATE HASH KEYS #
#################################
# We use radians for ra dec for CoordHashCode to avoid equality problem with different string eg +10 00 00 and 10 00 00
newStep "Adding an hashing-key column, sorting using it" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG  cmd='progress ; addcol CoordHashCode concat(toString(hmsToRadians(RAJ2000)),toString(dmsToRadians(DEJ2000)))' cmd='progress ; sort CoordHashCode' out=$CATALOG
newStep "Setting empty catalog identifiers to NaN" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG cmd='progress ; replaceval "" NaN HIP' cmd='progress ; replaceval "" NaN HD' cmd='progress ; replaceval "" NaN DM' out=$CATALOG
newStep "Creating catalog identifier columns as 'double' values" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG cmd='progress ; addcol _HIP parseDouble(HIP)' cmd='progress ; addcol _DM parseDouble(DM)' cmd='progress ; addcol _HD parseDouble(HD)' out=$CATALOG
newStep "Flagging duplicated _HIP entries" stilts ${STILTS_JAVA_OPTIONS} tmatch1 in=$PREVIOUSCATALOG matcher=exact values='_HIP' out=$CATALOG
newStep "Renaming GroupId and GroupSize to HIPGroupId and HIPGroupSize" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG cmd='progress ; colmeta -name HIPGroupID GroupID' cmd='progress ; colmeta -name HIPGroupSize GroupSize' out=$CATALOG
newStep "Flagging duplicated _HD entries" stilts ${STILTS_JAVA_OPTIONS} tmatch1 in=$PREVIOUSCATALOG matcher=exact values='_HD' out=$CATALOG
newStep "Renaming GroupId and GroupSize to HDGroupId and HDGroupSize" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG cmd='progress ; colmeta -name HDGroupID GroupID' cmd='progress ; colmeta -name HDGroupSize GroupSize' out=$CATALOG
newStep "Flagging duplicated _DM entries" stilts ${STILTS_JAVA_OPTIONS} tmatch1 in=$PREVIOUSCATALOG matcher=exact values='_DM' out=$CATALOG
newStep "Renaming GroupId and GroupSize to DMGroupId and DMGroupSize" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG cmd='progress ; colmeta -name DMGroupID GroupID' cmd='progress ; colmeta -name DMGroupSize GroupSize' out=$CATALOG

#########################################################
# AT THIS LEVEL THE CATALOG HAS KEPT ALL PERTINENT DATA #
#########################################################

newStep "Clearing query-specific 'dist' column" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG  cmd='progress ; delcols dist; addcol -before dist.ORIGIN dist NULL ' out=$CATALOG
newStep "Rejecting fully duplicated lines" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG   cmd='progress ; uniq -count' cmd='progress ; colmeta -name DuplicatedLines DupCount' out=$CATALOG
newStep "Removing duplicated catalog identifiers rows" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG cmd='progress; select NULL_HIPGroupSize' cmd='progress; select NULL_HDGroupSize' cmd='progress; select NULL_DMGroupSize' out=$CATALOG

case $FILTERING_STYLE in
    CDS ) # CDS fitering
        newStep "Rejecting stars with low confidence on 'DIAM_VK'" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG  cmd='progress ; select equals(diam_vk.confidence,\"HIGH\")' out=$CATALOG ;
        newStep "Rejecting stars with SB9 references" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG  cmd='progress ; select NULL_SBC9' out=$CATALOG ;
        newStep "Rejecting stars with WDS references" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG  cmd='progress ; select NULL_WDS' out=$CATALOG ;
        newStep "Adding a flag column for R provenance" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG  cmd='progress ; addcol f_Rmag NULL_R.confidence?1:0' out=$CATALOG ;
        newStep "Adding a flag column for I provenance" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG  cmd='progress ; addcol f_Imag NULL_I.confidence?1:0' out=$CATALOG ;
        newStep "Adding the 'Name' column" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG  cmd='progress ; addcol Name !equals(HIP,\"NaN\")?\"HIP\"+HIP:(!NULL_TYC2?\"TYC\"+TYC2:\"\")' out=$CATALOG ;
        
        # Columns deletion (to force output of computed values in 'sclsvr' over retrieved one in 'vobs')
        newStep "Removing unwanted column UDDK" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG cmd='delcols "UDDK"' out=$CATALOG ;
        
        # Columns renaming
        OLD_NAMES=( pmRa  pmDec  B     V     R     I     J     H     K     diam_vk  e_diam_vk  UD_B  UD_V  UD_R  UD_I  UD_J  UD_H  UD_K  e_Plx ) ;
        NEW_NAMES=( pmRA  pmDEC  Bmag  Vmag  Rmag  Imag  Jmag  Hmag  Kmag  LDD      e_LDD      UDDB  UDDV  UDDR  UDDI  UDDJ  UDDH  UDDK  e_plx ) ;
        i=0 ;
        for OLD_NAME in ${OLD_NAMES[*]}
        do
            NEW_NAME=${NEW_NAMES[i]} ;
            newStep "Renaming column '${OLD_NAME}' to '${NEW_NAME}'" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG cmd="progress ; colmeta -name ${NEW_NAME} ${OLD_NAME}" out=$CATALOG ;
            let "i=$i+1" ;
        done

        COLUMNS_SET="Name RAJ2000 DEJ2000 pmRA pmDEC Bmag Vmag Rmag f_Rmag Imag f_Imag Jmag Hmag Kmag LDD e_LDD UDDB UDDV UDDR UDDI UDDJ UDDH UDDK plx e_plx SpType Teff_SpType logg_SpType" ;
        newStep "Applying final columns set" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG cmd="keepcols \"${COLUMNS_SET}\"" out=$CATALOG ;
        for COLUMN_NAME in ${COLUMNS_SET}
        do
            newStep "Rejecting stars without '${COLUMN_NAME}'" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG  cmd="progress ; select !NULL_${COLUMN_NAME}" out=$CATALOG ;
        done
        ;;

    ESO ) # ESO fitering
        newStep "Removing stars with DEC < 40 " stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG cmd='progress; select "dmsToRadians(DEJ2000) < degreesToRadians(40)"' out=$CATALOG ;
        ;;

    * ) # Unknown filtering
        printUsage ;;
esac

out="final.fits"
echo "Final results are available in ${out} ... DONE."
mv $PREVIOUSCATALOG ${out}

# TODO check that no star exists with duplicated coords using one of the next
# filters....
#newStep "Reject stars with duplicated coordinates (first or last is kept)" stilts tpipe in=$PREVIOUSCATALOG cmd='progress ; sort CoordHashCode' cmd='progress ; uniq -count CoordHashCode' out=$CATALOG
#stilts tmatch1 in=catalog3.fits matcher=2d values='_RAJ2000 _DEJ2000' params=0 out=sameCoords2.fits
