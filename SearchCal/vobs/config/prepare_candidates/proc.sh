#!/bin/bash
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************

echo "processing @ date: `date -u`"

# Use large heap + CMS GC:
STILTS_JAVA_OPTIONS="-Xms4g -Xmx8g"
# STILTS_JAVA_OPTIONS="$STILTS_JAVA_OPTIONS -Djava.io.tmpdir=/home/users/bourgesl/tmp/"
# tmp is full
# /home/users/bourgesl/tmp/

# debug bash commands:
# set -eux;


# define catalogs:
CAT_ASCC_ORIG="ASCC_full.vot"
CAT_SIMBAD_ORIG="SIMBAD_full.vot"
CAT_MDFC="mdfc-v10.fits"


# 0. Download full ASCC (xmatch):
if [ ! -e $CAT_ASCC_ORIG ]
then
    wget -O $CAT_ASCC_ORIG "http://cdsxmatch.u-strasbg.fr/QueryCat/QueryCat?catName=I%2F280B%2Fascc&mode=allsky&format=votable"
fi
#    Download full SIMBAD (xmatch):
if [ ! -e $CAT_SIMBAD_ORIG ]
then
    wget -O $CAT_SIMBAD_ORIG "http://cdsxmatch.u-strasbg.fr/QueryCat/QueryCat?catName=SIMBAD&mode=allsky&format=votable"
fi
#    Download MDFC (OCA):
if [ ! -e $CAT_MDFC ]
then
    CAT_MDFC_ZIP="mdfc-v10.zip"
    if [ ! -e $CAT_MDFC_ZIP ]
    then
        wget -O $CAT_MDFC_ZIP "https://matisse.oca.eu/foswiki/pub/Main/TheMid-infraredStellarDiametersAndFluxesCompilationCatalogue%28MDFC%29/mdfc-v10.zip"
    fi
    unzip $CAT_MDFC_ZIP $CAT_MDFC
    if [ ! -e $CAT_MDFC ]
    then
        echo "missing MDFC catalog"
        exit 1
    fi
fi


# 1. Convert ASCC to FITS:
CAT_ASCC="$CAT_ASCC_ORIG.fits"
if [ $CAT_ASCC_ORIG -nt $CAT_ASCC ]
then
    stilts ${STILTS_JAVA_OPTIONS} tcopy in=$CAT_ASCC_ORIG out=$CAT_ASCC
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_ASCC > $CAT_ASCC.stats.log
fi

#    Convert SIMBAD to FITS:
CAT_SIMBAD="$CAT_SIMBAD_ORIG.fits"
if [ $CAT_SIMBAD_ORIG -nt $CAT_SIMBAD ]
then
    stilts ${STILTS_JAVA_OPTIONS} tcopy in=$CAT_SIMBAD_ORIG out=$CAT_SIMBAD
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_SIMBAD > $CAT_SIMBAD.stats.log
fi


# ASCC: filter columns: (ra/dec J2000 corrected with pm)
CAT_ASCC_MIN="1_min_$CAT_ASCC"
if [ $CAT_ASCC -nt $CAT_ASCC_MIN ]
then
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_ASCC cmd='progress ; keepcols "_RAJ2000 _DEJ2000 pmRA pmDE" ' out=$CAT_ASCC_MIN
fi

# ASCC: internal match within 3 as to identify inner groups
CAT_ASCC_MIN_GRP="1_min_Grp_$CAT_ASCC"
if [ $CAT_ASCC_MIN -nt $CAT_ASCC_MIN_GRP ]
then
    stilts ${STILTS_JAVA_OPTIONS} tmatch1 matcher='sky' params='3.0' values='_RAJ2000 _DEJ2000' action='identify' in=$CAT_ASCC_MIN out=$CAT_ASCC_MIN_GRP
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_ASCC_MIN_GRP cmd='progress ; delcols "GroupID" ; colmeta -name ASCC_GROUP_SIZE GroupSize ' out=$CAT_ASCC_MIN_GRP
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_ASCC_MIN_GRP > $CAT_ASCC_MIN_GRP.stats.log

