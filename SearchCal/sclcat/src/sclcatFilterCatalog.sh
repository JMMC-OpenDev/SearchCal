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
    echo -e "Usage: sclcatFilterCatalog <-h> <xxx-ref|xxx-run-YYYY-MM-DDTHH-MM-SS> \"<short catalog description>\"" 
    echo -e "\t-h\t\tprint this help."
    echo -e "\t<dir-run>\tFilter results in dir-run directory"
    echo -e "\t<short catalog description>\tShort catalog description"
    exit 1;
}

# log function
# perform echo on given parameters and tee it on logfile
function logInfo() {
    echo -e $@ |tee -a "$LOGFILE"
}

# Perform the given command, outputing the given comment, skipping if input file is older
newStep()
{
    ACTIONDESC=$1
    shift
    ACTIONCMD=$*
    logInfo
    logInfo -ne "$(date +'%Y-%m-%dT%H-%M-%S')  -  Step $PHASE ($PREVIOUSCATALOG -> $CATALOG) : $ACTIONDESC ... "  


    # Perform the given command only if previous catalog has changed since last computation
    if [ $PREVIOUSCATALOG -nt $CATALOG ]
    then
        logInfo
        # KEEP quotes arround $@ to ensure a correct arguments process
        logInfo performing : "$@"
        "$@" | tee -a $LOGFILE
        if [ $? -eq 0 ]
        then
            logInfo "DONE."
        else
            logInfo "FAILED (using previous catalog instead)."
        fi
        
        if [ $PREVIOUSCATALOG -nt $CATALOG ]
        then
            cp $PREVIOUSCATALOG $CATALOG
        fi

        stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CATALOG omode='count' | tee -a $LOGFILE
    else
        logInfo "SKIPPED."
    fi

    let PREVIOUSPHASE=$PHASE
    let PHASE=$PHASE+1
    PREVIOUSCATALOG=catalog$PREVIOUSPHASE.fits
    CATALOG=catalog$PHASE.fits
}

