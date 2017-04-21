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
# */

# Print usage 
function printUsage () {
    echo -e "Usage: sclcatFilterCatalog <-h> <xxx-ref|xxx-run-YYYY-MM-DDTHH-MM-SS> \"<short catalog description>\"" 
    echo -e "\t-h\t\tprint this help."
    echo -e "\t<dir-run>\tFilter results in dir-run directory"
    echo -e "\t<short catalog description>\tShort catalog description"
    exit 1;
}

function exitRequested() {
    echo "Ctrl^C as been requested, aborting process"
    exit 1
}

# log function
# perform echo on given parameters and tee it on logfile
function logInfo() {
    echo -e "$(date +'%Y-%m-%dT%H-%M-%S')  -  $@" |tee -a "$LOGFILE"
}

# Perform the given command, outputing the given comment, skipping if input file is older
newStep()
{
    ACTIONDESC=$1
    shift
    ACTIONCMD=$*
    logInfo
    logInfo "Step $PHASE ($PREVIOUSCATALOG -> $CATALOG) : $ACTIONDESC ... "  


    # Perform the given command only if previous catalog has changed since last computation
    if [ $PREVIOUSCATALOG -nt $CATALOG ]
    then
        # KEEP quotes arround $@ to ensure a correct arguments process
        logInfo performing : "$@"
        "$@" | tee -a $LOGFILE
        if [ ${PIPESTATUS[0]} -eq 0 ]
        then
            logInfo "DONE."
        else
            logInfo "FAILED (using previous catalog instead)."
	        exit 1
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

genMetaAndStats(){
INFILE="$1"
for omode in meta stats;
do
    OUTFILE="${INFILE}.${omode}.txt"
    if [ "${OUTFILE}" -ot "${INFILE}" ]
    then
        logInfo "Producing '$omode' file for '${INFILE}' into '${OUTFILE}'"
        stilts ${STILTS_JAVA_OPTIONS} tcat omode="$omode" in="${INFILE}" > "${OUTFILE}" ;
    fi
done
}

###############################################################################
#  end of function implementations                                            #
#  start main business                                                        #
###############################################################################


# trap control C to exit on first control^C
trap exitRequested 2 

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
PATH="$PATH:$PWD/../bin"

# Define global variables
SIMBADBLACKLIST="$PWD/../config/sclcatSimbadBlackList.txt"

# Parse command-line parameters
dir=$1
if [ ! -d "$dir" ]
then
    printUsage
else
    echo "Filtering results from $dir"
fi
shortDesc="$2"
if [  -z "$shortDesc" ]
then
    echo "Missing <shortDesc>"
    printUsage
else
    echo "Using short description: $shortDesc"
fi

# define some filename constants
LOGFILE="filter.log"
FINAL_FITS_FILENAME="final.fits"
INTERMEDIATE_JSDC_FILENAME="filtered.vot"

# Display java config info

echo "Most outputs are appended to '$LOGFILE'"
logInfo "Starting new Filtering on '$(date)'"
logInfo "Directory to analyse = '$dir'"
logInfo "PWD = '$(pwd)'"
logInfo "Java version:"
java -version |tee -a "$LOGFILE"

# Use large heap + CMS GC:
STILTS_JAVA_OPTIONS=" -Xms4096m -Xmx4096m -XX:+UseConcMarkSweepGC"
logInfo "Stilts options:"
logInfo "$STILTS_JAVA_OPTIONS"
logInfo 

# Starting real job

# Moving to the right directory
if ! cd "${dir}/result/"
then
  logInfo "You should have one 'result' directory in '$dir' for filtering"
  exit 1
fi

echo "$shortDesc" > shortDesc.txt
# TODO store shortDesc as a votable param

# First initialization
let PHASE=0
PREVIOUSCATALOG=catalog.vot

# Test if first input catalog is present
if [ ! -e "$PREVIOUSCATALOG" ]
then
    logInfo "You should have one '$PREVIOUSCATALOG' catalog in '$dir/result' for filtering"
    exit 1
fi


# Pipeline get 3 main steps
# 1/ Convert votable produced by sclsvr into fits ( for performances and file disk size saving )
# 2/ Filter raw catalog leaving all columns to produce a filtered JSDC VOTABLE
# 3/ Format add / remove columns to produce CDS JSDC catalog

#
# PIPELINE STEP 1 : convert Votable to fits
#

genMetaAndStats "${PREVIOUSCATALOG}"

CATALOG=catalog0.fits
newStep "Convert raw VOTable catalog to FITS" stilts ${STILTS_JAVA_OPTIONS} tcopy $PREVIOUSCATALOG $CATALOG

#
# PIPELINE STEP 2 : filter rows
#

#############################################################
# PRELIMINARY SELECTION : diamFlag must be true #
#############################################################
newStep "Keep stars with diamFlag==1" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG  cmd='progress ; select diamFlag' out=$CATALOG


#################################
# Check exact duplicates on RA/DEC coordinates, HIP, HD, DM
#################################

# JSDC scenario should not have duplicates : uncomment next line to double-check 
#CHECKDUPLICATES=on

if [ "${CHECKDUPLICATES}" = "on" ]
then
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

    # Remove duplicated lines (old JSDC method : mozaic)
    newStep "Rejecting fully duplicated lines" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG   cmd='progress ; uniq -count' cmd='progress ; colmeta -name DuplicatedLines DupCount' out=$CATALOG
    newStep "Removing duplicated catalog identifiers rows" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG cmd='progress; select NULL_HIPGroupSize' cmd='progress; select NULL_HDGroupSize' cmd='progress; select NULL_DMGroupSize' out=$CATALOG

    # newStep "Reject stars with duplicated coordinates (first or last is kept)" stilts tpipe in=$PREVIOUSCATALOG cmd='progress ; sort CoordHashCode' cmd='progress ; uniq -count CoordHashCode' out=$CATALOG
fi


# JSDC 2017.4 already have CalFlag : uncomment next line to double-check 
#ADD_CAL_FLAG=on

if [ "${ADD_CAL_FLAG}" = "on" ]
then
    # Add column CalFlag:
    # bit 1: chi2 > 5
    newStep "Adding CalFlag column" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG  cmd='progress ; addcol CalFlag (int)((diam_chi2>5.)?1:0)' out=$CATALOG

    # Filter SB9 and WDS
    # bit 2: binary (SBC9 or WDS)
    newStep "Marking stars with SB9 and WDS references" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG  cmd='progress ; replacecol CalFlag (CalFlag+((!NULL_SBC9||sep1<=2.||sep2<=2.)?2:0))' out=$CATALOG ;


    # Filter ObjTypes:
    # Generated from ObjectTypes_2017.ods
    # bit 4: bad object type
    newStep "Marking stars with bad simbad otypes" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG \
    cmd='progress ; replacecol CalFlag (CalFlag+((contains(ObjTypes,\",EB?,\")||contains(ObjTypes,\",Sy?,\")||contains(ObjTypes,\",CV?,\")||contains(ObjTypes,\",No?,\")||contains(ObjTypes,\",pr?,\")||contains(ObjTypes,\",TT?,\")||contains(ObjTypes,\",C*?,\")||contains(ObjTypes,\",S*?,\")||contains(ObjTypes,\",OH?,\")||contains(ObjTypes,\",CH?,\")||contains(ObjTypes,\",WR?,\")||contains(ObjTypes,\",Ae?,\")||contains(ObjTypes,\",RR?,\")||contains(ObjTypes,\",Ce?,\")||contains(ObjTypes,\",LP?,\")||contains(ObjTypes,\",Mi?,\")||contains(ObjTypes,\",sv?,\")||contains(ObjTypes,\",pA?,\")||contains(ObjTypes,\",WD?,\")||contains(ObjTypes,\",N*?,\")||contains(ObjTypes,\",BH?,\")||contains(ObjTypes,\",SN?,\")||contains(ObjTypes,\",BD?,\")||contains(ObjTypes,\",EB*,\")||contains(ObjTypes,\",Al*,\")||contains(ObjTypes,\",bL*,\")||contains(ObjTypes,\",WU*,\")||contains(ObjTypes,\",EP*,\")||contains(ObjTypes,\",SB*,\")||contains(ObjTypes,\",El*,\")||contains(ObjTypes,\",Sy*,\")||contains(ObjTypes,\",CV*,\")||contains(ObjTypes,\",NL*,\")||contains(ObjTypes,\",No*,\")||contains(ObjTypes,\",DN*,\")||contains(ObjTypes,\",Ae*,\")||contains(ObjTypes,\",C*,\")||contains(ObjTypes,\",S*,\")||contains(ObjTypes,\",pA*,\")||contains(ObjTypes,\",WD*,\")||contains(ObjTypes,\",ZZ*,\")||contains(ObjTypes,\",BD*,\")||contains(ObjTypes,\",N*,\")||contains(ObjTypes,\",OH*,\")||contains(ObjTypes,\",CH*,\")||contains(ObjTypes,\",pr*,\")||contains(ObjTypes,\",TT*,\")||contains(ObjTypes,\",WR*,\")||contains(ObjTypes,\",Ir*,\")||contains(ObjTypes,\",Or*,\")||contains(ObjTypes,\",RI*,\")||contains(ObjTypes,\",Er*,\")||contains(ObjTypes,\",FU*,\")||contains(ObjTypes,\",RC*,\")||contains(ObjTypes,\",RC?,\")||contains(ObjTypes,\",Psr,\")||contains(ObjTypes,\",BY*,\")||contains(ObjTypes,\",RS*,\")||contains(ObjTypes,\",Pu*,\")||contains(ObjTypes,\",RR*,\")||contains(ObjTypes,\",Ce*,\")||contains(ObjTypes,\",dS*,\")||contains(ObjTypes,\",RV*,\")||contains(ObjTypes,\",WV*,\")||contains(ObjTypes,\",bC*,\")||contains(ObjTypes,\",cC*,\")||contains(ObjTypes,\",gD*,\")||contains(ObjTypes,\",SX*,\")||contains(ObjTypes,\",LP*,\")||contains(ObjTypes,\",Mi*,\")||contains(ObjTypes,\",sr*,\")||contains(ObjTypes,\",SN*,\"))?4:0))' \
    out=$CATALOG ;
fi


# Get BadCal Votable if not present and fresh
if [ "${PREVIOUSCATALOG}" -nt "badcal.vot" ] 
then
    logInfo "Get badcal catalog"
    curl -o badcal.vot 'http://apps.jmmc.fr/badcal-dsa/SubmitCone?DSACATTAB=badcal.valid_stars&RA=0.0&DEC=0.0&SR=360.0' ; 
else 
    logInfo "Badcal catalog already present"
fi
# TODO: mark bad cals ?
newStep "Rejecting badcal stars" stilts ${STILTS_JAVA_OPTIONS} tskymatch2 ra1='radiansToDegrees(hmsToRadians(RAJ2000))' ra2='ra' dec1='radiansToDegrees(dmsToRadians(DEJ2000))' dec2='dec' error=5 join="1not2" find="all" out="$CATALOG" $PREVIOUSCATALOG  badcal.vot


# store an intermediate JSDC votable since all row filters should have been applied 
# and produce stats and meta reports
if [ "${PREVIOUSCATALOG}" -nt "${INTERMEDIATE_JSDC_FILENAME}" ] 
then 
    logInfo "Store intermediate filtered JSDC'${INTERMEDIATE_JSDC_FILENAME}' " 
    stilts ${STILTS_JAVA_OPTIONS} tpipe in="$PREVIOUSCATALOG" cmd='progress ; delcols CalFlag' out="${INTERMEDIATE_JSDC_FILENAME}"

    # Retrieve original header of catalog.vot to fix because stilts does not care about the GROUP elements of votables.
    logInfo "And put it the original SearchCal's votable header"
    cat catalog.vot | awk '{if ($1=="<TABLEDATA>")end=1;if(end!=1)print;}' > ${INTERMEDIATE_JSDC_FILENAME}.tmp 
    ls -l ${INTERMEDIATE_JSDC_FILENAME}
    ls -l $PWD/${INTERMEDIATE_JSDC_FILENAME}

    cat ${INTERMEDIATE_JSDC_FILENAME} | awk '{if ($1=="<TABLEDATA>")start=1;if(start==1)print;}' >> ${INTERMEDIATE_JSDC_FILENAME}.tmp 
    mv ${INTERMEDIATE_JSDC_FILENAME}.tmp ${INTERMEDIATE_JSDC_FILENAME} 

    # TODO remove blanking values for confidence (-2147483648)

    genMetaAndStats "${INTERMEDIATE_JSDC_FILENAME}"
else
    logInfo "Generation of '${INTERMEDIATE_JSDC_FILENAME}'"
    logInfo "SKIPPED"
fi

#
# PIPELINE STEP 3 : format output
#

# Fixed columns (johnson) + errors + origin (of magnitudes including 'computed' value)
# Columns renaming
# note: e_R, e_I, e_L, e_M, e_N are missing (no data)

OLD_NAMES=( pmRa e_pmRa pmDec e_pmDec plx e_Plx B e_B B.origin V e_V V.origin R R.origin I I.origin J e_J J.origin H e_H H.origin K e_K K.origin L e_L L.origin M e_M M.origin N e_N N.origin LDD e_LDD diam_chi2 UD_B UD_V UD_R UD_I UD_J UD_H UD_K UD_L UD_M UD_N SIMBAD SpType ObjTypes) ;
NEW_NAMES=( pmRA e_pmRA pmDEC e_pmDEC plx e_plx Bmag e_Bmag f_Bmag Vmag e_Vmag f_Vmag Rmag f_Rmag Imag f_Imag Jmag e_Jmag f_Jmag Hmag e_Hmag f_Hmag Kmag e_Kmag f_Kmag Lmag e_Lmag f_Lmag Mmag e_Mmag f_Mmag Nmag e_Nmag f_Nmag LDD e_LDD LDD_chi2 UDDB UDDV UDDR UDDI UDDJ UDDH UDDK UDDL UDDM UDDN MainId_SIMBAD SpType_SIMBAD ObjTypes_SIMBAD) ;
i=0 ;
RENAME_EXPR=""
for OLD_NAME in ${OLD_NAMES[*]}
do
NEW_NAME=${NEW_NAMES[i]} ;
let "i=$i+1" ;
RENAME_EXPR="${RENAME_EXPR}; colmeta -name ${NEW_NAME} ${OLD_NAME}"
done

newStep "Renaming column from \n'${OLD_NAMES[*]}' to \n'${NEW_NAMES[*]}'" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG cmd="progress ${RENAME_EXPR}" out=$CATALOG ;


COLUMNS_SET=" Name RAJ2000 e_RAJ2000 DEJ2000 e_DEJ2000 ${NEW_NAMES[*]} SpType_JMMC CalFlag " ;
newStep "Keeping final columns set \n'${COLUMNS_SET}'" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG cmd="keepcols \"${COLUMNS_SET}\"" out=$CATALOG ;


newStep "Clean useless params of catalog " stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG  cmd="setparam Description \"\"; setparam objectName \"JSDC\"; setparam -ref \"\"; setparam -out.max \"\"; clearparams \"*.origin\"; clearparams \"*.confidence\"" out=$CATALOG ;


# Store last generated fits catalog with its previously defined name
logInfo "Final results are available in ${FINAL_FITS_FILENAME} ... DONE."
cp -a $PREVIOUSCATALOG ${FINAL_FITS_FILENAME}
genMetaAndStats "${FINAL_FITS_FILENAME}"