# Total Rows: 2501313
# +-----------------+------------+-----------+-----------+-----------+---------+
# | column          | mean       | stdDev    | min       | max       | good    |
# +-----------------+------------+-----------+-----------+-----------+---------+
# | _RAJ2000        | 188.87096  | 100.47709 | 3.41E-4   | 359.99988 | 2501313 |
# | _DEJ2000        | -3.317228  | 41.249565 | -89.88966 | 89.91136  | 2501313 |
# | pmRA            | -1.7540642 | 32.2306   | -4417.35  | 6773.35   | 2501313 |
# | pmDE            | -6.0875754 | 30.390753 | -5813.7   | 10308.08  | 2501313 |
# | ASCC_GROUP_SIZE | 2.0271235  | 0.1670802 | 2         | 4         | 31412   |
# +-----------------+------------+-----------+-----------+-----------+---------+
fi


# Filter SIMBAD objects:
CAT_SIMBAD_OBJ="1_obj_$CAT_SIMBAD"
if [ $CAT_SIMBAD -nt $CAT_SIMBAD_OBJ ]
then
    # filter SIMBAD main_type (stars only) to exclude Planet, Galaxy ...
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_SIMBAD cmd='progress; select other_types.contains(\"*\")||main_type.contains(\"*\")||main_type.equals(\"Star\") ' out=$CAT_SIMBAD_OBJ
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_SIMBAD_OBJ > $CAT_SIMBAD_OBJ.stats.log

# Total Rows: 5492984
# +---------------+-------------+------------+-----------+-----------+---------+
# | column        | mean        | stdDev     | min       | max       | good    |
# +---------------+-------------+------------+-----------+-----------+---------+
# | main_id       |             |            |           |           | 5492984 |
# | ra            | 178.82549   | 102.579506 | 0.0       | 359.99997 | 5492984 |
# | dec           | -4.0715885  | 41.765236  | -89.88967 | 89.851585 | 5492984 |
# | coo_err_maj   | 0.038114276 | 3.6458046  | 0.0       | 3380.0    | 5096511 |
# | coo_err_min   | 0.032437317 | 2.5540175  | 0.0       | 1930.0    | 5096511 |
# | coo_err_angle | 94.107925   | 25.468988  | 0.0       | 179.0     | 5492984 |
# | nbref         | 2.375903    | 14.166813  | 0         | 5333      | 5492984 |
# | ra_sexa       |             |            |           |           | 5492984 |
# | dec_sexa      |             |            |           |           | 5492984 |
# | main_type     |             |            |           |           | 5492984 |
# | other_types   |             |            |           |           | 5492984 |
# | radvel        | 19849.314   | 60153.7    | -58663.2  | 293760.8  | 2934234 |
# | redshift      | 0.15120743  | 0.5337434  | -9.99898  | 8.92      | 2934237 |
# | sp_type       |             |            |           |           | 828184  |
# | morph_type    |             |            |           |           | 41256   |
# | plx           | 2.2845182   | 4.1535573  | -21.0     | 768.5004  | 4040247 |
# | pmra          | -1.377984   | 38.44833   | -4800.0   | 6766.0    | 4537995 |
# | pmdec         | -6.817751   | 36.410965  | -5817.86  | 10362.54  | 4537995 |
# | size_maj      | 0.42574927  | 10.906192  | 0.0       | 3438.0    | 202014  |
# | size_min      | 0.28035554  | 8.180499   | 0.0       | 3438.0    | 202011  |
# | size_angle    | 745.2638    | 4583.97    | -85       | 32767     | 202057  |
# | B             | 13.336429   | 2.9391332  | -1.46     | 45.38     | 3666918 |
# | V             | 12.959677   | 3.2722359  | -1.46     | 31.92     | 3877539 |
# | R             | 14.334398   | 2.9207342  | -1.46     | 50.0      | 682146  |
# | J             | 11.156289   | 2.6399913  | -3.0      | 29.25     | 4199694 |
# | H             | 10.674416   | 2.6306155  | -3.73     | 29.164    | 4189993 |
# | K             | 10.527715   | 2.66535    | -4.05     | 26.4      | 4189906 |
# | u             | 20.282812   | 2.171564   | 0.0       | 31.224    | 598810  |
# | g             | 16.598894   | 3.6802232  | 0.0       | 30.471    | 1008851 |
# | r             | 15.928052   | 3.7799177  | 0.003     | 31.197    | 1012493 |
# | i             | 15.51196    | 3.8336356  | 0.004     | 30.139    | 1018245 |
# | z             | 17.949238   | 2.0936356  | 0.0       | 31.741    | 619039  |
# +---------------+-------------+------------+-----------+-----------+---------+
fi