removeWdsSb9WithSimbadCrossMatch()
{

    # Remove black listed stars before simbad query
    BLACKLISTPARAM=$(grep -v "#" $SIMBADBLACKLIST)
    logInfo "Actual simbad blacklist: $BLACKLISTPARAM"
    newStep "Remove blacklisted stars to avoid error querying simbad" stilts ${STILTS_JAVA_OPTIONS} tpipe cmd='select !matches(Name,\"^('$BLACKLISTPARAM')$\")' in=$PREVIOUSCATALOG out=$CATALOG ;

    read -p "go simbad or ctrl C"
    # Extract star identifiers and prepare one simbad batch script
    if [ simbad.txt -ot "$PREVIOUSCATALOG" -o ! -e simbad.vot -o simbad.txt -ot $SIMBADBLACKLIST ]
    then
        logInfo "Querying simbad by script"
        stilts tcat icmd="keepcols Name" in=$PREVIOUSCATALOG out=names.csv
        csplit names.csv 2
        POST_FILE=postfile.txt
        echo -e "votable {\n MAIN_ID\n COO\n id(TYC)\n id(HIP)\n id(HD)\n id(SBC9)\n id(WDS)\n }\nvotable open\nset radius 1s" > $POST_FILE
        cat xx01 >> $POST_FILE
        echo "votable close" >> $POST_FILE
        curl -F "scriptFile=@$POST_FILE" -F "submit=submit file" -F "fileOutput=on" http://simbad.u-strasbg.fr/simbad/sim-script > simbad.txt
        cat simbad.txt | awk '{if(write==1)print;if(start==1)write=1;if ($1=="::data::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::")start=1;}' > simbad.vot
        stilts tcat omode=stats in=simbad.vot 
    fi
   
    # Check that size of input and ouptut list are the same
    SIMBADCNT=$( stilts tcat omode=count in=simbad.vot )
    pwd
    ls $PREVIOUSCATALOG
    CATCNT=$( stilts tcat omode=count in=$PREVIOUSCATALOG )
    if [ "${SIMBADCNT/*:/}" != "${CATCNT/*:/}" ]
    then
        logInfo "WARNING: SIMBAD does not return the same stars number as given in input"
        logInfo "  Please add following stars in '$SIMBADBLACKLIST' (check exact name grepping $PWD/names.csv file)"
        logInfo " then run the same command  again"
        BADLIST=$(grep "Identifier not found in the database" simbad.txt | awk 'match($0,":"){print substr($0,RSTART+1)}'|tr -d ' ')
        echo $BADLIST | tr " " "|"
        mv -v simbad.vot oldsimbad.vot
        mv -v simbad.txt oldsimbad.txt
        rm $PREVIOUSCATALOG

        exit 1
    fi

    newStep "Crossmatch with simbad votable" stilts ${STILTS_JAVA_OPTIONS} tmatch2 matcher='exact' values1='$0' values2='$0' join='all1' in1=$PREVIOUSCATALOG in2=simbad.vot out=$CATALOG ;
    
    newStep "Add new column with first WDS_id" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG out=$CATALOG cmd='addcol WDS_id matchGroup(ID_2d,\"WDS\ J([0-9+-]*)\")'

    if [ "WDS.vot" -ot "$PREVIOUSCATALOG" ]
    then
        logInfo "Extract wds catalog";
curl -o WDS.vot 'http://vizier.u-strasbg.fr/viz-bin/votable?-to=4&-from=-2&-this=-2&-out.max=unlimited&-out.form=VOTable&-order=I&-c=&-c.eq=J2000&-oc.form=dec&-c.r=+10&-c.u=arcsec&-c.geom=r&-out.add=_r&-out.add=_RA*-c.eq%2C_DE*-c.eq&-out.add=_RAJ%2C_DEJ&-sort=_r&-source=B%2Fwds%2Fwds&-out=WDS&WDS=&-out=Disc&Disc=&-out=Comp&Comp=&-out=Obs1&Obs1=&-out=pa1&pa1=&-out=sep1&sep1=&-out=sep2&sep2=&-out=mag1&mag1=&-out=mag2&mag2=&-out=DM&DM=&-out=Notes&Notes=&-out=n_RAJ2000&n_RAJ2000=&-out=RAJ2000&RAJ2000=&-out=DEJ2000&DEJ2000=&-meta.ucd=u&-ref=VIZ4cb6b0cb2a6e&-file=.&-meta=0&-meta.foot=1'
    fi

# keep only  WDS with sep1 or sep2 <= 2
    if [ "WDSToRemove.vot" -ot "WDS.vot" ]
    then
        logInfo "keep only  WDS with sep1 or sep2 <= 2"
        stilts tpipe in='WDS.vot' out='WDSToRemove.vot' cmd='select (sep1<=2\ ||\ sep2<=2)' cmd='keepcols WDS' cmd='colmeta -name WDS_ORIG WDS'
    fi

    newStep "Crossmatch with wds to remove" stilts ${STILTS_JAVA_OPTIONS} tmatch2 matcher='exact' values1='WDS_id' values2='WDS_ORIG' join='all1' in1=$PREVIOUSCATALOG in2=WDSToRemove.vot out=$CATALOG ;
    
#    newStep "Reject SBC9 computed with simbad" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG out=$CATALOG cmd='select NULL_ID_2c'
    newStep "Reject bad WDS computed with simbad" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG out=$CATALOG cmd='select ((!NULL_sep1&&!NULL_WDS_ORIG)||NULL_WDS_ORIG); delcols "WDS_ORIG WDS_id RA DEC MAIN_ID ID_* sep1"'
}


