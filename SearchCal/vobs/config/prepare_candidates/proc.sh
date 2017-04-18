#!/bin/bash
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************

echo "processing @ date: `date -u`"

# ref: 475366 stars (without duplicates)

# Use large heap + CMS GC:
STILTS_JAVA_OPTIONS="-Xms4g -Xmx8g"
# STILTS_JAVA_OPTIONS="$STILTS_JAVA_OPTIONS -Djava.io.tmpdir=/home/users/bourgesl/tmp/"
# tmp is full
# /home/users/bourgesl/tmp/

# define catalogs:
CAT_ASCC_ORIG="ASCC_full.vot"
CAT_SIMBAD_ORIG="SIMBAD_full.vot"


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


# ASCC: filter cols: (ra/dec J2000 corrected with pm)
CAT_ASCC_MIN="1_min_$CAT_ASCC"
if [ $CAT_ASCC -nt $CAT_ASCC_MIN ]
then
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_ASCC cmd='progress ; keepcols "_RAJ2000 _DEJ2000 pmRA pmDE" ' out=$CAT_ASCC_MIN
fi

# ASCC: internal match 5" to identify inner groups
CAT_ASCC_MIN_GRP="1_min_Grp_$CAT_ASCC"
if [ $CAT_ASCC_MIN -nt $CAT_ASCC_MIN_GRP ]
then
    stilts ${STILTS_JAVA_OPTIONS} tmatch1 matcher='sky' params='5.0' values='_RAJ2000 _DEJ2000' action='identify' in=$CAT_ASCC_MIN out=$CAT_ASCC_MIN_GRP
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_ASCC_MIN_GRP cmd='progress ; delcols "GroupID" ; colmeta -name ASCC_GROUP_SIZE_5 GroupSize ' out=$CAT_ASCC_MIN_GRP
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_ASCC_MIN_GRP > $CAT_ASCC_MIN_GRP.stats.log

# Total Rows: 2501313
# +-------------------+------------+------------+-----------+-----------+---------+
# | column            | mean       | stdDev     | min       | max       | good    |
# +-------------------+------------+------------+-----------+-----------+---------+
# | _RAJ2000          | 188.87096  | 100.47709  | 3.41E-4   | 359.99988 | 2501313 |
# | _DEJ2000          | -3.317228  | 41.249565  | -89.88966 | 89.91136  | 2501313 |
# | pmRA              | -1.7540642 | 32.2306    | -4417.35  | 6773.35   | 2501313 |
# | pmDE              | -6.0875754 | 30.390753  | -5813.7   | 10308.08  | 2501313 |
# | ASCC_GROUP_SIZE_5 | 2.0395231  | 0.20365836 | 2         | 4         | 36409   |
# +-------------------+------------+------------+-----------+-----------+---------+
fi

# SIMBAD: filter cols:
# (main_id/ra/dec) only
CAT_SIMBAD_MIN="1_min_$CAT_SIMBAD"
if [ $CAT_SIMBAD -nt $CAT_SIMBAD_MIN ]
then
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_SIMBAD cmd='progress ; keepcols "ra dec main_id" ' out=$CAT_SIMBAD_MIN
fi

# column subset:
CAT_SIMBAD_SUB="1_sub_$CAT_SIMBAD"
if [ $CAT_SIMBAD -nt $CAT_SIMBAD_SUB ]
then
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_SIMBAD cmd='progress ; keepcols "ra dec main_id sp_type main_type other_types" ' out=$CAT_SIMBAD_SUB
fi


# 2. Crossmatch ASCC / SIMBAD 5" to identify groups
CAT_ASCC_SIMBAD_GRP="2_Grp_xmatch_ASCC_SIMBAD_full.fits"
if [ $CAT_ASCC_MIN_GRP -nt $CAT_ASCC_SIMBAD_GRP ] || [ $CAT_SIMBAD -nt $CAT_ASCC_SIMBAD_GRP ]
then
    stilts ${STILTS_JAVA_OPTIONS} tmatch2 matcher='sky' params='5.0' values1='_RAJ2000 _DEJ2000' values2='ra dec' find='best1' fixcols='all' join='all1' in1=$CAT_ASCC_MIN_GRP in2=$CAT_SIMBAD_MIN out=$CAT_ASCC_SIMBAD_GRP
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_ASCC_SIMBAD_GRP cmd='progress ; delcols "main_id_2 GroupID Separation" ; colmeta -name SIMBAD_GROUP_SIZE_5 GroupSize ' out=$CAT_ASCC_SIMBAD_GRP
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_ASCC_SIMBAD_GRP > $CAT_ASCC_SIMBAD_GRP.stats.log