# SIMBAD: filter columns:
# (main_id/ra/dec) only to identify groups
CAT_SIMBAD_MIN="1_min_$CAT_SIMBAD_OBJ"
if [ $CAT_SIMBAD_OBJ -nt $CAT_SIMBAD_MIN ]
then
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_SIMBAD_OBJ cmd='progress ; keepcols "ra dec main_id" ' out=$CAT_SIMBAD_MIN
fi

# SIMBAD: column subset for candidates
CAT_SIMBAD_SUB="1_sub_$CAT_SIMBAD_OBJ"
if [ $CAT_SIMBAD_MIN -nt $CAT_SIMBAD_SUB ]
then
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_SIMBAD_OBJ cmd='progress ; keepcols "ra dec main_id sp_type main_type other_types" ' out=$CAT_SIMBAD_SUB
fi


# MDFC: filter columns for candidates
CAT_MDFC_MIN="1_min_$CAT_MDFC"
if [ $CAT_MDFC -nt $CAT_MDFC_MIN ]
then
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_MDFC cmd='progress ; keepcols "Name IRflag med_Lflux disp_Lflux med_Mflux disp_Mflux med_Nflux disp_Nflux" ; colmeta -ucd IR_FLAG IRflag ; colmeta -name Lflux_med -ucd PHOT_FLUX_L med_Lflux ; addcol -after Lflux_med -ucd PHOT_FLUX_L_ERROR e_Lflux_med 1.4826*disp_Lflux ; colmeta -name Mflux_med -ucd PHOT_FLUX_M med_Mflux ; addcol -after Mflux_med -ucd PHOT_FLUX_M_ERROR e_Mflux_med 1.4826*disp_Mflux ; colmeta -name Nflux_med -ucd PHOT_FLUX_N med_Nflux ; addcol -after Nflux_med -ucd PHOT_FLUX_N_ERROR e_Nflux_med 1.4826*disp_Nflux; delcols "disp_Lflux disp_Mflux disp_Nflux" ' out=$CAT_MDFC_MIN
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_MDFC_MIN > $CAT_MDFC_MIN.stats.log

# Total Rows: 465857
# +-------------+------------+-----------+--------------+-----------+--------+
# | column      | mean       | stdDev    | min          | max       | good   |
# +-------------+------------+-----------+--------------+-----------+--------+
# | Name        |            |           |              |           | 465857 |
# | IRflag      | 1.0827936  | 1.554538  | 0            | 7         | 465857 |
# | Lflux_med   | 1.5254774  | 42.61173  | 1.6299028E-4 | 15720.791 | 465755 |
# | e_Lflux_med | 0.458417   | 23.706154 | 3.0239256E-8 | 11478.607 | 432072 |
# | Mflux_med   | 0.8892675  | 19.973076 | 8.886453E-5  | 7141.7    | 465757 |
# | e_Mflux_med | 0.33401743 | 14.302171 | 8.423023E-10 | 6363.4277 | 432130 |
# | Nflux_med   | 0.33924013 | 18.71475  | 9.8775825E-5 | 9278.1    | 465750 |
# | e_Nflux_med | 0.12537919 | 7.3004456 | 9.378568E-10 | 3538.2249 | 433969 |
# +-------------+------------+-----------+--------------+-----------+--------+
fi


