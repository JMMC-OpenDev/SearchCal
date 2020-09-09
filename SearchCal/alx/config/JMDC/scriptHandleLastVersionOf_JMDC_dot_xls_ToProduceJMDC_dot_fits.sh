#! /bin/bash
# 2020.07: JMDC is already in CSV format (db)

# enable logging bash commands / errors:
#set -eux

# flag to invoke alxDecodeSpectralType()
FIX_SPTYPE=0

if test $# -ne 1
then
    echo "usage: $0 NAME"
    echo "where NAME is the csv version of the JMDC."
    echo "use http://jmdc.jmmc.fr/ and click on the CSV button (tool bar)"
    exit 1
fi

#./convertJMDCtoCSV.sh

NAME=`basename $1 .csv`
INPUT_CSV=${NAME}.csv

DIR="`pwd`/tmp"
mkdir -p ${DIR}

##FLAGS='-verbose'
##FLAGS='-stderr /dev/null'

# Check if not present and fresh
if [ ${DIR}/../${INPUT_CSV} -nt ${DIR}/${NAME}_raw.fits ]
then
    #Use stilts to convert this csv in fits. 
    #Add a colum to remove all blanks in identifier, this column will then be used for crossmatching.
    stilts  $FLAGS tpipe ifmt=csv omode=out ofmt=fits out="${DIR}/${NAME}_raw.fits" cmd="addcol SIMBAD 'replaceAll(trim(ID1), \" \", \"\" )'"  in="${DIR}/../${INPUT_CSV}"
fi

# Check if not present and fresh
if [ ${DIR}/${NAME}_raw.fits -nt ${DIR}/getstar-output-0.vot ]
then
    #use stilts to extract list of names, removing duplicates, pass to getStar service
    stilts $FLAGS tpipe ifmt=fits in="${DIR}/${NAME}_raw.fits" cmd='keepcols SIMBAD' cmd='sort SIMBAD' cmd='uniq SIMBAD' out="${DIR}/list_of_unique_stars.txt" ofmt=ascii
    #convert this file to an input list for GETSTAR, removing FIRST LINE (#SIMBAD):
    rm -rf ${DIR}/list_of_unique_stars0.txt #absolutely necessary (or generate random filename)!
    for i in `tail -n +2 ${DIR}/list_of_unique_stars.txt`; do echo -n "${i}|" >>  ${DIR}/list_of_unique_stars0.txt ; done #note: unuseful to remove last separator, getstar is happy with it
    #Last chance to beautify the object names. Apparently only 2MASS, CCDM and WDS identifiers need to be separated from J.. with a blank
    sed -e 's%2MASSJ%2MASS J%g;s%CCDMJ%CCDM J%g;s%WDSJ%WDS J%g' ${DIR}/list_of_unique_stars0.txt > ${DIR}/list_of_stars.txt

    #find complementary information through getstar service:
    sclsvrServer -noDate -noFileLine -v 3  GETSTAR "-objectName `cat ${DIR}/list_of_stars.txt` -file ${DIR}/getstar-output-0.vot" &>> ${DIR}/getstar.log
fi

#remove blanks in the TARGET_ID column returned by getstar and rename it to SIMBAD...
stilts $FLAGS tpipe ifmt=votable omode=out ofmt=votable in="${DIR}/getstar-output-0.vot" out="${DIR}/getstar-output.vot" cmd="replacecol SIMBAD 'replaceAll(TARGET_ID, \" \", \"\" )'" 

#cross-match with ${NAME}_raw, using the star name, for all stars of ${NAME}_raw...
stilts $FLAGS tmatch2 in1="${DIR}/${NAME}_raw.fits" ifmt1=fits in2="${DIR}/getstar-output.vot" ifmt2=votable omode=out out="${DIR}/${NAME}_intermediate.fits" ofmt=fits find=best1 fixcols=dups join=all1 matcher=exact values1="SIMBAD" values2="SIMBAD"

#warning if some match has not been done, meaning that a main ID is not correct:
stilts $FLAGS tpipe ifmt=fits cmd='keepcols SIMBAD_2' cmd='select NULL_SIMBAD_2' omode=count in="${DIR}/${NAME}_intermediate.fits" > ${DIR}/count #writes, e.g. columns: 1   rows: 3