# Total Rows: 2501313
# +---------------------+------------+------------+-----------+-----------+---------+
# | column              | mean       | stdDev     | min       | max       | good    |
# +---------------------+------------+------------+-----------+-----------+---------+
# | _RAJ2000_1          | 188.87096  | 100.47709  | 3.41E-4   | 359.99988 | 2501313 |
# | _DEJ2000_1          | -3.317228  | 41.249565  | -89.88966 | 89.91136  | 2501313 |
# | pmRA_1              | -1.7540642 | 32.2306    | -4417.35  | 6773.35   | 2501313 |
# | pmDE_1              | -6.0875754 | 30.390753  | -5813.7   | 10308.08  | 2501313 |
# | ASCC_GROUP_SIZE_5_1 | 2.0395231  | 0.20365836 | 2         | 4         | 36409   |
# | ra_2                | 188.4341   | 100.40486  | 3.96E-4   | 359.99988 | 2464924 |
# | dec_2               | -3.3820732 | 41.38795   | -89.88966 | 89.83234  | 2464924 |
# | SIMBAD_GROUP_SIZE_5 | 2.0129533  | 0.1130725  | 2         | 3         | 26866   |
# +---------------------+------------+------------+-----------+-----------+---------+
fi


# 2. Crossmatch ASCC / SIMBAD to get both DATA: 2.0"
CAT_ASCC_SIMBAD_XMATCH="2_xmatch_ASCC_SIMBAD_full.fits"
if [ $CAT_ASCC_SIMBAD_GRP -nt $CAT_ASCC_SIMBAD_XMATCH ]
then
    # best1 to have all possible ASCC candidates corresponding to the SIMBAD object:
    stilts ${STILTS_JAVA_OPTIONS} tmatch2 matcher='sky' params='2.0' values1='_RAJ2000_1 _DEJ2000_1' values2='ra dec' find='best1' fixcols='all' join='all1' in1=$CAT_ASCC_SIMBAD_GRP in2=$CAT_SIMBAD_SUB out=$CAT_ASCC_SIMBAD_XMATCH 
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_ASCC_SIMBAD_XMATCH cmd='progress ; delcols "ra_2_1 dec_2_1 GroupID" ; colmeta -name SIMBAD_GROUP_SIZE_2 GroupSize ' out=$CAT_ASCC_SIMBAD_XMATCH
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_ASCC_SIMBAD_XMATCH > $CAT_ASCC_SIMBAD_XMATCH.stats.log

# Total Rows: 2501313
# +-----------------------+-------------+-------------+-----------+-----------+---------+
# | column                | mean        | stdDev      | min       | max       | good    |
# +-----------------------+-------------+-------------+-----------+-----------+---------+
# | _RAJ2000_1_1          | 188.87096   | 100.47709   | 3.41E-4   | 359.99988 | 2501313 |
# | _DEJ2000_1_1          | -3.317228   | 41.249565   | -89.88966 | 89.91136  | 2501313 |
# | pmRA_1_1              | -1.7540642  | 32.2306     | -4417.35  | 6773.35   | 2501313 |
# | pmDE_1_1              | -6.0875754  | 30.390753   | -5813.7   | 10308.08  | 2501313 |
# | ASCC_GROUP_SIZE_5_1_1 | 2.0395231   | 0.20365836  | 2         | 4         | 36409   |
# | SIMBAD_GROUP_SIZE_5_1 | 2.0129533   | 0.1130725   | 2         | 3         | 26866   |
# | ra_2a                 | 188.43883   | 100.39977   | 3.96E-4   | 359.99988 | 2459685 |
# | dec_2a                | -3.3877246  | 41.386772   | -89.88966 | 89.83234  | 2459685 |
# | main_id_2             |             |             |           |           | 2459685 |
# | sp_type_2             |             |             |           |           | 494401  |
# | main_type_2           |             |             |           |           | 2459685 |
# | other_types_2         |             |             |           |           | 2459680 |
# | SIMBAD_GROUP_SIZE_2   | 2.005963    | 0.076990604 | 2         | 3         | 21633   |
# | Separation            | 0.039434206 | 0.111414105 | 0.0       | 1.999803  | 2459685 |
# +-----------------------+-------------+-------------+-----------+-----------+---------+
fi