# 2. Crossmatch ASCC / SIMBAD within 3 as to identify groups
CAT_ASCC_SIMBAD_GRP="2_Grp_xmatch_ASCC_SIMBAD_full.fits"
if [ $CAT_ASCC_MIN_GRP -nt $CAT_ASCC_SIMBAD_GRP ] || [ $CAT_SIMBAD_MIN -nt $CAT_ASCC_SIMBAD_GRP ]
then
    stilts ${STILTS_JAVA_OPTIONS} tmatch2 matcher='sky' params='3.0' values1='_RAJ2000 _DEJ2000' values2='ra dec' find='best1' join='all1' in1=$CAT_ASCC_MIN_GRP in2=$CAT_SIMBAD_MIN out=$CAT_ASCC_SIMBAD_GRP
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_ASCC_SIMBAD_GRP cmd='progress ; delcols "ra dec main_id GroupID Separation" ; colmeta -name SIMBAD_GROUP_SIZE GroupSize ' out=$CAT_ASCC_SIMBAD_GRP
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_ASCC_SIMBAD_GRP > $CAT_ASCC_SIMBAD_GRP.stats.log

# Total Rows: 2501313
# +-------------------+------------+------------+-------------+-----------+---------+
# | column            | mean       | stdDev     | min         | max       | good    |
# +-------------------+------------+------------+-------------+-----------+---------+
# | _RAJ2000          | 188.87096  | 100.47709  | 3.41E-4     | 359.99988 | 2501313 |
# | _DEJ2000          | -3.317228  | 41.249565  | -89.88966   | 89.91136  | 2501313 |
# | pmRA              | -1.7540642 | 32.2306    | -4417.35    | 6773.35   | 2501313 |
# | pmDE              | -6.0875754 | 30.390753  | -5813.7     | 10308.08  | 2501313 |
# | ASCC_GROUP_SIZE   | 2.0271235  | 0.1670802  | 2           | 4         | 31412   |
# | SIMBAD_GROUP_SIZE | 2.0057063  | 0.07532475 | 2           | 3         | 11566   |
# +-------------------+------------+------------+-------------+-----------+---------+
fi

# 2. Crossmatch ASCC / SIMBAD within 1 as to get both DATA
CAT_ASCC_SIMBAD_XMATCH="2_xmatch_ASCC_SIMBAD_full.fits"
if [ $CAT_ASCC_SIMBAD_GRP -nt $CAT_ASCC_SIMBAD_XMATCH ] || [ $CAT_SIMBAD_SUB -nt $CAT_ASCC_SIMBAD_XMATCH ]
then
    # best1 to have all possible ASCC candidates corresponding to the SIMBAD object:
    stilts ${STILTS_JAVA_OPTIONS} tmatch2 matcher='sky' params='1.0' values1='_RAJ2000 _DEJ2000' values2='ra dec' find='best1' join='all1' in1=$CAT_ASCC_SIMBAD_GRP in2=$CAT_SIMBAD_SUB out=$CAT_ASCC_SIMBAD_XMATCH 
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_ASCC_SIMBAD_XMATCH cmd='progress ; delcols "ra dec GroupSize GroupID" ; colmeta -name XM_SIMBAD_SEP -ucd XM_SIMBAD_SEP Separation ' out=$CAT_ASCC_SIMBAD_XMATCH
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_ASCC_SIMBAD_XMATCH > $CAT_ASCC_SIMBAD_XMATCH.stats.log

# Total Rows: 2501313
# +-------------------+------------+------------+-----------+------------+---------+
# | column            | mean       | stdDev     | min       | max        | good    |
# +-------------------+------------+------------+-----------+------------+---------+
# | _RAJ2000          | 188.87096  | 100.47709  | 3.41E-4   | 359.99988  | 2501313 |
# | _DEJ2000          | -3.317228  | 41.249565  | -89.88966 | 89.91136   | 2501313 |
# | pmRA              | -1.7540642 | 32.2306    | -4417.35  | 6773.35    | 2501313 |
# | pmDE              | -6.0875754 | 30.390753  | -5813.7   | 10308.08   | 2501313 |
# | ASCC_GROUP_SIZE   | 2.0271235  | 0.1670802  | 2         | 4          | 31412   |
# | SIMBAD_GROUP_SIZE | 2.0057063  | 0.07532475 | 2         | 3          | 11566   |
# | main_id           |            |            |           |            | 2461748 |
# | sp_type           |            |            |           |            | 500162  |
# | main_type         |            |            |           |            | 2461748 |
# | other_types       |            |            |           |            | 2461748 |
# | XM_SIMBAD_SEP     | 0.11238496 | 0.1082592  | 0.0       | 0.99995977 | 2461748 |
# +-------------------+------------+------------+-----------+------------+---------+
fi


