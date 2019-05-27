#! /bin/bash
if test $# -ne 1
then
        echo usage: $0 NAME
	echo where NAME is the xls version of the JMDC.
        exit 1
fi
NAME=`basename $1 .xls`
##FLAGS='-stderr /dev/null'
##use openoffice to convert xls in csv
soffice --headless --convert-to csv --outdir /tmp ${NAME}.xls # JMDC.xls
if [[ ! -e /tmp/${NAME}.csv ]]; then echo "CSV conversion not done -- openoffice probably active on this session (close all openoffice windows), or filename error" ; exit; fi
#fix non-ascii characters:
iconv -c -f utf-8 -t ascii -o /tmp/${NAME}-ascii.csv /tmp/${NAME}.csv
#Use stilts to convert this csv in fits. 
#Add a colum to remove all blanks in identifier, this column will then be used for crossmatching.
stilts  $FLAGS tpipe ifmt=csv omode=out ofmt=fits out="/tmp/${NAME}_raw.fits" cmd="addcol SIMBAD 'replaceAll(trim(ID1), \" \", \"\" )'"  in="/tmp/${NAME}-ascii.csv"
#use stilts to extract list of names, removing duplicates, pass to getStar service
stilts $FLAGS tpipe ifmt=fits in="/tmp/${NAME}_raw.fits" cmd='keepcols SIMBAD' cmd='sort SIMBAD' cmd='uniq SIMBAD' out="/tmp/list_of_unique_stars.txt" ofmt=ascii
#convert this file to an input list for GETSTAR, removing FIRST LINE (#SIMBAD):
\rm -rf /tmp/list_of_unique_stars0.txt #absolutely necessary (or generate random filename)!
for i in `tail -n +2 /tmp/list_of_unique_stars.txt`; do echo -n "${i}," >>  /tmp/list_of_unique_stars0.txt ; done #note: unuseful to remove last comma, getstar is happy with it
#Last chance to beautify the object names. Apparently only 2MASS and CCDM identifiers need to be separated from J.. with a blank
sed -e 's%2MASSJ%2MASS J%g;s%CCDMJ%CCDM J%g' /tmp/list_of_unique_stars0.txt > /tmp/list_of_stars.txt
#find complementary information through getstar service:
sclsvrServer -noDate -noFileLine -v 3  GETSTAR "-objectName `cat /tmp/list_of_stars.txt` -file /tmp/getstar-output.vot" &>> /tmp/getstar.log
#remove blanks in the  SIMBAD column returned by getstar...
stilts $FLAGS tpipe ifmt=votable omode=out ofmt=votable in="/tmp/getstar-output.vot" out="/tmp/getstar-output.vot" cmd="replacecol SIMBAD 'replaceAll( SIMBAD, \" \", \"\" )'" 
#cross-match with ${NAME}_raw, using the star name, for all stars of ${NAME}_raw...
stilts $FLAGS tmatch2 in1="/tmp/${NAME}_raw.fits" ifmt1=fits in2="/tmp/getstar-output.vot" ifmt2=votable omode=out out="/tmp/${NAME}_intermediate.fits" ofmt=fits find=best1 fixcols=dups join=all1 matcher=exact values1="SIMBAD" values2="SIMBAD"
#warning if some match has not been done, meaning that a main ID is not correct:
stilts $FLAGS tpipe ifmt=fits cmd='keepcols SIMBAD' cmd='select NULL_SIMBAD' omode=count in="/tmp/${NAME}_intermediate.fits" > /tmp/count #writes, e.g. columns: 1   rows: 3
declare -i NB
let NB=0`cat /tmp/count |cut -d: -f3|tr -d ' '`
if (( $NB > 0 ))
then 
 echo "WARNING! Cross-Matching Incomplete!"
 echo "list of unmatched sources:"
 stilts $FLAGS tpipe ifmt=fits cmd='select NULL_SIMBAD_2' cmd='keepcols ID1' omode=out ofmt=ascii out="/tmp/unmatched.txt" in="/tmp/${NAME}_intermediate.fits"
 cat /tmp/unmatched.txt|tr -d ' '|tr -d \" |sort |uniq |grep -v \#
fi
#remove SIMBAD_1, SIMBAD_2, deletedFlag, GroupID, GroupSize... and all the origin and confidence columns. Better done one by one in case a column to be deleted is absent:
stilts $FLAGS tpipe  ifmt=fits omode=out ofmt=fits out="/tmp/${NAME}_intermediate.fits" in="/tmp/${NAME}_intermediate.fits" cmd="delcols 'SIMBAD_2 GroupID GroupSize *.confidence'"
#do not remove origin
#stilts $FLAGS tpipe  ifmt=fits omode=out ofmt=fits out="/tmp/${NAME}_intermediate.fits" in="/tmp/${NAME}_intermediate.fits" cmd="delcols '*.origin '"
stilts $FLAGS tpipe  ifmt=fits omode=out ofmt=fits out="/tmp/${NAME}_intermediate.fits" in="/tmp/${NAME}_intermediate.fits" cmd="colmeta -name SIMBAD SIMBAD_1"
stilts $FLAGS tpipe  ifmt=fits omode=out ofmt=fits out="/tmp/${NAME}_intermediate.fits" in="/tmp/${NAME}_intermediate.fits" cmd="delcols 'deletedFlag'"
stilts $FLAGS tpipe  ifmt=fits omode=out ofmt=fits out="/tmp/${NAME}_intermediate.fits" in="/tmp/${NAME}_intermediate.fits" cmd="delcols 'color_table* lum_class* SpType_JMMC*'"
#due to changes in JMDC following the publication at CDS, the file passed to update_ld_in_jmdc needs to have a supplementary column UD_TO_LD_CONVFACTOR added before call.
stilts $FLAGS tpipe  ifmt=fits omode=out ofmt=fits out="/tmp/${NAME}_intermediate.fits" in="/tmp/${NAME}_intermediate.fits" cmd="addcol -after MU_LAMBDA UD_TO_LD_CONVFACTOR 0.00" cmd="addcol -after UD_TO_LD_CONVFACTOR LDD_ORIG 0.00"
#add spectral type index columns etc.
stilts tpipe ifmt=fits in="/tmp/${NAME}_intermediate.fits" cmd='keepcols "SPTYPE"' omode=out ofmt=ascii out="/tmp/sptype.ascii"

alxDecodeSpectralType -i "/tmp/sptype.ascii" -o "/tmp/sptype.tst"

stilts tjoin nin=2 ifmt1=fits in1="/tmp/${NAME}_intermediate.fits" ifmt2=tst in2="/tmp/sptype.tst" ofmt=fits out=${NAME}_final.fits fixcols="dups" 

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
