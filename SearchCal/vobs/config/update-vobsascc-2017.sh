#!/bin/bash
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************

# Use large heap + CMS GC:
STILTS_JAVA_OPTIONS="-Xms4g -Xmx8g"

# Merge JSDC 2017 with prepare_candidates/3_xmatch_ASCC_SIMBAD_full_cols.fits:

CAT_JSDC_2020="vobsascc_simbad_sptype-2020.fits"

if [ prepare_candidates/3_xmatch_ASCC_SIMBAD_full_cols.fits -nt $CAT_JSDC_2020 ]
then
    stilts ${STILTS_JAVA_OPTIONS} tmatch2 matcher='sky' params='0.1' values1='hmsToDegrees(RAJ2000) dmsToDegrees(DEJ2000)' values2='hmsToDegrees(RAJ2000) dmsToDegrees(DEJ2000)' find='best1' fixcols='all' join='all1' in1=stilts/vobsascc_simbad_sptype-topcat.tst ifmt1="tst" in2=prepare_candidates/3_xmatch_ASCC_SIMBAD_full_cols.fits out=$CAT_JSDC_2020
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_JSDC_2020 > $CAT_JSDC_2020.stats.log
fi

# Output
CAT_JSDC_FINAL="vobsascc_simbad_sptype-2020-final.vot"
if [ $CAT_JSDC_2020 -nt $CAT_JSDC_FINAL ]
then
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_JSDC_2020 cmd='progress ; delcols "RAJ2000_1 DEJ2000_1 pmRA_1 pmDE_1 MAIN_ID_1 SP_TYPE_1 OTYPES_1 GROUP_SIZE_5_1 Separation" ; colmeta -name RAJ2000 RAJ2000_2 ; colmeta -name DEJ2000 DEJ2000_2 ; colmeta -name pmRA pmRA_2 ; colmeta -name pmDE pmDE_2 ; colmeta -name MAIN_ID MAIN_ID_2 ; colmeta -name SP_TYPE SP_TYPE_2 ; colmeta -name OTYPES OTYPES_2 ; colmeta -name XM_SIMBAD_sep XM_SIMBAD_sep_2 ; colmeta -name GROUP_SIZE_3 GROUP_SIZE_3_2 ; select "!NULL_SP_TYPE" ' out=$CAT_JSDC_FINAL
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_JSDC_FINAL > $CAT_JSDC_FINAL.stats.log
fi

# 4. Generate SearchCal config files:
CONFIG="vobsascc_simbad_sptype-2020.cfg"
xsltproc -o $CONFIG prepare_candidates/sclguiVOTableToTSV.xsl $CAT_JSDC_FINAL


echo "TODO: fix table header in $CONFIG"