# 2. Crossmatch ASCC / SIMBAD / MDFC (SIMBAD main_id):
CAT_ASCC_SIMBAD_MDFC_XMATCH="2_xmatch_ASCC_SIMBAD_MDFC_full.fits"
if [ $CAT_ASCC_SIMBAD_XMATCH -nt $CAT_ASCC_SIMBAD_MDFC_XMATCH ] || [ $CAT_MDFC_MIN -nt $CAT_ASCC_SIMBAD_MDFC_XMATCH ]
then
    stilts ${STILTS_JAVA_OPTIONS} tmatch2 matcher='exact' values1='main_id' values2='Name' join='all1' in1=$CAT_ASCC_SIMBAD_XMATCH in2=$CAT_MDFC_MIN out=$CAT_ASCC_SIMBAD_MDFC_XMATCH
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_ASCC_SIMBAD_MDFC_XMATCH cmd='progress ; delcols "Name" ' out=$CAT_ASCC_SIMBAD_MDFC_XMATCH
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_ASCC_SIMBAD_MDFC_XMATCH > $CAT_ASCC_SIMBAD_MDFC_XMATCH.stats.log

# Total Rows: 2501313
# +-------------------+-------------+------------+--------------+------------+---------+
# | column            | mean        | stdDev     | min          | max        | good    |
# +-------------------+-------------+------------+--------------+------------+---------+
# | _RAJ2000          | 188.87096   | 100.47709  | 3.41E-4      | 359.99988  | 2501313 |
# | _DEJ2000          | -3.317228   | 41.249565  | -89.88966    | 89.91136   | 2501313 |
# | pmRA              | -1.7540642  | 32.2306    | -4417.35     | 6773.35    | 2501313 |
# | pmDE              | -6.0875754  | 30.390753  | -5813.7      | 10308.08   | 2501313 |
# | ASCC_GROUP_SIZE   | 2.0271235   | 0.1670802  | 2            | 4          | 31412   |
# | SIMBAD_GROUP_SIZE | 2.0057063   | 0.07532475 | 2            | 3          | 11566   |
# | main_id           |             |            |              |            | 2461748 |
# | sp_type           |             |            |              |            | 500162  |
# | main_type         |             |            |              |            | 2461748 |
# | other_types       |             |            |              |            | 2461748 |
# | XM_SIMBAD_SEP     | 0.11238496  | 0.1082592  | 0.0          | 0.99995977 | 2461748 |
# | IRflag            | 1.0790151   | 1.5525337  | 0            | 7          | 461608  |
# | Lflux_med         | 1.4995508   | 41.104538  | 1.6299028E-4 | 15720.791  | 461512  |
# | e_Lflux_med       | 0.42962062  | 16.085009  | 3.0239256E-8 | 5404.2817  | 428814  |
# | Mflux_med         | 0.8740168   | 18.899055  | 8.886453E-5  | 7141.7     | 461515  |
# | e_Mflux_med       | 0.31775543  | 10.563476  | 8.423023E-10 | 3063.0676  | 428870  |
# | Nflux_med         | 0.3261291   | 17.666105  | 9.8775825E-5 | 9278.1     | 461508  |
# | e_Nflux_med       | 0.112476036 | 4.0161715  | 9.378568E-10 | 1230.4095  | 430583  |
# +-------------------+-------------+------------+--------------+------------+---------+
fi


