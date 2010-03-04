#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclcatESOFilterResult.sh,v 1.5 2010-03-04 16:27:39 lafrasse Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.4  2009/12/02 10:34:21  mella
# fix filter label
#
# Revision 1.3  2008/12/05 15:03:19  lafrasse
# Updated to handle catalogs when no match is found (using previous catalog
# instead, in order to continue the execution).
#
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
# sclcatESOFilterResult [h|c|e] \<run-directory\>
# 
# @details
# This script build the synthesis file from the collected calibrator lists.
#
# @opt
# @optname h : show usage help
# @optname c : CDS catalog filtering
# @optname e : ESO catalog filtering
# */

# Print usage 
function printUsage () {
    echo -e "Usage: sclcatESOFilterResult [h|c|e] <xxx-ref|xxx-run-YYYY-MM-DDTHH-MM-SS>" 
    echo -e "\t-h\t\tprint this help."
    echo -e "\t-c\t\tCDS filtering."
    echo -e "\t-e\t\tESO filtering."
    echo -e "\t<eso-run>\tFilter results in eso-run directory"
    exit 1;
}

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

# Command line options parsing
FILTERING_STYLE=""
while getopts "hce" option
do
  case $option in
    h ) # Help option
        printUsage ;;
    c ) # CDS filtering option
        FILTERING_STYLE="CDS" ;;
    h ) # ESO filtering option
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
newStep "Convert raw VOTable catalog to FITS" stilts tcopy $PREVIOUSCATALOG $CATALOG

#####################################
# PRELIMINARY CALIBRATORS SELECTION #
#####################################
newStep "Rejecting stars without VIS2" stilts tpipe in=$PREVIOUSCATALOG  cmd='progress ; select !NULL_VIS2' out=$CATALOG

#################################
# COMPUTING DUPLICATE HASH KEYS #
#################################
# We use radians for ra dec for CoordHashCode to avoid equality problem with different string eg +10 00 00 and 10 00 00
newStep "Adding an hashing-key column, sorting using it" stilts tpipe in=$PREVIOUSCATALOG  cmd='progress ; addcol CoordHashCode concat(toString(hmsToRadians(RAJ2000)),toString(dmsToRadians(DEJ2000)))' cmd='progress ; sort CoordHashCode' out=$CATALOG
newStep "Setting empty catalog identifiers to NaN" stilts tpipe in=$PREVIOUSCATALOG cmd='progress ; replaceval "" NaN HIP' cmd='progress ; replaceval "" NaN HD' cmd='progress ; replaceval "" NaN DM' out=$CATALOG
newStep "Creating catalog identifier columns as 'double' values" stilts tpipe in=$PREVIOUSCATALOG cmd='progress ; addcol _HIP parseDouble(HIP)' cmd='progress ; addcol _DM parseDouble(DM)' cmd='progress ; addcol _HD parseDouble(HD)' out=$CATALOG
newStep "Flagging duplicated _HIP entries" stilts tmatch1 in=$PREVIOUSCATALOG matcher=exact values='_HIP' out=$CATALOG
newStep "Renaming GroupId and GroupSize to HIPGroupId and HIPGroupSize" stilts tpipe in=$PREVIOUSCATALOG cmd='progress ; colmeta -name HIPGroupID GroupID' cmd='progress ; colmeta -name HIPGroupSize GroupSize' out=$CATALOG
newStep "Flagging duplicated _HD entries" stilts tmatch1 in=$PREVIOUSCATALOG matcher=exact values='_HD' out=$CATALOG
newStep "Renaming GroupId and GroupSize to HDGroupId and HDGroupSize" stilts tpipe in=$PREVIOUSCATALOG cmd='progress ; colmeta -name HDGroupID GroupID' cmd='progress ; colmeta -name HDGroupSize GroupSize' out=$CATALOG
newStep "Flagging duplicated _DM entries" stilts tmatch1 in=$PREVIOUSCATALOG matcher=exact values='_DM' out=$CATALOG
newStep "Renaming GroupId and GroupSize to DMGroupId and DMGroupSize" stilts tpipe in=$PREVIOUSCATALOG cmd='progress ; colmeta -name DMGroupID GroupID' cmd='progress ; colmeta -name DMGroupSize GroupSize' out=$CATALOG

#########################################################
# AT THIS LEVEL THE CATALOG HAS KEPT ALL PERTINENT DATA #
#########################################################

