#!/bin/bash
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
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


# Perform the given command, outputing the given comment, skipping if input file is older
newStep()
{
    ACTIONDESC=$1
    shift
    ACTIONCMD=$*
    echo
    echo -n "$(date +'%Y-%m-%dT%H-%M-%S')  -  Step $PHASE ($PREVIOUSCATALOG -> $CATALOG) : $ACTIONDESC ... "  


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

# Display java config info

echo "Java version:"
java -version

STILTS_JAVA_OPTIONS=" -Xms1024m -Xmx2048m "
echo "Stilts options:"
echo "$STILTS_JAVA_OPTIONS"
echo 

# Starting real job

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
        newStep "Rejecting stars with WDS references" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG  cmd='progress ; select !(sep1<2||sep2<2)' out=$CATALOG ;
        newStep "Rejecting stars with MultFlag=S" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG  cmd='progress ; select !contains(\"\"+MultFlag,\"S\")' out=$CATALOG ;
        newStep "Adding a flag column for R provenance" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG  cmd='progress ; addcol f_Rmag NULL_R.confidence?1:0' out=$CATALOG ;
        newStep "Adding a flag column for I provenance" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG  cmd='progress ; addcol f_Imag NULL_I.confidence?1:0' out=$CATALOG ;
        newStep "Adding the 'Name' column" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG  cmd='progress ; addcol Name !equals(HIP,\"NaN\")?\"HIP\"+HIP:(!(NULL_TYC1||NULL_TYC2||NULL_TYC3)?\"TYC\"+TYC1+\"-\"+TYC2+\"-\"+TYC3:\"\")' out=$CATALOG ;
        
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
        #newStep "Removing stars with DEC < 40 " stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG cmd='progress; select "dmsToRadians(DEJ2000) < degreesToRadians(40)"' out=$CATALOG ;
        newStep "Removing star with RA=18 56 44.3 " stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG cmd='progress; select !equals(RAJ2000,\"18\ 56\ 44.3\")' out=$CATALOG ;
       
        #
        # Add special simbad filtering until wds and sbc9 coordinates fixes
        #
        newStep "Adding the 'Name' column to use one simbad script" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG  cmd='progress ; addcol Name !equals(HIP,\"NaN\")?\"HIP\"+HIP:!equals(HD,\"NaN\")?\"HD\"+HD:(!(NULL_TYC1||NULL_TYC2||NULL_TYC3)?\"TYC\"+TYC1+\"-\"+TYC2+\"-\"+TYC3:\"\"+RAJ2000+\"\ \"+DEJ2000)' out=$CATALOG ;
      
        # Extract star identifiers and prepare one simbad batch script
        if [ simbad.txt -ot $PREVIOUSCATALOG ]
        then
            echo "Querying simbad by script"
            stilts tcat icmd="keepcols Name" in=$PREVIOUSCATALOG out=names.csv
            csplit names.csv 2
            POST_FILE=postfile.txt
            echo -e "votable {\n MAIN_ID\n COO\n id(SBC9)\n id(WDS)\n }\nvotable open\nset radius 1s" > $POST_FILE
            cat xx01 >> $POST_FILE
            echo "votable close" >> $POST_FILE
            curl -F "scriptFile=@$POST_FILE" -F "submit=submit file" -F "fileOutput=on" http://simbad.u-strasbg.fr/simbad/sim-script > simbad.txt
            cat simbad.txt | awk '{if(write==1)print;if(start==1)write=1;if ($1=="::data::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::")start=1;}' > simbad.vot
            stilts tcat omode=stats in=simbad.vot 
        fi

        newStep "Crossmatch with simbad votable" stilts ${STILTS_JAVA_OPTIONS} tmatch2 matcher='exact' values1='$0' values2='$0' join='all1' in1=$PREVIOUSCATALOG in2=simbad.vot out=$CATALOG ;
        newStep "Add new column with first WDS_id" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG out=$CATALOG cmd='addcol WDS_id matchGroup(ID_2a,\"WDS\ J([0-9+-]*)\")'

        if [ ! -e "WDS.vot" ]
        then
            echo "Extract wds catalog";
        curl -o WDS.vot 'http://vizier.u-strasbg.fr/viz-bin/votable?-to=4&-from=-2&-this=-2&-out.max=unlimited&-out.form=VOTable&-order=I&-c=&-c.eq=J2000&-oc.form=dec&-c.r=+10&-c.u=arcsec&-c.geom=r&-out.add=_r&-out.add=_RA*-c.eq%2C_DE*-c.eq&-out.add=_RAJ%2C_DEJ&-sort=_r&-source=B%2Fwds%2Fwds&-out=WDS&WDS=&-out=Disc&Disc=&-out=Comp&Comp=&-out=Obs1&Obs1=&-out=pa1&pa1=&-out=sep1&sep1=&-out=sep2&sep2=&-out=mag1&mag1=&-out=mag2&mag2=&-out=DM&DM=&-out=Notes&Notes=&-out=n_RAJ2000&n_RAJ2000=&-out=RAJ2000&RAJ2000=&-out=DEJ2000&DEJ2000=&-meta.ucd=u&-ref=VIZ4cb6b0cb2a6e&-file=.&-meta=0&-meta.foot=1'
        fi

        # keep only  WDS with sep1 or sep2 <= 2
        if [ ! -e "WDSToRemove.vot" ]
        then
            echo "keep only  WDS with sep1 or sep2 <= 2"
            stilts tpipe in='WDS.vot' out='WDSToRemove.vot' cmd='select (sep1<=2\ ||\ sep2<=2)' cmd='keepcols WDS'
        fi
        
        newStep "Crossmatch with wds to remove" stilts ${STILTS_JAVA_OPTIONS} tmatch2 matcher='exact' values1='WDS_id' values2='WDS' join='all1' in1=$PREVIOUSCATALOG in2=WDSToRemove.vot out=$CATALOG ;

        newStep "Reject bad WDS computed with simbad and clean temporary columns" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG out=$CATALOG cmd='select (NULL_ID_2\ &&\ NULL_WDS_2)' cmd='delcols $380\ $381\ $382\ $383\ $384\ $385\ $386\ $387\ $388\ $389\ $390\ $391\ $392\ $393\ $394\ $395\ $396\ $397' #\ $398\ $399\ $400'
        
        newStep "Removing remaining stars sbc9 and wds with sep1 or sep2 < 2 " stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG cmd='progress; select !(!NULL_SBC9||sep1<2||sep2<2)' out=$CATALOG ;

        ;;
    * ) # Unknown filtering
        printUsage ;;
esac

out="final.fits"
echo "Final results are available in ${out} ... DONE."
cp $PREVIOUSCATALOG ${out}

# TODO check that no star exists with duplicated coords using one of the next
# filters....
#newStep "Reject stars with duplicated coordinates (first or last is kept)" stilts tpipe in=$PREVIOUSCATALOG cmd='progress ; sort CoordHashCode' cmd='progress ; uniq -count CoordHashCode' out=$CATALOG
#stilts tmatch1 in=catalog3.fits matcher=2d values='_RAJ2000 _DEJ2000' params=0 out=sameCoords2.fits