# 3. Prepare candidates: columns, sort, then filters:
FULL_CANDIDATES="3_xmatch_ASCC_SIMBAD_MDFC_full_cols.fits"
if [ $CAT_ASCC_SIMBAD_MDFC_XMATCH -nt $FULL_CANDIDATES ]
then
    # note: merge (main_type,other_types) as other_types can be NULL:
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_ASCC_SIMBAD_MDFC_XMATCH cmd='progress ; addcol -after sp_type -ucd OBJ_TYPES OTYPES NULL_main_type?\"\":replaceAll(concat(\",\",main_type,\",\",replaceAll(other_types,\"[|]\",\",\"),\",\"),\"\ \",\"\") ; delcols "main_type other_types" ; addcol -before _RAJ2000 -ucd POS_EQ_RA_MAIN RAJ2000 degreesToHms(_RAJ2000,5) ; addcol -after RAJ2000 -ucd POS_EQ_DEC_MAIN DEJ2000 degreesToDms(_DEJ2000,5) ; colmeta -ucd POS_EQ_PMRA pmRA ; colmeta -ucd POS_EQ_PMDEC pmDE ; colmeta -name MAIN_ID -ucd ID_MAIN main_id ; colmeta -name SP_TYPE -ucd SPECT_TYPE_MK sp_type ; addcol -after XM_SIMBAD_SEP -ucd GROUP_SIZE GROUP_SIZE_3 max((!NULL_ASCC_GROUP_SIZE?ASCC_GROUP_SIZE:0),(!NULL_SIMBAD_GROUP_SIZE?SIMBAD_GROUP_SIZE:0)) ; sort _DEJ2000 ; delcols "_RAJ2000 _DEJ2000 ASCC_GROUP_SIZE SIMBAD_GROUP_SIZE" ' out=$FULL_CANDIDATES
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$FULL_CANDIDATES > $FULL_CANDIDATES.stats.log

# Total Rows: 2501313
# +---------------+-------------+------------+--------------+------------+---------+
# | column        | mean        | stdDev     | min          | max        | good    |
# +---------------+-------------+------------+--------------+------------+---------+
# | RAJ2000       |             |            |              |            | 2501313 |
# | DEJ2000       |             |            |              |            | 2501313 |
# | pmRA          | -1.7540642  | 32.2306    | -4417.35     | 6773.35    | 2501313 |
# | pmDE          | -6.0875754  | 30.390753  | -5813.7      | 10308.08   | 2501313 |
# | MAIN_ID       |             |            |              |            | 2461748 |
# | SP_TYPE       |             |            |              |            | 500162  |
# | OTYPES        |             |            |              |            | 2461748 |
# | XM_SIMBAD_SEP | 0.11238496  | 0.1082592  | 0.0          | 0.99995977 | 2461748 |
# | GROUP_SIZE_3  | 0.025469823 | 0.22656564 | 0            | 4          | 2501313 |
# | IRflag        | 1.0790151   | 1.5525337  | 0            | 7          | 461608  |
# | Lflux_med     | 1.4995508   | 41.104538  | 1.6299028E-4 | 15720.791  | 461512  |
# | e_Lflux_med   | 0.42962062  | 16.085009  | 3.0239256E-8 | 5404.2817  | 428814  |
# | Mflux_med     | 0.8740168   | 18.899055  | 8.886453E-5  | 7141.7     | 461515  |
# | e_Mflux_med   | 0.31775543  | 10.563476  | 8.423023E-10 | 3063.0676  | 428870  |
# | Nflux_med     | 0.3261291   | 17.666105  | 9.8775825E-5 | 9278.1     | 461508  |
# | e_Nflux_med   | 0.112476036 | 4.0161715  | 9.378568E-10 | 1230.4095  | 430583  |
# +---------------+-------------+------------+--------------+------------+---------+
fi

# 3. Prepare candidate catalogs using the SearchCal cfg format:

# Convert to VOTables:
# - bright: select stars WITH SP_TYPE
CATALOG_BRIGHT="3_xmatch_ASCC_SIMBAD_MDFC_full_2as_sptype.vot"
if [ $FULL_CANDIDATES -nt $CATALOG_BRIGHT ]
then
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$FULL_CANDIDATES cmd='progress; select "!NULL_SP_TYPE" ' out=$CATALOG_BRIGHT
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CATALOG_BRIGHT > $CATALOG_BRIGHT.stats.log

