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

# Total Rows: 483743
# +----------------+-------------+------------+--------------+-----------+--------+
# | column         | mean        | stdDev     | min          | max       | good   |
# +----------------+-------------+------------+--------------+-----------+--------+
# | RAJ2000_1      |             |            |              |           | 483743 |
# | DEJ2000_1      |             |            |              |           | 483743 |
# | pmRA_1         | -1.634368   | 46.54296   | -3678.16     | 6773.35   | 483743 |
# | pmDE_1         | -8.050076   | 45.325455  | -5813.06     | 10308.08  | 483743 |
# | MAIN_ID_1      |             |            |              |           | 483743 |
# | SP_TYPE_1      |             |            |              |           | 483743 |
# | OTYPES_1       |             |            |              |           | 483743 |
# | GROUP_SIZE_5_1 | 0.08952274  | 0.41809794 | 0            | 4         | 483743 |
# | RAJ2000        |             |            |              |           | 483743 |
# | DEJ2000        |             |            |              |           | 483743 |
# | pmRA           | -1.6333953  | 46.54063   | -3678.16     | 6773.35   | 483743 |
# | pmDE           | -8.041777   | 45.320972  | -5813.06     | 10308.08  | 483743 |
# | MAIN_ID        |             |            |              |           | 477617 |
# | SP_TYPE        |             |            |              |           | 473861 |
# | OTYPES         |             |            |              |           | 477617 |
# | XM_SIMBAD_SEP  | 0.06297363  | 0.08924434 | 6.167478E-5  | 1.3919119 | 477617 |
# | GROUP_SIZE_3   | 0.08447254  | 0.40571716 | 0            | 4         | 483743 |
# | IRflag         | 1.0774522   | 1.5542612  | 0            | 7         | 441485 |
# | Lflux_med      | 1.4522443   | 38.79086   | 2.5337382E-4 | 15720.791 | 441394 |
# | e_Lflux_med    | 0.38784388  | 12.130808  | 3.0239256E-8 | 3939.0857 | 409795 |
# | Mflux_med      | 0.8494947   | 17.689856  | 1.2762281E-4 | 7141.7    | 441396 |
# | e_Mflux_med    | 0.29921743  | 9.523177   | 8.423023E-10 | 3063.0676 | 409856 |
# | Nflux_med      | 0.2798165   | 10.248473  | 9.8775825E-5 | 3850.1    | 441388 |
# | e_Nflux_med    | 0.10173585  | 3.3483303  | 9.378568E-10 | 1230.4095 | 411433 |
# | GroupID        | 5006.5405   | 2890.3855  | 1            | 10013     | 20137  |
# | GroupSize      | 3.6001391   | 0.92049545 | 2            | 12        | 20137  |
# | Separation     | 0.022653759 | 0.18509565 | 1.9999968E-6 | 2.9995322 | 483743 |
# +----------------+-------------+------------+--------------+-----------+--------+
fi


# Output
CAT_JSDC_FINAL="vobsascc_simbad_sptype-2020-final.vot"
if [ $CAT_JSDC_2020 -nt $CAT_JSDC_FINAL ]
then
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_JSDC_2020 cmd='progress ; delcols "*_1 GroupID GroupSize Separation" ' out=$CAT_JSDC_FINAL
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_JSDC_FINAL > $CAT_JSDC_FINAL.stats.log

# Total Rows: 483743
# +---------------+------------+------------+--------------+-----------+--------+
# | column        | mean       | stdDev     | min          | max       | good   |
# +---------------+------------+------------+--------------+-----------+--------+
# | RAJ2000       |            |            |              |           | 483743 |
# | DEJ2000       |            |            |              |           | 483743 |
# | pmRA          | -1.6333953 | 46.54063   | -3678.16     | 6773.35   | 483743 |
# | pmDE          | -8.041777  | 45.320972  | -5813.06     | 10308.08  | 483743 |
# | MAIN_ID       |            |            |              |           | 477617 |
# | SP_TYPE       |            |            |              |           | 473861 |
# | OTYPES        |            |            |              |           | 477617 |
# | XM_SIMBAD_SEP | 0.06297363 | 0.08924434 | 6.167478E-5  | 1.3919119 | 477617 |
# | GROUP_SIZE_3  | 0.08447254 | 0.40571716 | 0            | 4         | 483743 |
# | IRflag        | 1.0774522  | 1.5542612  | 0            | 7         | 441485 |
# | Lflux_med     | 1.4522443  | 38.79086   | 2.5337382E-4 | 15720.791 | 441394 |
# | e_Lflux_med   | 0.38784388 | 12.130808  | 3.0239256E-8 | 3939.0857 | 409795 |
# | Mflux_med     | 0.8494947  | 17.689856  | 1.2762281E-4 | 7141.7    | 441396 |
# | e_Mflux_med   | 0.29921743 | 9.523177   | 8.423023E-10 | 3063.0676 | 409856 |
# | Nflux_med     | 0.2798165  | 10.248473  | 9.8775825E-5 | 3850.1    | 441388 |
# | e_Nflux_med   | 0.10173585 | 3.3483303  | 9.378568E-10 | 1230.4095 | 411433 |
# +---------------+------------+------------+--------------+-----------+--------+
fi

# 4. Generate SearchCal config files:
CONFIG="vobsascc_simbad_sptype-2020.cfg"
xsltproc -o $CONFIG prepare_candidates/sclguiVOTableToTSV.xsl $CAT_JSDC_FINAL


echo "TODO: fix table header in $CONFIG"

