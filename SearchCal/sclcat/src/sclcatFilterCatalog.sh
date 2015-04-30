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

removeWdsSb9WithSimbadCrossMatch()
{

    # check file
    if [ ! -e "$SIMBADBLACKLIST" ]
    then
        logInfo "Blacklist file not found : should be here '$SIMBADBLACKLIST'"
        exit 1
    fi
    logInfo "Using '$SIMBADBLACKLIST' file to get proper simbad script filtering"


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
STILTS_JAVA_OPTIONS=" -Xms2048m -Xmx4096m -XX:+UseConcMarkSweepGC"
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

CATALOG=catalog0.fits
genMetaAndStats "${CATALOG}"
newStep "Convert raw VOTable catalog to FITS" stilts ${STILTS_JAVA_OPTIONS} tcopy $PREVIOUSCATALOG $CATALOG

#
# PIPELINE STEP 2 : filter rows
#


#############################################################
# PRELIMINARY CALIBRATORS SELECTION : diamFlag must be true #
#############################################################
newStep "Keep stars with diamFlag==1" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG  cmd='progress ; select diamFlag' out=$CATALOG

# JSDC FAINT:
# keep only stars with SpType_JMMC (ie interpreted correctly)
newStep "Keep stars with SpType_JMMC NOT NULL" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG  cmd='progress ; select !NULL_SpType_JMMC' out=$CATALOG



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


# redundant check with DIAM_FLAG=1
#newStep "Rejecting stars with low or medium confidence on 'diam_mean'" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG  cmd='progress ; select diam_mean.confidence==3' out=$CATALOG ;

# redundant check with hard-coded value in sclsvr
#newStep "Rejecting stars with e_plx/plx>.25" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG  cmd='progress ; select !((e_plx/plx)>0.25)' out=$CATALOG ;

# Filter SB9 and WDS
newStep "Rejecting stars with SB9 references" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG  cmd='progress ; select NULL_SBC9' out=$CATALOG ;
newStep "Rejecting stars with WDS references" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG  cmd='progress ; select !(sep1<2||sep2<2)' out=$CATALOG ;

# Multiplicity (MultFlag=S)
# TODO: SCL GUI MultiplicityFilter rejects BINFLAG(SB from SPTYPE) and MULTFLAG(ASCC CGOVX) not NULL
newStep "Rejecting stars with MultFlag=S" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG  cmd='progress ; select !contains(\"\"+MultFlag,\"S\")' out=$CATALOG ;

# Variability ? (varFlag 1 - 3) ...
# TODO: SCL GUI VariabilityFilter rejects VARFLAG_1 (ASCC GN) and VARFLAG_2 (ASCC UVW Tycho-1) not NULL and VARFLAG_3 (ASCC CDMPRU) != 'C'

# Get BadCal Votable if not present and fresh
if [ "${PREVIOUSCATALOG}" -nt "badcal.vot" ] 
then
    logInfo "Get badcal catalog"
    curl -o badcal.vot 'http://apps.jmmc.fr/badcal-dsa/SubmitCone?DSACATTAB=badcal.valid_stars&RA=0.0&DEC=0.0&SR=360.0' ; 
else 
    logInfo "Badcal catalog already present"
fi
newStep "Rejecting badcal stars" stilts ${STILTS_JAVA_OPTIONS} tskymatch2 ra1='radiansToDegrees(hmsToRadians(RAJ2000))' ra2='ra' dec1='radiansToDegrees(dmsToRadians(DEJ2000))' dec2='dec' error=5 join="1not2" find="all" out="$CATALOG" $PREVIOUSCATALOG  badcal.vot

# extracted from http://apps.jmmc.fr/exist/apps/myapp/jsdcStatUtils.xql
newStep "Rejecting stars with bad simbad otypes" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG \
cmd='progress ; select !(contains(ObjTypes,\",Unknown,\")||contains(ObjTypes,\",?,\")||contains(ObjTypes,\",Transient,\")||contains(ObjTypes,\",ev,\")||contains(ObjTypes,\",Radio,\")||contains(ObjTypes,\",Rad,\")||contains(ObjTypes,\",Radio(m),\")||contains(ObjTypes,\",mR,\")||contains(ObjTypes,\",Radio(cm),\")||contains(ObjTypes,\",cm,\")||contains(ObjTypes,\",Radio(mm),\")||contains(ObjTypes,\",mm,\")||contains(ObjTypes,\",Radio(sub-mm),\")||contains(ObjTypes,\",smm,\")||contains(ObjTypes,\",HI,\")||contains(ObjTypes,\",HI,\")||contains(ObjTypes,\",radioBurst,\")||contains(ObjTypes,\",rB,\")||contains(ObjTypes,\",Maser,\")||contains(ObjTypes,\",Mas,\")||contains(ObjTypes,\",IR>30um,\")||contains(ObjTypes,\",FIR,\")||contains(ObjTypes,\",IR<10um,\")||contains(ObjTypes,\",NIR,\")||contains(ObjTypes,\",Red,\")||contains(ObjTypes,\",red,\")||contains(ObjTypes,\",RedExtreme,\")||contains(ObjTypes,\",ERO,\")||contains(ObjTypes,\",Blue,\")||contains(ObjTypes,\",blu,\")||contains(ObjTypes,\",X,\")||contains(ObjTypes,\",X,\")||contains(ObjTypes,\",ULX?,\")||contains(ObjTypes,\",UX?,\")||contains(ObjTypes,\",ULX,\")||contains(ObjTypes,\",ULX,\")||contains(ObjTypes,\",gamma,\")||contains(ObjTypes,\",gam,\")||contains(ObjTypes,\",gammaBurst,\")||contains(ObjTypes,\",gB,\")||contains(ObjTypes,\",Inexistent,\")||contains(ObjTypes,\",err,\")||contains(ObjTypes,\",Gravitation,\")||contains(ObjTypes,\",grv,\")||contains(ObjTypes,\",LensingEv,\")||contains(ObjTypes,\",Lev,\")||contains(ObjTypes,\",Candidate_LensSystem,\")||contains(ObjTypes,\",LS?,\")||contains(ObjTypes,\",Candidate_Lens,\")||contains(ObjTypes,\",Le?,\")||contains(ObjTypes,\",Possible_lensImage,\")||contains(ObjTypes,\",LI?,\")||contains(ObjTypes,\",GravLens,\")||contains(ObjTypes,\",gLe,\")||contains(ObjTypes,\",GravLensSystem,\")||contains(ObjTypes,\",gLS,\")||contains(ObjTypes,\",Candidates,\")||contains(ObjTypes,\",..?,\")||contains(ObjTypes,\",Possible_G,\")||contains(ObjTypes,\",G?,\")||contains(ObjTypes,\",Possible_SClG,\")||contains(ObjTypes,\",SC?,\")||contains(ObjTypes,\",Possible_ClG,\")||contains(ObjTypes,\",C?G,\")||contains(ObjTypes,\",Possible_GrG,\")||contains(ObjTypes,\",Gr?,\")||contains(ObjTypes,\",Candidate_**,\")||contains(ObjTypes,\",**?,\")||contains(ObjTypes,\",Candidate_EB*,\")||contains(ObjTypes,\",EB?,\")||contains(ObjTypes,\",Candidate_CV*,\")||contains(ObjTypes,\",CV?,\")||contains(ObjTypes,\",Candidate_XB*,\")||contains(ObjTypes,\",XB?,\")||contains(ObjTypes,\",Candidate_LMXB,\")||contains(ObjTypes,\",LX?,\")||contains(ObjTypes,\",Candidate_HMXB,\")||contains(ObjTypes,\",HX?,\")||contains(ObjTypes,\",Candidate_YSO,\")||contains(ObjTypes,\",Y*?,\")||contains(ObjTypes,\",Candidate_pMS*,\")||contains(ObjTypes,\",pr?,\")||contains(ObjTypes,\",Candidate_TTau*,\")||contains(ObjTypes,\",TT?,\")||contains(ObjTypes,\",Candidate_C*,\")||contains(ObjTypes,\",C*?,\")||contains(ObjTypes,\",Candidate_S*,\")||contains(ObjTypes,\",S*?,\")||contains(ObjTypes,\",Candidate_OH,\")||contains(ObjTypes,\",OH?,\")||contains(ObjTypes,\",Candidate_CH,\")||contains(ObjTypes,\",CH?,\")||contains(ObjTypes,\",Candidate_WR*,\")||contains(ObjTypes,\",WR?,\")||contains(ObjTypes,\",Candidate_Be*,\")||contains(ObjTypes,\",Be?,\")||contains(ObjTypes,\",Candidate_Ae*,\")||contains(ObjTypes,\",Ae?,\")||contains(ObjTypes,\",Candidate_HB*,\")||contains(ObjTypes,\",HB?,\")||contains(ObjTypes,\",Candidate_RRLyr,\")||contains(ObjTypes,\",RR?,\")||contains(ObjTypes,\",Candidate_Cepheid,\")||contains(ObjTypes,\",Ce?,\")||contains(ObjTypes,\",Candidate_post-AGB*,\")||contains(ObjTypes,\",pA?,\")||contains(ObjTypes,\",Candidate_BSS,\")||contains(ObjTypes,\",BS?,\")||contains(ObjTypes,\",Candidate_BH,\")||contains(ObjTypes,\",BH?,\")||contains(ObjTypes,\",Candidate_SN*,\")||contains(ObjTypes,\",SN?,\")||contains(ObjTypes,\",multiple_object,\")||contains(ObjTypes,\",mul,\")||contains(ObjTypes,\",Region,\")||contains(ObjTypes,\",reg,\")||contains(ObjTypes,\",Void,\")||contains(ObjTypes,\",vid,\")||contains(ObjTypes,\",SuperClG,\")||contains(ObjTypes,\",SCG,\")||contains(ObjTypes,\",ClG,\")||contains(ObjTypes,\",ClG,\")||contains(ObjTypes,\",GroupG,\")||contains(ObjTypes,\",GrG,\")||contains(ObjTypes,\",Compact_Gr_G,\")||contains(ObjTypes,\",CGG,\")||contains(ObjTypes,\",PairG,\")||contains(ObjTypes,\",PaG,\")||contains(ObjTypes,\",IG,\")||contains(ObjTypes,\",IG,\")||contains(ObjTypes,\",Cl*?,\")||contains(ObjTypes,\",C?*,\")||contains(ObjTypes,\",GlCl?,\")||contains(ObjTypes,\",Gl?,\")||contains(ObjTypes,\",Cl*,\")||contains(ObjTypes,\",Cl*,\")||contains(ObjTypes,\",GlCl,\")||contains(ObjTypes,\",GlC,\")||contains(ObjTypes,\",OpCl,\")||contains(ObjTypes,\",OpC,\")||contains(ObjTypes,\",Assoc*,\")||contains(ObjTypes,\",As*,\")||contains(ObjTypes,\",EB*,\")||contains(ObjTypes,\",EB*,\")||contains(ObjTypes,\",EB*Algol,\")||contains(ObjTypes,\",Al*,\")||contains(ObjTypes,\",EB*betLyr,\")||contains(ObjTypes,\",bL*,\")||contains(ObjTypes,\",EB*WUMa,\")||contains(ObjTypes,\",WU*,\")||contains(ObjTypes,\",SB*,\")||contains(ObjTypes,\",SB*,\")||contains(ObjTypes,\",EllipVar,\")||contains(ObjTypes,\",El*,\")||contains(ObjTypes,\",Symbiotic*,\")||contains(ObjTypes,\",Sy*,\")||contains(ObjTypes,\",CataclyV*,\")||contains(ObjTypes,\",CV*,\")||contains(ObjTypes,\",DQHer,\")||contains(ObjTypes,\",DQ*,\")||contains(ObjTypes,\",AMHer,\")||contains(ObjTypes,\",AM*,\")||contains(ObjTypes,\",Nova-like,\")||contains(ObjTypes,\",NL*,\")||contains(ObjTypes,\",Nova,\")||contains(ObjTypes,\",No*,\")||contains(ObjTypes,\",DwarfNova,\")||contains(ObjTypes,\",DN*,\")||contains(ObjTypes,\",XB,\")||contains(ObjTypes,\",XB*,\")||contains(ObjTypes,\",LMXB,\")||contains(ObjTypes,\",LXB,\")||contains(ObjTypes,\",HMXB,\")||contains(ObjTypes,\",HXB,\")||contains(ObjTypes,\",ISM,\")||contains(ObjTypes,\",ISM,\")||contains(ObjTypes,\",PartofCloud,\")||contains(ObjTypes,\",PoC,\")||contains(ObjTypes,\",PN?,\")||contains(ObjTypes,\",PN?,\")||contains(ObjTypes,\",ComGlob,\")||contains(ObjTypes,\",CGb,\")||contains(ObjTypes,\",Bubble,\")||contains(ObjTypes,\",bub,\")||contains(ObjTypes,\",EmObj,\")||contains(ObjTypes,\",EmO,\")||contains(ObjTypes,\",Cloud,\")||contains(ObjTypes,\",Cld,\")||contains(ObjTypes,\",GalNeb,\")||contains(ObjTypes,\",GNe,\")||contains(ObjTypes,\",BrNeb,\")||contains(ObjTypes,\",BNe,\")||contains(ObjTypes,\",DkNeb,\")||contains(ObjTypes,\",DNe,\")||contains(ObjTypes,\",RfNeb,\")||contains(ObjTypes,\",RNe,\")||contains(ObjTypes,\",MolCld,\")||contains(ObjTypes,\",MoC,\")||contains(ObjTypes,\",Globule,\")||contains(ObjTypes,\",glb,\")||contains(ObjTypes,\",denseCore,\")||contains(ObjTypes,\",cor,\")||contains(ObjTypes,\",HVCld,\")||contains(ObjTypes,\",HVC,\")||contains(ObjTypes,\",HII,\")||contains(ObjTypes,\",HII,\")||contains(ObjTypes,\",PN,\")||contains(ObjTypes,\",PN,\")||contains(ObjTypes,\",HIshell,\")||contains(ObjTypes,\",sh,\")||contains(ObjTypes,\",SNR?,\")||contains(ObjTypes,\",SR?,\")||contains(ObjTypes,\",SNR,\")||contains(ObjTypes,\",SNR,\")||contains(ObjTypes,\",Circumstellar,\")||contains(ObjTypes,\",cir,\")||contains(ObjTypes,\",outflow?,\")||contains(ObjTypes,\",of?,\")||contains(ObjTypes,\",Outflow,\")||contains(ObjTypes,\",out,\")||contains(ObjTypes,\",HH,\")||contains(ObjTypes,\",HH,\")||contains(ObjTypes,\",YSO,\")||contains(ObjTypes,\",Y*O,\")||contains(ObjTypes,\",Ae*,\")||contains(ObjTypes,\",Ae*,\")||contains(ObjTypes,\",Em*,\")||contains(ObjTypes,\",Em*,\")||contains(ObjTypes,\",Be*,\")||contains(ObjTypes,\",Be*,\")||contains(ObjTypes,\",SG*,\")||contains(ObjTypes,\",sg*,\")||contains(ObjTypes,\",post-AGB*,\")||contains(ObjTypes,\",pA*,\")||contains(ObjTypes,\",OH/IR,\")||contains(ObjTypes,\",OH*,\")||contains(ObjTypes,\",CH,\")||contains(ObjTypes,\",CH*,\")||contains(ObjTypes,\",pMS*,\")||contains(ObjTypes,\",pr*,\")||contains(ObjTypes,\",TTau*,\")||contains(ObjTypes,\",TT*,\")||contains(ObjTypes,\",WR*,\")||contains(ObjTypes,\",WR*,\")||contains(ObjTypes,\",Irregular_V*,\")||contains(ObjTypes,\",Ir*,\")||contains(ObjTypes,\",Orion_V*,\")||contains(ObjTypes,\",Or*,\")||contains(ObjTypes,\",Eruptive*,\")||contains(ObjTypes,\",Er*,\")||contains(ObjTypes,\",FUOr,\")||contains(ObjTypes,\",FU*,\")||contains(ObjTypes,\",Erupt*RCrB,\")||contains(ObjTypes,\",RC*,\")||contains(ObjTypes,\",Pulsar,\")||contains(ObjTypes,\",Psr,\")||contains(ObjTypes,\",BYDra,\")||contains(ObjTypes,\",BY*,\")||contains(ObjTypes,\",RSCVn,\")||contains(ObjTypes,\",RS*,\")||contains(ObjTypes,\",PulsV*,\")||contains(ObjTypes,\",Pu*,\")||contains(ObjTypes,\",RRLyr,\")||contains(ObjTypes,\",RR*,\")||contains(ObjTypes,\",Cepheid,\")||contains(ObjTypes,\",Ce*,\")||contains(ObjTypes,\",PulsV*delSct,\")||contains(ObjTypes,\",dS*,\")||contains(ObjTypes,\",PulsV*RVTau,\")||contains(ObjTypes,\",RV*,\")||contains(ObjTypes,\",PulsV*WVir,\")||contains(ObjTypes,\",WV*,\")||contains(ObjTypes,\",PulsV*bCep,\")||contains(ObjTypes,\",bC*,\")||contains(ObjTypes,\",deltaCep,\")||contains(ObjTypes,\",cC*,\")||contains(ObjTypes,\",gammaDor,\")||contains(ObjTypes,\",gD*,\")||contains(ObjTypes,\",pulsV*SX,\")||contains(ObjTypes,\",SX*,\")||contains(ObjTypes,\",LPV*,\")||contains(ObjTypes,\",LP*,\")||contains(ObjTypes,\",Mira,\")||contains(ObjTypes,\",Mi*,\")||contains(ObjTypes,\",semi-regV*,\")||contains(ObjTypes,\",sr*,\")||contains(ObjTypes,\",SN,\")||contains(ObjTypes,\",SN*,\")||contains(ObjTypes,\",Sub-stellar,\")||contains(ObjTypes,\",su*,\")||contains(ObjTypes,\",Planet?,\")||contains(ObjTypes,\",Pl?,\")||contains(ObjTypes,\",Galaxy,\")||contains(ObjTypes,\",G,\")||contains(ObjTypes,\",PartofG,\")||contains(ObjTypes,\",PoG,\")||contains(ObjTypes,\",GinCl,\")||contains(ObjTypes,\",GiC,\")||contains(ObjTypes,\",BClG,\")||contains(ObjTypes,\",BiC,\")||contains(ObjTypes,\",GinGroup,\")||contains(ObjTypes,\",GiG,\")||contains(ObjTypes,\",GinPair,\")||contains(ObjTypes,\",GiP,\")||contains(ObjTypes,\",High_z_G,\")||contains(ObjTypes,\",HzG,\")||contains(ObjTypes,\",AbsLineSystem,\")||contains(ObjTypes,\",ALS,\")||contains(ObjTypes,\",Ly-alpha_ALS,\")||contains(ObjTypes,\",LyA,\")||contains(ObjTypes,\",DLy-alpha_ALS,\")||contains(ObjTypes,\",DLA,\")||contains(ObjTypes,\",metal_ALS,\")||contains(ObjTypes,\",mAL,\")||contains(ObjTypes,\",Ly-limit_ALS,\")||contains(ObjTypes,\",LLS,\")||contains(ObjTypes,\",Broad_ALS,\")||contains(ObjTypes,\",BAL,\")||contains(ObjTypes,\",RadioG,\")||contains(ObjTypes,\",rG,\")||contains(ObjTypes,\",HII_G,\")||contains(ObjTypes,\",H2G,\")||contains(ObjTypes,\",LSB_G,\")||contains(ObjTypes,\",LSB,\")||contains(ObjTypes,\",AGN_Candidate,\")||contains(ObjTypes,\",AG?,\")||contains(ObjTypes,\",QSO_Candidate,\")||contains(ObjTypes,\",Q?,\")||contains(ObjTypes,\",Blazar_Candidate,\")||contains(ObjTypes,\",Bz?,\")||contains(ObjTypes,\",BLLac_Candidate,\")||contains(ObjTypes,\",BL?,\")||contains(ObjTypes,\",EmG,\")||contains(ObjTypes,\",EmG,\")||contains(ObjTypes,\",StarburstG,\")||contains(ObjTypes,\",SBG,\")||contains(ObjTypes,\",BlueCompG,\")||contains(ObjTypes,\",bCG,\")||contains(ObjTypes,\",LensedImage,\")||contains(ObjTypes,\",LeI,\")||contains(ObjTypes,\",LensedG,\")||contains(ObjTypes,\",LeG,\")||contains(ObjTypes,\",LensedQ,\")||contains(ObjTypes,\",LeQ,\")||contains(ObjTypes,\",AGN,\")||contains(ObjTypes,\",AGN,\")||contains(ObjTypes,\",LINER,\")||contains(ObjTypes,\",LIN,\")||contains(ObjTypes,\",Seyfert,\")||contains(ObjTypes,\",SyG,\")||contains(ObjTypes,\",Seyfert_1,\")||contains(ObjTypes,\",Sy1,\")||contains(ObjTypes,\",Seyfert_2,\")||contains(ObjTypes,\",Sy2,\")||contains(ObjTypes,\",Blazar,\")||contains(ObjTypes,\",Bla,\")||contains(ObjTypes,\",BLLac,\")||contains(ObjTypes,\",BLL,\")||contains(ObjTypes,\",OVV,\")||contains(ObjTypes,\",OVV,\")||contains(ObjTypes,\",QSO,\")||contains(ObjTypes,\",QSO,\"))' \
out=$CATALOG ;



# store an intermediate JSDC votable since all row filters should have been applied 
# and produce stats and meta reports
if [ "${PREVIOUSCATALOG}" -nt "${INTERMEDIATE_JSDC_FILENAME}" ] 
then 
    logInfo "Store intermediate filtered JSDC'${INTERMEDIATE_JSDC_FILENAME}' " 
    stilts ${STILTS_JAVA_OPTIONS} tcat in="$PREVIOUSCATALOG" out="${INTERMEDIATE_JSDC_FILENAME}"

    # Retrieve original header of catalog.vot to fix because stilts does not care about the GROUP elements of votables.
    logInfo "And put it the original SearchCal's votable header"
    cat catalog.vot | awk '{if ($1=="<TABLEDATA>")end=1;if(end!=1)print;}' > ${INTERMEDIATE_JSDC_FILENAME}.tmp 
    ls -l ${INTERMEDIATE_JSDC_FILENAME}
    ls -l $PWD/${INTERMEDIATE_JSDC_FILENAME}

    cat ${INTERMEDIATE_JSDC_FILENAME} | awk '{if ($1=="<TABLEDATA>")start=1;if(start==1)print;}' >> ${INTERMEDIATE_JSDC_FILENAME}.tmp 
    mv ${INTERMEDIATE_JSDC_FILENAME}.tmp ${INTERMEDIATE_JSDC_FILENAME} 

    # TODO remove blanking values for confidence (-2147483648)
    # TODO handle NaN here or in the SC GUI
    # TODO GZIP votable

    
    genMetaAndStats "${INTERMEDIATE_JSDC_FILENAME}"
else
    logInfo "Generation of '${INTERMEDIATE_JSDC_FILENAME}'"
    logInfo "SKIPPED"
fi

#
# PIPELINE STEP 3 : format output
#

# clean unrelevant dist content irrelevant with JSDC scenario
# was useless ?
# newStep "Clearing query-specific 'dist' column" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG  cmd='progress ; delcols dist; addcol -before dist.ORIGIN dist NULL ' out=$CATALOG

# TODO remove these lines replaced by origin management bellow when it has been mentionned as changes the JSDC readme
# newStep "Adding a flag column for R provenance" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG  cmd='progress ; addcol f_Rmag NULL_R.confidence?1:0' out=$CATALOG ;
# newStep "Adding a flag column for I provenance" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG  cmd='progress ; addcol f_Imag NULL_I.confidence?1:0' out=$CATALOG ;

# Create Name + filter names
newStep "Adding the 'Name' column to use one simbad script" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG  cmd='progress ; addcol Name !equals(HIP,\"NaN\")?\"HIP\"+HIP:!equals(HD,\"NaN\")?\"HD\"+HD:(!(NULL_TYC1||NULL_TYC2||NULL_TYC3)?\"TYC\"+TYC1+\"-\"+TYC2+\"-\"+TYC3:\"\"+RAJ2000+\"\ \"+DEJ2000)' out=$CATALOG ;

# LBO: TODO: disable this step as it is too slow (>60 minutes) on very large catalogs
# newStep "Flagging duplicated Name entries" stilts ${STILTS_JAVA_OPTIONS} tmatch1 in=$PREVIOUSCATALOG matcher=exact values='Name' out=$CATALOG

# this filter do not remove any row and should be moved back to previous pipeline's step
# disabled, may be removed ?
# newStep "Removing duplicated Name entries" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG cmd='progress ; colmeta -name NameGroupID GroupID' cmd='progress ; colmeta -name NameGroupSize GroupSize' cmd='progress; select NULL_NameGroupSize' out=$CATALOG


# Note: UDDK is empty as JSDC scenario does not query Borde/Merand catalogs so this column is not present in the input catalog
#       UDDK is removed now to avoid futur conflict because UD_K will be renamed UDDK
#newStep "Removing unwanted column UDDK" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG cmd='delcols "UDDK"' out=$CATALOG ;


# Fixed columns (johnson or cousin ?) + errors + origin (of magnitudes including 'computed' value)
# Columns renaming
# note: e_R, e_I, e_L, e_M, e_N are missing (no data)
# origin HIP2 for RA/DE J2000, pmRA/pmDEC and plx/e_plx

OLD_NAMES=( pmRa e_pmRa pmDec e_pmDec plx e_Plx B e_B B.origin V e_V V.origin R R.origin I I.origin J e_J J.origin H e_H H.origin K e_K K.origin L L.origin M M.origin N N.origin LDD e_LDD diam_chi2 UD_B UD_V UD_R UD_I UD_J UD_H UD_K UD_L UD_M UD_N SpType ObjTypes) ;
NEW_NAMES=( pmRA e_pmRA pmDEC e_pmDEC plx e_plx Bmag e_Bmag f_Bmag Vmag e_Vmag f_Vmag Rmag f_Rmag Imag f_Imag Jmag e_Jmag f_Jmag Hmag e_Hmag f_Hmag Kmag e_Kmag f_Kmag Lmag f_Lmag Mmag f_Mmag Nmag f_Nmag LDD e_LDD LDD_chi2 UDDB UDDV UDDR UDDI UDDJ UDDH UDDK UDDL UDDM UDDN SpType_SIMBAD ObjTypes_SIMBAD) ;
i=0 ;
RENAME_EXPR=""
for OLD_NAME in ${OLD_NAMES[*]}
do
NEW_NAME=${NEW_NAMES[i]} ;
let "i=$i+1" ;
RENAME_EXPR="${RENAME_EXPR}; colmeta -name ${NEW_NAME} ${OLD_NAME}"
done
newStep "Renaming column from \n'${OLD_NAMES[*]}' to \n'${NEW_NAMES[*]}'" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG cmd="progress ${RENAME_EXPR}" out=$CATALOG ;

COLUMNS_SET=" Name RAJ2000 e_RAJ2000 DEJ2000 e_DEJ2000 ${NEW_NAMES[*]} SpType_JMMC AV_fit e_AV_fit AV_fit_chi2 Teff_SpType logg_SpType" ;
newStep "Keeping final columns set \n'${COLUMNS_SET}'" stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG cmd="keepcols \"${COLUMNS_SET}\"" out=$CATALOG ;

# Add special simbad filtering until wds and sbc9 coordinates fixes
#removeWdsSb9WithSimbadCrossMatch

newStep "Clean useless params of catalog " stilts ${STILTS_JAVA_OPTIONS} tpipe in=$PREVIOUSCATALOG  cmd="setparam Description \"\"; setparam objectName \"\"; setparam -ref \"\"; setparam -out.max \"\"; clearparams \"*.origin\"; clearparams \"*.confidence\"" out=$CATALOG ;

# Store last generated fits catalog with its previously defined name
logInfo "Final results are available in ${FINAL_FITS_FILENAME} ... DONE."
cp -a $PREVIOUSCATALOG ${FINAL_FITS_FILENAME}
genMetaAndStats "${FINAL_FITS_FILENAME}"

