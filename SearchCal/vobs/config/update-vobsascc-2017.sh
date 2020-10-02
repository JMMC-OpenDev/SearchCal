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
    stilts ${STILTS_JAVA_OPTIONS} tmatch2 matcher='sky' params='0.1' values1='hmsToDegrees(RAJ2000) dmsToDegrees(DEJ2000)' values2='hmsToDegrees(RAJ2000) dmsToDegrees(DEJ2000)' find='best1' join='all1' in1=stilts/vobsascc_simbad_sptype-topcat.tst ifmt1="tst" in2=prepare_candidates/3_xmatch_ASCC_SIMBAD_MDFC_full_cols.fits out=$CAT_JSDC_2020
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_JSDC_2020 > $CAT_JSDC_2020.stats.log

# Total Rows: 494401
# +---------------+-------------+--------------+--------------+--------------+--------+
# | column        | mean        | stdDev       | min          | max          | good   |
# +---------------+-------------+--------------+--------------+--------------+--------+
# | RAJ2000_1     |             |              |              |              | 494401 |
# | DEJ2000_1     |             |              |              |              | 494401 |
# | pmRA_1        | -1.5503546  | 54.18501     | -4417.35     | 6773.35      | 494401 |
# | pmDE_1        | -9.241737   | 51.88438     | -5813.06     | 10308.08     | 494401 |
# | MAIN_ID_1     |             |              |              |              | 494401 |
# | SP_TYPE_1     |             |              |              |              | 494401 |
# | OTYPES_1      |             |              |              |              | 494401 |
# | GROUP_SIZE_5  | 0.093363486 | 0.42720315   | 0            | 4            | 494401 |
# | RAJ2000_2     |             |              |              |              | 494401 |
# | DEJ2000_2     |             |              |              |              | 494401 |
# | pmRA_2        | -1.5503546  | 54.18501     | -4417.35     | 6773.35      | 494401 |
# | pmDE_2        | -9.241737   | 51.88438     | -5813.06     | 10308.08     | 494401 |
# | MAIN_ID_2     |             |              |              |              | 492856 |
# | SP_TYPE_2     |             |              |              |              | 485644 |
# | OTYPES_2      |             |              |              |              | 492856 |
# | XM_SIMBAD_SEP | 0.05375451  | 0.071633495  | 0.0          | 0.99983424   | 492856 |
# | GROUP_SIZE_3  | 0.08616083  | 0.4095679    | 0            | 4            | 494401 |
# | IRflag        | 1.076797    | 1.555027     | 0            | 7            | 447309 |
# | Lflux_med     | 1.5240437   | 41.749847    | 1.6299028E-4 | 15720.791    | 447216 |
# | e_Lflux_med   | 0.4377065   | 16.345991    | 3.0239256E-8 | 5404.2817    | 415041 |
# | Mflux_med     | 0.8879836   | 19.193916    | 8.886453E-5  | 7141.7       | 447219 |
# | e_Mflux_med   | 0.32379213  | 10.734346    | 8.423023E-10 | 3063.0676    | 415097 |
# | Nflux_med     | 0.33234861  | 17.945023    | 9.8775825E-5 | 9278.1       | 447211 |
# | e_Nflux_med   | 0.11460468  | 4.081093     | 9.378568E-10 | 1230.4095    | 416788 |
# | Separation    | 2.778862E-4 | 1.9624607E-4 | 0.0          | 6.0000026E-4 | 494401 |
# +---------------+-------------+--------------+--------------+--------------+--------+
fi

# Output
CAT_JSDC_FINAL="vobsascc_simbad_sptype-2020-final.vot"
if [ $CAT_JSDC_2020 -nt $CAT_JSDC_FINAL ]
then
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_JSDC_2020 cmd='progress ; delcols "RAJ2000_1 DEJ2000_1 pmRA_1 pmDE_1 MAIN_ID_1 SP_TYPE_1 OTYPES_1 GROUP_SIZE_5 Separation" ; colmeta -name RAJ2000 RAJ2000_2 ; colmeta -name DEJ2000 DEJ2000_2 ; colmeta -name pmRA pmRA_2 ; colmeta -name pmDE pmDE_2 ; colmeta -name MAIN_ID MAIN_ID_2 ; colmeta -name SP_TYPE SP_TYPE_2 ; colmeta -name OTYPES OTYPES_2 ; select "!NULL_SP_TYPE" ' out=$CAT_JSDC_FINAL
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_JSDC_FINAL > $CAT_JSDC_FINAL.stats.log

# Total Rows: 485644
# +---------------+-------------+-------------+--------------+------------+--------+
# | column        | mean        | stdDev      | min          | max        | good   |
# +---------------+-------------+-------------+--------------+------------+--------+
# | RAJ2000       |             |             |              |            | 485644 |
# | DEJ2000       |             |             |              |            | 485644 |
# | pmRA          | -1.6011155  | 52.007328   | -4417.35     | 6773.35    | 485644 |
# | pmDE          | -9.036239   | 49.588734   | -5813.06     | 10308.08   | 485644 |
# | MAIN_ID       |             |             |              |            | 485644 |
# | SP_TYPE       |             |             |              |            | 485644 |
# | OTYPES        |             |             |              |            | 485644 |
# | XM_SIMBAD_SEP | 0.053475495 | 0.070962645 | 0.0          | 0.99983424 | 485644 |
# | GROUP_SIZE_3  | 0.06315326  | 0.35209316  | 0            | 4          | 485644 |
# | IRflag        | 1.0762293   | 1.5549337   | 0            | 7          | 446718 |
# | Lflux_med     | 1.5257183   | 41.777416   | 1.6299028E-4 | 15720.791  | 446625 |
# | e_Lflux_med   | 0.4380842   | 16.354708   | 3.0239256E-8 | 5404.2817  | 414598 |
# | Mflux_med     | 0.8889324   | 19.20657    | 8.886453E-5  | 7141.7     | 446628 |
# | e_Mflux_med   | 0.3240569   | 10.740054   | 8.423023E-10 | 3063.0676  | 414654 |
# | Nflux_med     | 0.33270058  | 17.956882   | 9.8775825E-5 | 9278.1     | 446620 |
# | e_Nflux_med   | 0.11469175  | 4.0832677   | 9.378568E-10 | 1230.4095  | 416343 |
# +---------------+-------------+-------------+--------------+------------+--------+
fi

# 4. Generate SearchCal config files:
CONFIG="vobsascc_simbad_sptype-2020.cfg"
xsltproc -o $CONFIG prepare_candidates/sclguiVOTableToTSV.xsl $CAT_JSDC_FINAL


echo "TODO: fix table header in $CONFIG"