# 3. Prepare candidates: columns, sort, then filters:
FULL_CANDIDATES="3_xmatch_ASCC_SIMBAD_full_cols.fits"
if [ $CAT_ASCC_SIMBAD_XMATCH -nt $FULL_CANDIDATES ]
then
    # TODO: check if main_type_2 is useful (should be included in other_types_2)
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_ASCC_SIMBAD_XMATCH cmd='progress ; addcol -after sp_type_2 -ucd OBJ_TYPES OTYPES NULL_main_type_2?\"\":replaceAll(concat(\",\",main_type_2,\",\",replaceAll(other_types_2,\"[|]\",\",\"),\",\"),\"\ \",\"\") ; delcols "ra_2a dec_2a main_type_2 other_types_2 SIMBAD_GROUP_SIZE_2 Separation" ; addcol -before _RAJ2000_1_1 -ucd POS_EQ_RA_MAIN RAJ2000 degreesToHms(_RAJ2000_1_1,4) ; addcol -after RAJ2000 -ucd POS_EQ_DEC_MAIN DEJ2000 degreesToDms(_DEJ2000_1_1,4) ; addcol -after DEJ2000 -ucd POS_EQ_PMRA pmRA pmRA_1_1 ; addcol -after pmRA -ucd POS_EQ_PMDEC pmDE pmDE_1_1 ; addcol -after pmDE -ucd ID_MAIN MAIN_ID main_id_2 ; colmeta -name SP_TYPE -ucd SPECT_TYPE_MK sp_type_2 ; addcol -ucd GROUP_SIZE GROUP_SIZE_5 max((!NULL_ASCC_GROUP_SIZE_5_1_1?ASCC_GROUP_SIZE_5_1_1:0),(!NULL_SIMBAD_GROUP_SIZE_5_1?SIMBAD_GROUP_SIZE_5_1:0)) ; sort _DEJ2000_1_1 ; delcols "_RAJ2000_1_1 _DEJ2000_1_1 ASCC_GROUP_SIZE_5_1_1 SIMBAD_GROUP_SIZE_5_1 pmRA_1_1 pmDE_1_1 main_id_2" ' out=$FULL_CANDIDATES
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$FULL_CANDIDATES > $FULL_CANDIDATES.stats.log

# Total Rows: 2501313
# +--------------+-------------+------------+----------+----------+---------+
# | column       | mean        | stdDev     | min      | max      | good    |
# +--------------+-------------+------------+----------+----------+---------+
# | RAJ2000      |             |            |          |          | 2501313 |
# | DEJ2000      |             |            |          |          | 2501313 |
# | pmRA         | -1.7540642  | 32.2306    | -4417.35 | 6773.35  | 2501313 |
# | pmDE         | -6.0875754  | 30.390753  | -5813.7  | 10308.08 | 2501313 |
# | MAIN_ID      |             |            |          |          | 2459685 |
# | SP_TYPE      |             |            |          |          | 494401  |
# | OTYPES       |             |            |          |          | 2501313 |
# | GROUP_SIZE_5 | 0.029712794 | 0.24560091 | 0        | 4        | 2501313 |
# +--------------+-------------+------------+----------+----------+---------+
fi


# 3. Prepare candidate catalogs using the following format:
# Name        UCD
# RAJ2000	$2	String	"h:m:s"		POS_EQ_RA_MAIN	Right Ascension	char	HMS->degrees	
# DEJ2000	$3	String	"d:m:s"		POS_EQ_DEC_MAIN	Declination	char	DMS->degrees	
# pmRA	$4	Double			POS_EQ_PMRA	Proper Motion in Right Ascension (pmra)	double		
# pmDE	$5	Double			POS_EQ_PMDEC	Proper Motion in Declination (pmdec)	double		
# MAIN_ID	$6	String			ID_MAIN	Main Identifier of a Celestial Object	char		
# SP_TYPE	$7	String			SPECT_TYPE_MK	MK Spectral Classification	char		
# OTYPES	$8	String			OBJ_TYPES		char		
# GROUP_SIZE_5	$9	Integer		Number of rows in match group	GROUP_SIZE		int		-2147483648

# Convert to VOTables:
# - bright: select stars within 2 arcsecs WITH SP_TYPE
CATALOG_BRIGHT="3_xmatch_ASCC_SIMBAD_full_2as_sptype.vot"
if [ $FULL_CANDIDATES -nt $CATALOG_BRIGHT ]
then
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$FULL_CANDIDATES cmd='progress; select "!NULL_SP_TYPE" ' out=$CATALOG_BRIGHT
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CATALOG_BRIGHT > $CATALOG_BRIGHT.stats.log
fi

# - faint: select stars within 2 arcsecs WITHOUT SP_TYPE
CATALOG_FAINT="3_xmatch_ASCC_SIMBAD_full_2as.vot"
if [ $FULL_CANDIDATES -nt $CATALOG_FAINT ]
then
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$FULL_CANDIDATES cmd='progress; select "NULL_SP_TYPE" ; delcols "SP_TYPE" ' out=$CATALOG_FAINT
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CATALOG_FAINT > $CATALOG_FAINT.stats.log
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


