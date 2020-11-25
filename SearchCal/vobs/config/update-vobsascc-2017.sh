#!/bin/bash
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************

# Use large heap + CMS GC:
STILTS_JAVA_OPTIONS="-Xms4g -Xmx8g"

# debug bash commands:
# set -eux;

# Merge JSDC 2017 with prepare_candidates/3_xmatch_ASCC_SIMBAD_MDFC_full_cols.fits

CAT_JSDC_2020="vobsascc_simbad_sptype-2020.fits"

if [ prepare_candidates/3_xmatch_ASCC_SIMBAD_MDFC_full_cols.fits ]
then
    # coordinates are precise up to 4-6 digits on arcsecs: use 10 mas << closest object in ASCC
    # Get all neightbours within 3 as:
    stilts ${STILTS_JAVA_OPTIONS} tmatch2 matcher='sky' params='3.0' values1='hmsToDegrees(RAJ2000) dmsToDegrees(DEJ2000)' values2='hmsToDegrees(RAJ2000) dmsToDegrees(DEJ2000)' find='all' join='all1' fixcols='all' suffix2='' in1=stilts/vobsascc_simbad_sptype-topcat.tst ifmt1="tst" in2=prepare_candidates/3_xmatch_ASCC_SIMBAD_MDFC_full_sptype.vot out=$CAT_JSDC_2020
    # remove duplicates (on DEJ2000)
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_JSDC_2020 cmd='progress ; select !NULL_DEJ2000 ; sort dmsToDegrees(DEJ2000) ; uniq DEJ2000 ' out=$CAT_JSDC_2020
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_JSDC_2020 > $CAT_JSDC_2020.stats.log

# Total Rows: 483123
# +----------------+-------------+------------+--------------+-----------+--------+
# | column         | mean        | stdDev     | min          | max       | good   |
# +----------------+-------------+------------+--------------+-----------+--------+
# | RAJ2000_1      |             |            |              |           | 483123 |
# | DEJ2000_1      |             |            |              |           | 483123 |
# | pmRA_1         | -1.6305183  | 43.634697  | -3678.16     | 6516.79   | 483123 |
# | pmDE_1         | -7.954049   | 43.54662   | -5745.04     | 10308.08  | 483123 |
# | MAIN_ID_1      |             |            |              |           | 483123 |
# | SP_TYPE_1      |             |            |              |           | 483123 |
# | OTYPES_1       |             |            |              |           | 483123 |
# | GROUP_SIZE_5_1 | 0.09180271  | 0.4230334  | 0            | 4         | 483123 |
# | RAJ2000        |             |            |              |           | 483123 |
# | DEJ2000        |             |            |              |           | 483123 |
# | pmRA           | -1.6278806  | 43.63961   | -3678.16     | 6516.79   | 483123 |
# | pmDE           | -7.946728   | 43.536514  | -5745.04     | 10308.08  | 483123 |
# | MAIN_ID        |             |            |              |           | 476943 |
# | SP_TYPE        |             |            |              |           | 472962 |
# | OTYPES         |             |            |              |           | 476943 |
# | XM_SIMBAD_SEP  | 0.0626615   | 0.08856533 | 6.167478E-5  | 1.3919119 | 476943 |
# | GROUP_SIZE_3   | 0.08684331  | 0.4110372  | 0            | 4         | 483123 |
# | IRflag         | 1.0768082   | 1.5542074  | 0            | 7         | 440682 |
# | Lflux_med      | 1.4543884   | 38.825848  | 4.2477768E-4 | 15720.791 | 440597 |
# | e_Lflux_med    | 0.3882011   | 12.138079  | 3.0239256E-8 | 3939.0857 | 409311 |
# | Mflux_med      | 0.8507921   | 17.70605   | 2.456566E-4  | 7141.7    | 440599 |
# | e_Mflux_med    | 0.29954013  | 9.528872   | 8.423023E-10 | 3063.0676 | 409372 |
# | Nflux_med      | 0.27932897  | 10.235015  | 9.8775825E-5 | 3850.1    | 440590 |
# | e_Nflux_med    | 0.100201115 | 3.1783657  | 9.378568E-10 | 1230.4095 | 410933 |
# | GroupID        | 5143.1025   | 2969.273   | 1            | 10286     | 20682  |
# | GroupSize      | 3.5958805   | 0.92023975 | 2            | 12        | 20682  |
# | Separation     | 0.023348203 | 0.18783638 | 3.0000365E-6 | 2.9995322 | 483123 |
# +----------------+-------------+------------+--------------+-----------+--------+
fi


# Output
CAT_JSDC_FINAL="vobsascc_simbad_sptype-2020-final.vot"
if [ $CAT_JSDC_2020 -nt $CAT_JSDC_FINAL ]
then
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_JSDC_2020 cmd='progress ; delcols "*_1 GroupID GroupSize Separation" ' out=$CAT_JSDC_FINAL
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_JSDC_FINAL > $CAT_JSDC_FINAL.stats.log

# Total Rows: 483123
# +---------------+-------------+------------+--------------+-----------+--------+
# | column        | mean        | stdDev     | min          | max       | good   |
# +---------------+-------------+------------+--------------+-----------+--------+
# | RAJ2000       |             |            |              |           | 483123 |
# | DEJ2000       |             |            |              |           | 483123 |
# | pmRA          | -1.6278806  | 43.63961   | -3678.16     | 6516.79   | 483123 |
# | pmDE          | -7.946728   | 43.536514  | -5745.04     | 10308.08  | 483123 |
# | MAIN_ID       |             |            |              |           | 476943 |
# | SP_TYPE       |             |            |              |           | 472962 |
# | OTYPES        |             |            |              |           | 476943 |
# | XM_SIMBAD_SEP | 0.0626615   | 0.08856533 | 6.167478E-5  | 1.3919119 | 476943 |
# | GROUP_SIZE_3  | 0.08684331  | 0.4110372  | 0            | 4         | 483123 |
# | IRflag        | 1.0768082   | 1.5542074  | 0            | 7         | 440682 |
# | Lflux_med     | 1.4543884   | 38.825848  | 4.2477768E-4 | 15720.791 | 440597 |
# | e_Lflux_med   | 0.3882011   | 12.138079  | 3.0239256E-8 | 3939.0857 | 409311 |
# | Mflux_med     | 0.8507921   | 17.70605   | 2.456566E-4  | 7141.7    | 440599 |
# | e_Mflux_med   | 0.29954013  | 9.528872   | 8.423023E-10 | 3063.0676 | 409372 |
# | Nflux_med     | 0.27932897  | 10.235015  | 9.8775825E-5 | 3850.1    | 440590 |
# | e_Nflux_med   | 0.100201115 | 3.1783657  | 9.378568E-10 | 1230.4095 | 410933 |
# +---------------+-------------+------------+--------------+-----------+--------+
fi

# 4. Generate SearchCal config files:
CONFIG="vobsascc_simbad_sptype-2020.cfg"
xsltproc -o $CONFIG prepare_candidates/sclguiVOTableToTSV.xsl $CAT_JSDC_FINAL


echo "TODO: fix table header in $CONFIG"