declare -i NB
let NB=0`cat ${DIR}/count |cut -d: -f3|tr -d ' '`
if (( $NB > 0 ))
then 
  echo "WARNING! Cross-Matching Incomplete !"
  echo "list of unmatched sources:"
  stilts $FLAGS tpipe ifmt=fits cmd='select NULL_SIMBAD_2' cmd='keepcols ID1' omode=out ofmt=ascii out="${DIR}/unmatched.txt" in="${DIR}/${NAME}_intermediate.fits"
  cat ${DIR}/unmatched.txt|tr -d ' '|tr -d \" |sort |uniq |grep -v \#
fi

#remove SIMBAD_1, SIMBAD_2, deletedFlag, GroupID, GroupSize... and all the origin and confidence columns. Better done one by one in case a column to be deleted is absent:
stilts $FLAGS tpipe  ifmt=fits omode=out ofmt=fits out="${DIR}/${NAME}_intermediate.fits" in="${DIR}/${NAME}_intermediate.fits" cmd="delcols 'SIMBAD_2 GroupID GroupSize* XM* *.confidence'"
#do not remove origin
#stilts $FLAGS tpipe  ifmt=fits omode=out ofmt=fits out="${DIR}/${NAME}_intermediate.fits" in="${DIR}/${NAME}_intermediate.fits" cmd="delcols '*.origin '"
stilts $FLAGS tpipe  ifmt=fits omode=out ofmt=fits out="${DIR}/${NAME}_intermediate.fits" in="${DIR}/${NAME}_intermediate.fits" cmd="colmeta -name SIMBAD SIMBAD_1"
stilts $FLAGS tpipe  ifmt=fits omode=out ofmt=fits out="${DIR}/${NAME}_intermediate.fits" in="${DIR}/${NAME}_intermediate.fits" cmd="delcols 'deletedFlag'"
#due to changes in JMDC following the publication at CDS, the file passed to update_ld_in_jmdc needs to have a supplementary column UD_TO_LD_CONVFACTOR added before call.
stilts $FLAGS tpipe  ifmt=fits omode=out ofmt=fits out="${DIR}/${NAME}_intermediate.fits" in="${DIR}/${NAME}_intermediate.fits" cmd="addcol -after MU_LAMBDA UD_TO_LD_CONVFACTOR 0.00" cmd="addcol -after UD_TO_LD_CONVFACTOR LDD_ORIG 0.00"

if (( $FIX_SPTYPE != 0 ))
then
    stilts $FLAGS tpipe  ifmt=fits omode=out ofmt=fits out="${DIR}/${NAME}_intermediate.fits" in="${DIR}/${NAME}_intermediate.fits" cmd="delcols 'color_table* lum_class* SpType_JMMC*'"

    #add spectral type index columns etc.
    stilts tpipe ifmt=fits in="${DIR}/${NAME}_intermediate.fits" cmd='keepcols "SPTYPE"' omode=out ofmt=ascii out="${DIR}/sptype.ascii"

    alxDecodeSpectralType -i "${DIR}/sptype.ascii" -o "${DIR}/sptype.tst"

    stilts tjoin nin=2 ifmt1=fits in1="${DIR}/${NAME}_intermediate.fits" ifmt2=tst in2="${DIR}/sptype.tst" ofmt=fits out=${NAME}_final.fits fixcols="dups" 
else
    cp ${DIR}/${NAME}_intermediate.fits ${NAME}_final.fits
fi

#Add or change measured LDD in Database (${NAME}_final.fits) by using Neilson & Leister coefficients.
#this is done with the GDL procedure update_ld_in_jmdc.pro
#the latter uses the MuFactor.fits file.
#in case this file must be updated:
# eventually, idl_interpol_teff_logg.pro is used to produce a .tst file that MUST be converted to fits (TeffLogg.fits) using stilts/topcat. It is just
# an interpolator of the alxTeffLogg file, on the finer grid we use.
# then N&L coefficients for Giants , and then for FGK dwarves, must be computed using addNeilsonToTeffLogg. The two outputs must be joined by topcat, and must start at O5.00 (otherwise correct update_ld_in_jmdc).
idl -e "update_ld_in_jmdc,\"${NAME}_final.fits\"" #could be GDL.
#then use make_jsdc_script_simple.pro to compute the database. IDL is needed until GDL knows about gaussfit.
idl -e "make_jsdc_script_simple,\"${NAME}_final_lddUpdated.fits\",/nopause"