newStep "Clearing query-specific 'dist' column" stilts tpipe in=$PREVIOUSCATALOG  cmd='progress ; delcols dist; addcol -before dist.ORIGIN dist NULL ' out=$CATALOG
newStep "Rejecting fully duplicated lines" stilts tpipe in=$PREVIOUSCATALOG   cmd='progress ; uniq -count' cmd='progress ; colmeta -name DuplicatedLines DupCount' out=$CATALOG
newStep "Removing duplicated catalog identifiers rows" stilts tpipe in=$PREVIOUSCATALOG cmd='progress; select NULL_HIPGroupSize' cmd='progress; select NULL_HDGroupSize' cmd='progress; select NULL_DMGroupSize' out=$CATALOG

case $FILTERING_STYLE in
    CDS ) # CDS fitering
        newStep "Rejecting stars with low confidence on 'DIAM_VK'" stilts tpipe in=$PREVIOUSCATALOG  cmd='progress ; select equals(diam_vk.confidence,\"HIGH\")' out=$CATALOG ;
        newStep "Rejecting stars with SB9 references" stilts tpipe in=$PREVIOUSCATALOG  cmd='progress ; select NULL_SBC9' out=$CATALOG ;
        newStep "Rejecting stars with WDS references" stilts tpipe in=$PREVIOUSCATALOG  cmd='progress ; select NULL_WDS' out=$CATALOG ;
        newStep "Adding a flag column for R provenance" stilts tpipe in=$PREVIOUSCATALOG  cmd='progress ; addcol f-Rmag NULL_R.confidence?1:0' out=$CATALOG ;
        newStep "Adding a flag column for I provenance" stilts tpipe in=$PREVIOUSCATALOG  cmd='progress ; addcol f-Imag NULL_I.confidence?1:0' out=$CATALOG ;
        newStep "Adding the 'Name' column" stilts tpipe in=$PREVIOUSCATALOG  cmd='progress ; addcol Name !equals(HIP,\"NaN\")?\"HIP\"+HIP:(!NULL_TYC2?\"TYC\"+TYC2:\"\")' out=$CATALOG ;
        
        # Columns deletion (to force output of computed values in 'sclsvr' over retrieved one in 'vobs')
        newStep "Removing unwanted column Teff" stilts tpipe in=$PREVIOUSCATALOG cmd='delcols "Teff"' out=$CATALOG ;
        newStep "Removing unwanted column UDDK" stilts tpipe in=$PREVIOUSCATALOG cmd='delcols "UDDK"' out=$CATALOG ;
        
        # Columns renaming
        OLD_NAMES=( pmRa  pmDec  B     V     R     I     J     H     K     diam_vk  e_diam_vk  UD_B  UD_V  UD_R  UD_I  UD_J  UD_H  UD_K  e_Plx  jTeff        LogG ) ;
        NEW_NAMES=( pmRA  pmDEC  Bmag  Vmag  Rmag  Imag  Jmag  Hmag  Kmag  LDD      e_LDD      UDDB  UDDV  UDDR  UDDI  UDDJ  UDDH  UDDK  e_plx  Teff_SpType  logg_SpType ) ;
        i=0 ;
        for OLD_NAME in ${OLD_NAMES[*]}
        do
            NEW_NAME=${NEW_NAMES[i]} ;
            newStep "Renaming column '${OLD_NAME}' to '${NEW_NAME}'" stilts tpipe in=$PREVIOUSCATALOG cmd="progress ; colmeta -name ${NEW_NAME} ${OLD_NAME}" out=$CATALOG ;
            let "i=$i+1" ;
        done
        
        newStep "Applying final columns set" stilts tpipe in=$PREVIOUSCATALOG cmd='keepcols "Name RAJ2000 DEJ2000 pmRA pmDEC Bmag Vmag Rmag f-Rmag Imag f-Imag Jmag Hmag Kmag LDD e_LDD UDDB UDDV UDDR UDDI UDDJ UDDH UDDK plx e_plx SpType Teff_SpType logg_SpType"' out=$CATALOG ;
        ;;

    ESO ) # ESO fitering
        newStep "Removing stars with DEC < 40 " stilts tpipe in=$PREVIOUSCATALOG cmd='progress; select "dmsToRadians(DEJ2000) < degreesToRadians(40)"' out=$CATALOG ;
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