# Total Rows: 500162
# +---------------+-------------+-------------+--------------+------------+--------+
# | column        | mean        | stdDev      | min          | max        | good   |
# +---------------+-------------+-------------+--------------+------------+--------+
# | RAJ2000       |             |             |              |            | 500162 |
# | DEJ2000       |             |             |              |            | 500162 |
# | pmRA          | -1.6031227  | 52.759445   | -4417.35     | 6773.35    | 500162 |
# | pmDE          | -8.998286   | 49.64335    | -5813.06     | 10308.08   | 500162 |
# | MAIN_ID       |             |             |              |            | 500162 |
# | SP_TYPE       |             |             |              |            | 500162 |
# | OTYPES        |             |             |              |            | 500162 |
# | XM_SIMBAD_SEP | 0.055007104 | 0.072424755 | 0.0          | 0.99983424 | 500162 |
# | GROUP_SIZE_3  | 0.06301558  | 0.351829    | 0            | 4          | 500162 |
# | IRflag        | 1.0765225   | 1.5553885   | 0            | 7          | 447163 |
# | Lflux_med     | 1.5278635   | 41.758827   | 1.6299028E-4 | 15720.791  | 447070 |
# | e_Lflux_med   | 0.43827438  | 16.346554   | 3.0239256E-8 | 5404.2817  | 415030 |
# | Mflux_med     | 0.8901432   | 19.198273   | 8.886453E-5  | 7141.7     | 447073 |
# | e_Mflux_med   | 0.32440892  | 10.73485    | 8.423023E-10 | 3063.0676  | 415085 |
# | Nflux_med     | 0.3329325   | 17.94806    | 9.8775825E-5 | 9278.1     | 447065 |
# | e_Nflux_med   | 0.11482929  | 4.081261    | 9.378568E-10 | 1230.4095  | 416778 |
# +---------------+-------------+-------------+--------------+------------+--------+
fi

# - faint: select stars WITHOUT SP_TYPE
CATALOG_FAINT="3_xmatch_ASCC_SIMBAD_full_2as.vot"
if [ $FULL_CANDIDATES -nt $CATALOG_FAINT ]
then
    # delete any MDFC columns as it should correspond to JSDC BRIGHT
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$FULL_CANDIDATES cmd='progress; select "NULL_SP_TYPE" ; delcols "SP_TYPE IRflag Lflux_med e_Lflux_med Mflux_med e_Mflux_med Nflux_med e_Nflux_med" ' out=$CATALOG_FAINT
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CATALOG_FAINT > $CATALOG_FAINT.stats.log

# Total Rows: 2001151
# +---------------+-------------+------------+----------+------------+---------+
# | column        | mean        | stdDev     | min      | max        | good    |
# +---------------+-------------+------------+----------+------------+---------+
# | RAJ2000       |             |            |          |            | 2001151 |
# | DEJ2000       |             |            |          |            | 2001151 |
# | pmRA          | -1.79179    | 24.5505    | -3681.47 | 4147.35    | 2001151 |
# | pmDE          | -5.3600807  | 23.14804   | -5813.7  | 3203.63    | 2001151 |
# | MAIN_ID       |             |            |          |            | 1961586 |
# | OTYPES        |             |            |          |            | 1961586 |
# | XM_SIMBAD_SEP | 0.12701507  | 0.11098415 | 0.0      | 0.99995977 | 1961586 |
# | GROUP_SIZE_3  | 0.016085742 | 0.1810614  | 0        | 4          | 2001151 |
# +---------------+-------------+------------+----------+------------+---------+
fi


# 4. Generate SearchCal config files:
CONFIG="vobsascc_simbad_sptype.cfg"
if [ $CATALOG_BRIGHT -nt $CONFIG ]
then
    xsltproc -o $CONFIG sclguiVOTableToTSV.xsl $CATALOG_BRIGHT
fi

CONFIG="vobsascc_simbad_no_sptype.cfg"
if [ $CATALOG_FAINT -nt $CONFIG ]
then
    xsltproc -o $CONFIG sclguiVOTableToTSV.xsl $CATALOG_FAINT
fi


echo "That's all folks ! @ date: `date -u`"