# Command line options parsing
if [ $# -lt 1 ] # Always at least 1 option specified
then
    printUsage
fi
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

# Define global variables
SIMBADBLACKLIST=$PWD/../config/sclcatSimbadBlackList.txt

# Parse command-line parameters
dir=$1
if [ ! -d "$dir" ]
then
    printUsage
else
    echo "Filtering results from $dir"
fi
shortDesc=$2
if [  -z "$shortDesc" ]
then
    echo "Missing <shortDesc>"
    printUsage
else
    echo "Using short description: $shortDesc"
fi


LOGFILE=filter.log

# Display java config info

echo "Most outputs are appended to '$LOGFILE'"
logInfo "Starting new Filtering on '$(date)'"
logInfo "Directory to analyse = '$dir'"
logInfo "PWD = '$(pwd)'"
logInfo "Java version:"
java -version |tee -a "$LOGFILE"

STILTS_JAVA_OPTIONS=" -Xms1024m -Xmx2048m "
logInfo "Stilts options:"
logInfo "$STILTS_JAVA_OPTIONS"
logInfo 

if [ ! -e "$SIMBADBLACKLIST" ]
then
    logInfo "Blacklist file not found : should be here '$SIMBADBLACKLIST'"
    exit 1
fi
logInfo "Using '$SIMBADBLACKLIST' file to get proper simbad script filtering"

# Starting real job

# Moving to the right directory
if ! cd "${dir}/result/"
then
  logInfo "You should have one 'result' directory in '$dir' for filtering"
  exit 1
fi

echo "$shortDesc" > shortDesc.txt


# First initialization
let PHASE=0
PREVIOUSCATALOG=catalog.vot

# Test if first input catalog is present
if [ ! -e "$PREVIOUSCATALOG" ]
then
    logInfo "You should have one '$PREVIOUSCATALOG' catalog in '$dir/result' for filtering"
    exit 1
fi




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
# Columns deletion (to force output of computed values in 'sclsvr' over retrieved one in 'vobs')
newStep "Removing unwanted column UDDK" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG cmd='delcols "UDDK"' out=$CATALOG ;
newStep "Rejecting fully duplicated lines" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG   cmd='progress ; uniq -count' cmd='progress ; colmeta -name DuplicatedLines DupCount' out=$CATALOG
# newStep "Removing duplicated catalog identifiers rows" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG cmd='progress; select NULL_HIPGroupSize' cmd='progress; select NULL_HDGroupSize' cmd='progress; select NULL_DMGroupSize' out=$CATALOG

newStep "Rejecting stars with low confidence on 'DIAM_VK'" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG  cmd='progress ; select equals(diam_vk.confidence,\"HIGH\")' out=$CATALOG ;
newStep "Rejecting stars with e_plx/plx>.25" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG  cmd='progress ; select !((e_plx/plx)>0.25)' out=$CATALOG ;
newStep "Rejecting stars with SB9 references" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG  cmd='progress ; select NULL_SBC9' out=$CATALOG ;
newStep "Rejecting stars with WDS references" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG  cmd='progress ; select !(sep1<2||sep2<2)' out=$CATALOG ;
logInfo "Get badcal catalog" ; curl -o badcal.vot 'http://apps.jmmc.fr/badcal-dsa/SubmitCone?DSACATTAB=badcal.valid_stars&RA=0.0&DEC=0.0&SR=360.0' ;
newStep "Rejecting badcal stars" stilts ${STILTS_JAVA_OPTIONS} tskymatch2 ra1='radiansToDegrees(hmsToRadians(RAJ2000))' ra2='ra' dec1='radiansToDegrees(dmsToRadians(DEJ2000))' dec2='dec' error=1 join="1not2" find="all" out="$CATALOG" $PREVIOUSCATALOG  badcal.vot
newStep "Rejecting stars with MultFlag=S" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG  cmd='progress ; select !contains(\"\"+MultFlag,\"S\")' out=$CATALOG ;
newStep "Adding a flag column for R provenance" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG  cmd='progress ; addcol f_Rmag NULL_R.confidence?1:0' out=$CATALOG ;
newStep "Adding a flag column for I provenance" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG  cmd='progress ; addcol f_Imag NULL_I.confidence?1:0' out=$CATALOG ;
newStep "Adding the 'Name' column to use one simbad script" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG  cmd='progress ; addcol Name !equals(HIP,\"NaN\")?\"HIP\"+HIP:!equals(HD,\"NaN\")?\"HD\"+HD:(!(NULL_TYC1||NULL_TYC2||NULL_TYC3)?\"TYC\"+TYC1+\"-\"+TYC2+\"-\"+TYC3:\"\"+RAJ2000+\"\ \"+DEJ2000)' out=$CATALOG ;
newStep "Flagging duplicated Name entries" stilts ${STILTS_JAVA_OPTIONS} tmatch1 in=$PREVIOUSCATALOG matcher=exact values='Name' out=$CATALOG
newStep "Removing duplicated Name entries" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG cmd='progress ; colmeta -name NameGroupID GroupID' cmd='progress ; colmeta -name NameGroupSize GroupSize' cmd='progress; select NULL_NameGroupSize' out=$CATALOG


# Columns renaming
OLD_NAMES=( pmRa  pmDec  B     V     R     I     J     H     K     N     diam_vk  e_diam_vk  UD_B  UD_V  UD_R  UD_I  UD_J  UD_H  UD_K UD_N  e_Plx ) ;
NEW_NAMES=( pmRA  pmDEC  Bmag  Vmag  Rmag  Imag  Jmag  Hmag  Kmag  Nmag  LDD      e_LDD      UDDB  UDDV  UDDR  UDDI  UDDJ  UDDH  UDDK UDDN  e_plx ) ;
i=0 ;
RENAME_EXPR=""
for OLD_NAME in ${OLD_NAMES[*]}
do
NEW_NAME=${NEW_NAMES[i]} ;
let "i=$i+1" ;
RENAME_EXPR="${RENAME_EXPR}; colmeta -name ${NEW_NAME} ${OLD_NAME}"
done
newStep "Renaming column from \n'${OLD_NAMES[*]}' to \n'${NEW_NAMES[*]}'" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG cmd="progress ${RENAME_EXPR}" out=$CATALOG ;

COLUMNS_SET="Name RAJ2000 DEJ2000 pmRA pmDEC Bmag Vmag Rmag f_Rmag Imag f_Imag Jmag Hmag Kmag Nmag LDD e_LDD UDDB UDDV UDDR UDDI UDDJ UDDH UDDK UDDN plx e_plx SpType Teff_SpType logg_SpType diam_mean e_diam_mean sep1" ;
newStep "Keeping final columns set (plus diam_mean e_diam_mean Nmag and UDDN) and sep1 " stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG cmd="keepcols \"${COLUMNS_SET}\"" out=$CATALOG ;
EMPTY_FILTER=""
for COLUMN_NAME in ${COLUMNS_SET}
do
# leave some N values empty
if echo "$COLUMN_NAME" |grep -v Nmag |grep -v UDDN |grep -v sep1 &> /dev/null
then
EMPTY_FILTER="${EMPTY_FILTER}!NULL_${COLUMN_NAME}&&"
fi
done
EMPTY_FILTER="select \"${EMPTY_FILTER}true \""
# filter using built expression
logInfo "Computed filter expression for comming step: $EMPTY_FILTER"
newStep "Rejecting stars with empty cells " stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG  cmd="progress ; ${EMPTY_FILTER}" out=$CATALOG ;

# Add special simbad filtering until wds and sbc9 coordinates fixes
#removeWdsSb9WithSimbadCrossMatch

newStep "Clean useless params of catalog " stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG  cmd="setparam Description \"\"; setparam objectName \"\"; setparam -ref \"\"; setparam -out.max \"\"" out=$CATALOG ;

out="final.fits"
logInfo "Final results are available in ${out} ... DONE."
cp $PREVIOUSCATALOG ${out}

# TODO check that no star exists with duplicated coords using one of the next
# filters....
#newStep "Reject stars with duplicated coordinates (first or last is kept)" stilts tpipe in=$PREVIOUSCATALOG cmd='progress ; sort CoordHashCode' cmd='progress ; uniq -count CoordHashCode' out=$CATALOG
#stilts tmatch1 in=catalog3.fits matcher=2d values='_RAJ2000 _DEJ2000' params=0 out=sameCoords2.fits
