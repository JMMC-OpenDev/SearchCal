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
CAT_ASCC="ASCC_full.fits"
CAT_SIMBAD_ORIG="SIMBAD_full.vot"
CAT_MDFC="mdfc-v10.fits"


# 0. Download full ASCC (vizier):
if [ ! -e $CAT_ASCC ]
then
    # wget -O $CAT_ASCC_ORIG "http://cdsxmatch.u-strasbg.fr/QueryCat/QueryCat?catName=I%2F280B%2Fascc&mode=allsky&format=votable"
    # stilts ${STILTS_JAVA_OPTIONS} tcopy in=$CAT_ASCC_ORIG out=$CAT_ASCC
    
    wget -O $CAT_ASCC "http://vizier.u-strasbg.fr/viz-bin/asu-binfits?-source=I%2F280B%2Fascc&-out.max=unlimited&-oc.form=dec&-sort=&-c.eq=J2000&-out.add=_RAJ2000&-out.add=_DEJ2000&-out=pmRA&-out=pmDE&-out=Vmag&-out=TYC1&-out=TYC2&-out=TYC3&"

    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_ASCC > $CAT_ASCC.stats.log
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

# 1. Convert catalogs to FITS:
#    Convert SIMBAD to FITS:
CAT_SIMBAD="$CAT_SIMBAD_ORIG.fits"
if [ $CAT_SIMBAD_ORIG -nt $CAT_SIMBAD ]
then
    stilts ${STILTS_JAVA_OPTIONS} tcopy in=$CAT_SIMBAD_ORIG out=$CAT_SIMBAD
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_SIMBAD > $CAT_SIMBAD.stats.log

# Total Rows: 11508992
# +---------------+------------+-----------+-----------+-----------+----------+
# | column        | mean       | stdDev    | min       | max       | good     |
# +---------------+------------+-----------+-----------+-----------+----------+
# | main_id       |            |           |           |           | 11508992 |
# | ra            | 177.7619   | 99.7595   | 0.0       | 359.99997 | 11508992 |
# | dec           | -0.5741351 | 37.58958  | -89.88967 | 90.0      | 11508992 |
# | coo_err_maj   | 14.223434  | 1615.9175 | 0.0       | 636847.25 | 7308072  |
# | coo_err_min   | 13.598179  | 1615.8577 | 0.0       | 636847.25 | 7308072  |
# | coo_err_angle | 117.29588  | 44.351677 | 0.0       | 179.0     | 11508992 |
# | nbref         | 1.9910971  | 14.32646  | 0         | 15068     | 11508992 |
# | ra_sexa       |            |           |           |           | 11508992 |
# | dec_sexa      |            |           |           |           | 11508992 |
# | main_type     |            |           |           |           | 11508992 |
# | other_types   |            |           |           |           | 9478704  |
# | radvel        | 60301.453  | 84664.96  | -84723.35 | 296155.1  | 5372754  |
# | redshift      | 0.41141555 | 0.7764166 | -9.999    | 11.8      | 5373034  |
# | sp_type       |            |           |           |           | 841593   |
# | morph_type    |            |           |           |           | 137303   |
# | plx           | 2.2656035  | 4.215552  | -21.0     | 768.5004  | 4490809  |
# | pmra          | -1.4004905 | 37.682827 | -4800.0   | 6766.0    | 5147975  |
# | pmdec         | -6.5805216 | 35.697716 | -5817.86  | 10362.54  | 5147975  |
# | size_maj      | 6.861357   | 418.51642 | -0.017833 | 429050.0  | 1254465  |
# | size_min      | 6.4552627  | 418.15164 | -0.017833 | 429050.0  | 1254443  |
# | size_angle    | 2399.6418  | 8381.409  | -267      | 32767     | 1252988  |
# | B             | 14.947774  | 4.5783224 | -1.46     | 45.38     | 4515917  |
# | V             | 14.648483  | 4.7224307 | -1.46     | 38.76     | 4835967  |
# | R             | 17.56859   | 4.6411934 | -1.46     | 50.0      | 1356242  |
# | J             | 12.606695  | 4.4891043 | -3.0      | 35.847    | 5024696  |
# | H             | 12.074506  | 4.485383  | -3.73     | 43.8      | 4979024  |
# | K             | 11.996656  | 4.504595  | -4.05     | 33.274    | 5057408  |
# | u             | 21.509323  | 2.164739  | 0.0       | 33.273    | 2333027  |
# | g             | 19.22381   | 3.3147292 | 0.0       | 33.486    | 2851898  |
# | r             | 18.370024  | 3.245424  | 0.0       | 32.122    | 2847579  |
# | i             | 18.02315   | 3.3632078 | 0.0       | 33.58     | 2877020  |
# | z             | 18.87645   | 2.135126  | 0.0       | 36.46     | 2490270  |
# +---------------+------------+-----------+-----------+-----------+----------+
fi


# ASCC: filter columns: (ra/dec J2000 corrected with pm) - from epoch 1991.25
CAT_ASCC_MIN="1_min_$CAT_ASCC"
if [ $CAT_ASCC -nt $CAT_ASCC_MIN ]
then
    # convert ASCC coords to epoch 1991.25:
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_ASCC cmd='progress ; keepcols "_RAJ2000 _DEJ2000 pmRA pmDE Vmag TYC1" ; addcol ra_a_91 "_RAJ2000 + (1991.25 - 2000.0) * (0.001 * pmRA / 3600.0)" ; addcol de_a_91 "_DEJ2000 + (1991.25 - 2000.0) * (0.001 * pmDE / 3600.0)" ' out=$CAT_ASCC_MIN
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_ASCC_MIN > $CAT_ASCC_MIN.stats.log
    
# Total Rows: 2501313
# +----------+------------+-----------+--------------+-----------+---------+
# | column   | mean       | stdDev    | min          | max       | good    |
# +----------+------------+-----------+--------------+-----------+---------+
# | _RAJ2000 | 188.87096  | 100.47709 | 3.4094515E-4 | 359.99988 | 2501313 |
# | _DEJ2000 | -3.317228  | 41.249565 | -89.88966    | 89.911354 | 2501313 |
# | pmRA     | -1.7540642 | 32.2306   | -4417.35     | 6773.35   | 2501313 |
# | pmDE     | -6.0875754 | 30.390753 | -5813.7      | 10308.08  | 2501313 |
# | Vmag     | 11.069625  | 1.1319602 | -1.121       | 16.746    | 2500626 |
# | TYC1     | 4933.0464  | 2840.6257 | 0            | 9537      | 2501313 |
# | ra_a_91  | 188.87096  | 100.47709 | 2.829278E-4  | 359.9999  | 2501313 |
# | de_a_91  | -3.3172133 | 41.24957  | -89.88964    | 89.91136  | 2501313 |
# +----------+------------+-----------+--------------+-----------+---------+
fi


# ASCC: internal match within 3 as to identify inner groups and keep all (V defined)
CAT_ASCC_MIN_GRP="1_min_Grp_$CAT_ASCC"
if [ $CAT_ASCC_MIN -nt $CAT_ASCC_MIN_GRP ]
then
    stilts ${STILTS_JAVA_OPTIONS} tmatch1 matcher='sky' params='3.0' values='ra_a_91 de_a_91' action='identify' in=$CAT_ASCC_MIN out=$CAT_ASCC_MIN_GRP
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_ASCC_MIN_GRP cmd='progress ; colmeta -name ASCC_GROUP_SIZE GroupSize ; select !NULL_Vmag||ASCC_GROUP_SIZE>0 ; delcols "GroupID TYC1" ' out=$CAT_ASCC_MIN_GRP
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_ASCC_MIN_GRP > $CAT_ASCC_MIN_GRP.stats.log

# Total Rows: 2500765
# +-----------------+------------+------------+--------------+-----------+---------+
# | column          | mean       | stdDev     | min          | max       | good    |
# +-----------------+------------+------------+--------------+-----------+---------+
# | _RAJ2000        | 188.873    | 100.47335  | 3.4094515E-4 | 359.99988 | 2500765 |
# | _DEJ2000        | -3.3170435 | 41.251923  | -89.88966    | 89.911354 | 2500765 |
# | pmRA            | -1.7544895 | 32.22942   | -4417.35     | 6773.35   | 2500765 |
# | pmDE            | -6.0867248 | 30.385548  | -5813.7      | 10308.08  | 2500765 |
# | Vmag            | 11.069625  | 1.1319602  | -1.121       | 16.746    | 2500626 |
# | ra_a_91         | 188.873    | 100.47335  | 2.829278E-4  | 359.9999  | 2500765 |
# | de_a_91         | -3.3170285 | 41.251926  | -89.88964    | 89.91136  | 2500765 |
# | ASCC_GROUP_SIZE | 2.0281894  | 0.17002448 | 2            | 4         | 31714   |
# +-----------------+------------+------------+--------------+-----------+---------+
fi


# Filter SIMBAD objects:
CAT_SIMBAD_OBJ="1_obj_$CAT_SIMBAD"
if [ $CAT_SIMBAD -nt $CAT_SIMBAD_OBJ ]
then
    # filter SIMBAD main_type (stars only) to exclude Planet, Galaxy ...
    CAT_SIMBAD_OBJ_STAR="1_obj_star_$CAT_SIMBAD"
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_SIMBAD cmd='progress; select !NULL_sp_type||(!NULL_main_type&&(main_type.contains(\"*\")||main_type.equals(\"Star\")))||(!NULL_other_types&&other_types.contains(\"*\")) ' out=$CAT_SIMBAD_OBJ_STAR
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_SIMBAD_OBJ_STAR > $CAT_SIMBAD_OBJ_STAR.stats.log

# Total Rows: 6248650
# +---------------+-------------+-----------+-----------+-----------+---------+
# | column        | mean        | stdDev    | min       | max       | good    |
# +---------------+-------------+-----------+-----------+-----------+---------+
# | main_id       |             |           |           |           | 6248650 |
# | ra            | 185.20473   | 101.4056  | 0.0       | 359.99997 | 6248650 |
# | dec           | -6.2443633  | 40.803207 | -89.88967 | 89.851585 | 6248650 |
# | coo_err_maj   | 0.035816494 | 3.4640882 | 0.0       | 3380.0    | 5647384 |
# | coo_err_min   | 0.03066139  | 2.4271805 | 0.0       | 1930.0    | 5647384 |
# | coo_err_angle | 96.2182     | 27.821684 | 0.0       | 179.0     | 6248650 |
# | nbref         | 2.2576418   | 13.421349 | 0         | 5365      | 6248650 |
# | ra_sexa       |             |           |           |           | 6248650 |
# | dec_sexa      |             |           |           |           | 6248650 |
# | main_type     |             |           |           |           | 6248650 |
# | other_types   |             |           |           |           | 5494259 |
# | radvel        | 19426.246   | 59548.836 | -58663.2  | 293760.8  | 3002173 |
# | redshift      | 0.14789309  | 0.5281785 | -9.99898  | 8.92      | 3002176 |
# | sp_type       |             |           |           |           | 841593  |
# | morph_type    |             |           |           |           | 41527   |
# | plx           | 2.3035212   | 4.1802936 | -21.0     | 768.5004  | 4375306 |
# | pmra          | -1.4180012  | 37.77446  | -4800.0   | 6766.0    | 4954418 |
# | pmdec         | -6.7730966  | 35.822327 | -5817.86  | 10362.54  | 4954418 |
# | size_maj      | 0.4329702   | 11.195558 | 0.0       | 3438.0    | 203347  |
# | size_min      | 0.2812631   | 8.158857  | 0.0       | 3438.0    | 203344  |
# | size_angle    | 816.66766   | 4821.939  | -89       | 32767     | 203394  |
# | B             | 13.3428135  | 2.9428766 | -1.46     | 45.38     | 3677310 |
# | V             | 13.566709   | 3.6461103 | -1.46     | 31.92     | 4307574 |
# | R             | 14.336972   | 2.9117358 | -1.46     | 50.0      | 691035  |
# | J             | 11.173995   | 2.643588  | -3.0      | 29.25     | 4240911 |
# | H             | 10.687006   | 2.635389  | -3.73     | 29.164    | 4226545 |
# | K             | 10.5449295  | 2.6733224 | -4.05     | 26.4      | 4234458 |
# | u             | 20.260662   | 2.1689684 | 0.0       | 31.431    | 619155  |
# | g             | 16.63318    | 3.659106  | 0.0       | 30.7      | 1034907 |
# | r             | 15.962293   | 3.757046  | 0.003     | 31.197    | 1038171 |
# | i             | 15.548708   | 3.812424  | 0.004     | 30.139    | 1043130 |
# | z             | 17.934155   | 2.087205  | 0.0       | 31.741    | 639296  |
# +---------------+-------------+-----------+-----------+-----------+---------+

    # filter on V > 16 (not in TYCHO2)
    MAG_FILTER="V<=16.0"
    
    CAT_SIMBAD_OBJ_Vgt16="1_obj_star_Vgt16_$CAT_SIMBAD"
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_SIMBAD_OBJ_STAR cmd="progress; select !(${MAG_FILTER}) " out=$CAT_SIMBAD_OBJ_Vgt16
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_SIMBAD_OBJ_Vgt16 > $CAT_SIMBAD_OBJ_Vgt16.stats.log

# Total Rows: 3128539
# +---------------+-------------+------------+-----------+-----------+---------+
# | column        | mean        | stdDev     | min       | max       | good    |
# +---------------+-------------+------------+-----------+-----------+---------+
# | main_id       |             |            |           |           | 3128539 |
# | ra            | 182.8842    | 102.11178  | 0.0       | 359.99997 | 3128539 |
# | dec           | -7.1725016  | 40.31073   | -89.86853 | 89.851585 | 3128539 |
# | coo_err_maj   | 0.06578386  | 4.98497    | 0.0       | 3380.0    | 2593416 |
# | coo_err_min   | 0.055437274 | 3.3999896  | 0.0       | 1930.0    | 2593416 |
# | coo_err_angle | 101.84102   | 35.12569   | 0.0       | 179.0     | 3128539 |
# | nbref         | 2.658308    | 11.195468  | 0         | 4839      | 3128539 |
# | ra_sexa       |             |            |           |           | 3128539 |
# | dec_sexa      |             |            |           |           | 3128539 |
# | main_type     |             |            |           |           | 3128539 |
# | other_types   |             |            |           |           | 2405057 |
# | radvel        | 63635.203   | 93862.82   | -58663.2  | 293760.8  | 915596  |
# | redshift      | 0.48465452  | 0.86680955 | -9.99898  | 8.92      | 915599  |
# | sp_type       |             |            |           |           | 277274  |
# | morph_type    |             |            |           |           | 39851   |
# | plx           | 2.2415833   | 5.1742206  | -21.0     | 448.0     | 1475767 |
# | pmra          | -1.1622353  | 48.39557   | -4800.0   | 6491.47   | 1920937 |
# | pmdec         | -8.708685   | 45.720493  | -5709.22  | 3314.4    | 1920937 |
# | size_maj      | 0.42922124  | 11.402792  | 0.0       | 3438.0    | 195869  |
# | size_min      | 0.27591413  | 8.308442   | 0.0       | 3438.0    | 195867  |
# | size_angle    | 807.0493    | 4796.261   | -89       | 32767     | 195885  |
# | B             | 18.200153   | 2.3802269  | 3.0       | 45.38     | 741074  |
# | V             | 18.894974   | 1.7286395  | 16.001    | 31.92     | 1187463 |
# | R             | 16.11899    | 2.5267396  | 0.0       | 50.0      | 381341  |
# | J             | 13.709038   | 2.5193992  | 1.148     | 29.25     | 1398844 |
# | H             | 13.009701   | 2.657156   | 0.0       | 29.164    | 1385135 |
# | K             | 12.777522   | 2.8198137  | -0.322    | 26.4      | 1395326 |
# | u             | 20.396566   | 2.0413544  | 0.0       | 31.224    | 600435  |
# | g             | 19.186985   | 1.9192594  | 0.0       | 30.7      | 637685  |
# | r             | 18.598316   | 1.951937   | 0.004     | 31.197    | 639129  |
# | i             | 18.2592     | 1.9835283  | 0.004     | 30.139    | 634964  |
# | z             | 18.090551   | 1.9308994  | 0.0       | 30.878    | 616462  |
# +---------------+-------------+------------+-----------+-----------+---------+

    # filter on V <= 16 (max in TYCHO2)
    # convert SIMBAD coords to epoch 1991.25:
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_SIMBAD_OBJ_STAR cmd="progress; select ${MAG_FILTER} ; addcol ra_s_91 'ra  + ((NULL_pmra ) ? 0.0 : (1991.25 - 2000.0) * (0.001 * pmra  / 3600.0))' ; addcol de_s_91 'dec + ((NULL_pmdec) ? 0.0 : (1991.25 - 2000.0) * (0.001 * pmdec / 3600.0))' " out=$CAT_SIMBAD_OBJ
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_SIMBAD_OBJ > $CAT_SIMBAD_OBJ.stats.log

# Total Rows: 3120111
# +---------------+---------------+-------------+-------------+-----------+---------+
# | column        | mean          | stdDev      | min         | max       | good    |
# +---------------+---------------+-------------+-------------+-----------+---------+
# | main_id       |               |             |             |           | 3120111 |
# | ra            | 187.53151     | 100.63884   | 0.0         | 359.9999  | 3120111 |
# | dec           | -5.313718     | 41.270164   | -89.88967   | 89.83233  | 3120111 |
# | coo_err_maj   | 0.010368337   | 1.0422641   | 0.0         | 180.001   | 3053968 |
# | coo_err_min   | 0.009621817   | 1.0374907   | 0.0         | 180.0     | 3053968 |
# | coo_err_angle | 90.58019      | 15.797075   | 0.0         | 179.0     | 3120111 |
# | nbref         | 1.8558933     | 15.3216305  | 0           | 5365      | 3120111 |
# | ra_sexa       |               |             |             |           | 3120111 |
# | dec_sexa      |               |             |             |           | 3120111 |
# | main_type     |               |             |             |           | 3120111 |
# | other_types   |               |             |             |           | 3089202 |
# | radvel        | 27.227926     | 1483.5297   | -1761.35    | 264852.44 | 2086577 |
# | redshift      | 1.20713376E-4 | 0.009311298 | -0.005858   | 3.02      | 2086577 |
# | sp_type       |               |             |             |           | 564319  |
# | morph_type    |               |             |             |           | 1676    |
# | plx           | 2.3350456     | 3.5692704   | -0.6992     | 768.5004  | 2899539 |
# | pmra          | -1.5799638    | 29.108122   | -4339.89    | 6766.0    | 3033481 |
# | pmdec         | -5.5473948    | 27.717447   | -5817.86    | 10362.54  | 3033481 |
# | size_maj      | 0.53116584    | 1.6329207   | 0.0         | 60.0      | 7478    |
# | size_min      | 0.42138413    | 1.4209796   | 0.0         | 60.0      | 7477    |
# | size_angle    | 1067.5784     | 5443.2124   | -64         | 32767     | 7509    |
# | B             | 12.116874     | 1.3995379   | -1.46       | 31.5      | 2936236 |
# | V             | 11.538857     | 1.5163372   | -1.46       | 16.0      | 3120111 |
# | R             | 12.142687     | 1.5268431   | -1.46       | 25.55     | 309694  |
# | J             | 9.926266      | 1.6075664   | -3.0        | 18.651    | 2842067 |
# | H             | 9.554735      | 1.7254449   | -3.73       | 18.23     | 2841410 |
# | K             | 9.447694      | 1.7599914   | -4.05       | 18.22     | 2839132 |
# | u             | 15.901589     | 1.531018    | 1.25        | 31.431    | 18720   |
# | g             | 12.533403     | 1.3006506   | 0.008       | 25.118    | 397222  |
# | r             | 11.740285     | 1.2908778   | 0.003       | 27.5      | 399042  |
# | i             | 11.332129     | 1.3476776   | 0.004       | 27.98     | 408166  |
# | z             | 13.7118435    | 1.680482    | 0.004       | 31.741    | 22834   |
# | ra_s_91       | 187.53152     | 100.63884   | 8.506944E-6 | 359.9999  | 3120111 |
# | de_s_91       | -5.313705     | 41.27017    | -89.889656  | 89.83232  | 3120111 |
# +---------------+---------------+-------------+-------------+-----------+---------+
fi


# SIMBAD: internal match within 3 as to identify inner groups and filter columns:
# (main_id, ra/dec 1991) only to identify groups
CAT_SIMBAD_MIN="1_min_$CAT_SIMBAD_OBJ"
if [ $CAT_SIMBAD_OBJ -nt $CAT_SIMBAD_MIN ]
then
    CAT_SIMBAD_MIN_0="1_min_0_$CAT_SIMBAD_OBJ"

    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_SIMBAD_OBJ cmd='progress ; keepcols "ra_s_91 de_s_91 main_id" ' out=$CAT_SIMBAD_MIN_0
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_SIMBAD_MIN_0 > $CAT_SIMBAD_MIN_0.stats.log

# Total Rows: 3120111
# +---------+-----------+-----------+-------------+----------+---------+
# | column  | mean      | stdDev    | min         | max      | good    |
# +---------+-----------+-----------+-------------+----------+---------+
# | ra_s_91 | 187.53152 | 100.63884 | 8.506944E-6 | 359.9999 | 3120111 |
# | de_s_91 | -5.313705 | 41.27017  | -89.889656  | 89.83232 | 3120111 |
# | main_id |           |           |             |          | 3120111 |
# +---------+-----------+-----------+-------------+----------+---------+

    # identify inner groups in SIMBAD:
    stilts ${STILTS_JAVA_OPTIONS} tmatch1 matcher='sky' params='3.0' values='ra_s_91 de_s_91' action='identify' in=$CAT_SIMBAD_MIN_0 out=$CAT_SIMBAD_MIN
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_SIMBAD_MIN cmd='progress ; colmeta -name SIMBAD_GROUP_SIZE GroupSize ; delcols "GroupID" ' out=$CAT_SIMBAD_MIN
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_SIMBAD_MIN > $CAT_SIMBAD_MIN.stats.log

# Total Rows: 3120111
# +-------------------+-----------+-----------+-------------+----------+---------+
# | column            | mean      | stdDev    | min         | max      | good    |
# +-------------------+-----------+-----------+-------------+----------+---------+
# | ra_s_91           | 187.53152 | 100.63884 | 8.506944E-6 | 359.9999 | 3120111 |
# | de_s_91           | -5.313705 | 41.27017  | -89.889656  | 89.83232 | 3120111 |
# | main_id           |           |           |             |          | 3120111 |
# | SIMBAD_GROUP_SIZE | 4.2767262 | 24.75791  | 2           | 346      | 71652   |
# +-------------------+-----------+-----------+-------------+----------+---------+
fi


# SIMBAD: column subset for candidates
CAT_SIMBAD_SUB="1_sub_$CAT_SIMBAD_OBJ"
if [ $CAT_SIMBAD_OBJ -nt $CAT_SIMBAD_SUB ]
then
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_SIMBAD_OBJ cmd='progress ; keepcols "ra_s_91 de_s_91 main_id sp_type main_type other_types V" ' out=$CAT_SIMBAD_SUB
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_SIMBAD_SUB > $CAT_SIMBAD_SUB.stats.log

# Total Rows: 3120111
# +-------------+-----------+-----------+-------------+----------+---------+
# | column      | mean      | stdDev    | min         | max      | good    |
# +-------------+-----------+-----------+-------------+----------+---------+
# | ra_s_91     | 187.53152 | 100.63884 | 8.506944E-6 | 359.9999 | 3120111 |
# | de_s_91     | -5.313705 | 41.27017  | -89.889656  | 89.83232 | 3120111 |
# | main_id     |           |           |             |          | 3120111 |
# | sp_type     |           |           |             |          | 564319  |
# | main_type   |           |           |             |          | 3120111 |
# | other_types |           |           |             |          | 3089202 |
# | V           | 11.538857 | 1.5163372 | -1.46       | 16.0     | 3120111 |
# +-------------+-----------+-----------+-------------+----------+---------+
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


# 2. Crossmatch ASCC / SIMBAD within 3 as to identify both groups
CAT_ASCC_SIMBAD_GRP="2_Grp_xmatch_ASCC_SIMBAD_full.fits"
if [ $CAT_ASCC_MIN_GRP -nt $CAT_ASCC_SIMBAD_GRP ] || [ $CAT_SIMBAD_MIN -nt $CAT_ASCC_SIMBAD_GRP ]
then
    CAT_ASCC_SIMBAD_GRP_0="2_Grp_xmatch_ASCC_SIMBAD_full_0.fits"

    # use best1 to get groups in SIMBAD for every ASCC source at epoch 1991.25
    stilts ${STILTS_JAVA_OPTIONS} tmatch2 matcher='sky' params='3.0' values1='ra_a_91 de_a_91' values2='ra_s_91 de_s_91' find='best1' join='all1' in1=$CAT_ASCC_MIN_GRP in2=$CAT_SIMBAD_MIN out=$CAT_ASCC_SIMBAD_GRP_0
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_ASCC_SIMBAD_GRP_0 > ${CAT_ASCC_SIMBAD_GRP_0}.stats.log

# Total Rows: 2500765
# +-------------------+------------+------------+--------------+-----------+---------+
# | column            | mean       | stdDev     | min          | max       | good    |
# +-------------------+------------+------------+--------------+-----------+---------+
# | _RAJ2000          | 188.873    | 100.47335  | 3.4094515E-4 | 359.99988 | 2500765 |
# | _DEJ2000          | -3.3170435 | 41.251923  | -89.88966    | 89.911354 | 2500765 |
# | pmRA              | -1.7544895 | 32.22942   | -4417.35     | 6773.35   | 2500765 |
# | pmDE              | -6.0867248 | 30.385548  | -5813.7      | 10308.08  | 2500765 |
# | Vmag              | 11.069625  | 1.1319602  | -1.121       | 16.746    | 2500626 |
# | ra_a_91           | 188.873    | 100.47335  | 2.829278E-4  | 359.9999  | 2500765 |
# | de_a_91           | -3.3170285 | 41.251926  | -89.88964    | 89.91136  | 2500765 |
# | ASCC_GROUP_SIZE   | 2.0281894  | 0.17002448 | 2            | 4         | 31714   |
# | ra_s_91           | 188.4883   | 100.36739  | 2.8021954E-4 | 359.9999  | 2456512 |
# | de_s_91           | -3.5633376 | 41.398327  | -89.889656   | 89.83232  | 2456512 |
# | main_id           |            |            |              |           | 2456512 |
# | SIMBAD_GROUP_SIZE | 2.1596286  | 0.8564379  | 2            | 147       | 39310   |
# | GroupID           | 3419.4148  | 1973.3864  | 1            | 6836      | 13709   |
# | GroupSize         | 2.008097   | 0.08961759 | 2            | 3         | 13709   |
# | Separation        | 0.10418369 | 0.1365418  | 4.721107E-6  | 2.9999418 | 2456512 |
# +-------------------+------------+------------+--------------+-----------+---------+

    # Only consider groups between ASCC x SIMBAD within 3as:
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_ASCC_SIMBAD_GRP_0 cmd='progress ; delcols "ra_s_91 de_s_91 main_id SIMBAD_GROUP_SIZE GroupID Separation" ; colmeta -name SIMBAD_GROUP_SIZE GroupSize ' out=$CAT_ASCC_SIMBAD_GRP
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_ASCC_SIMBAD_GRP > $CAT_ASCC_SIMBAD_GRP.stats.log

# Total Rows: 2500765
# +-------------------+------------+------------+--------------+-----------+---------+
# | column            | mean       | stdDev     | min          | max       | good    |
# +-------------------+------------+------------+--------------+-----------+---------+
# | _RAJ2000          | 188.873    | 100.47335  | 3.4094515E-4 | 359.99988 | 2500765 |
# | _DEJ2000          | -3.3170435 | 41.251923  | -89.88966    | 89.911354 | 2500765 |
# | pmRA              | -1.7544895 | 32.22942   | -4417.35     | 6773.35   | 2500765 |
# | pmDE              | -6.0867248 | 30.385548  | -5813.7      | 10308.08  | 2500765 |
# | Vmag              | 11.069625  | 1.1319602  | -1.121       | 16.746    | 2500626 |
# | ra_a_91           | 188.873    | 100.47335  | 2.829278E-4  | 359.9999  | 2500765 |
# | de_a_91           | -3.3170285 | 41.251926  | -89.88964    | 89.91136  | 2500765 |
# | ASCC_GROUP_SIZE   | 2.0281894  | 0.17002448 | 2            | 4         | 31714   |
# | SIMBAD_GROUP_SIZE | 2.008097   | 0.08961759 | 2            | 3         | 13709   |
# +-------------------+------------+------------+--------------+-----------+---------+
fi


# 2. Crossmatch ASCC / SIMBAD within 1 as to get both DATA
CAT_ASCC_SIMBAD_XMATCH="2_xmatch_ASCC_SIMBAD_full.fits"
if [ $CAT_ASCC_SIMBAD_GRP -nt $CAT_ASCC_SIMBAD_XMATCH ] || [ $CAT_SIMBAD_SUB -nt $CAT_ASCC_SIMBAD_XMATCH ]
then
    # use best to have the best ASCC x SIMBAD object: 
    # Use V-Vmag in matcher (up to 1.0 mag), note Separation becomes sqrt(dist^2, delta_mag^2)
    stilts ${STILTS_JAVA_OPTIONS} tmatch2 matcher='sky+1d' params='1.0 1.0' values1='ra_a_91 de_a_91 Vmag' values2='ra_s_91 de_s_91 V' find='best' join='all1' in1=$CAT_ASCC_SIMBAD_GRP in2=$CAT_SIMBAD_SUB out=$CAT_ASCC_SIMBAD_XMATCH 
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_ASCC_SIMBAD_XMATCH cmd='progress ; delcols "ra_s_91 de_s_91" ; colmeta -name XM_SIMBAD_SEP -ucd XM_SIMBAD_SEP Separation ' out=$CAT_ASCC_SIMBAD_XMATCH
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_ASCC_SIMBAD_XMATCH > $CAT_ASCC_SIMBAD_XMATCH.stats.log

# Total Rows: 2500765
# +-------------------+------------+------------+--------------+-----------+---------+
# | column            | mean       | stdDev     | min          | max       | good    |
# +-------------------+------------+------------+--------------+-----------+---------+
# | _RAJ2000          | 188.873    | 100.47335  | 3.4094515E-4 | 359.99988 | 2500765 |
# | _DEJ2000          | -3.3170435 | 41.251923  | -89.88966    | 89.911354 | 2500765 |
# | pmRA              | -1.7544895 | 32.22942   | -4417.35     | 6773.35   | 2500765 |
# | pmDE              | -6.0867248 | 30.385548  | -5813.7      | 10308.08  | 2500765 |
# | Vmag              | 11.069625  | 1.1319602  | -1.121       | 16.746    | 2500626 |
# | ra_a_91           | 188.873    | 100.47335  | 2.829278E-4  | 359.9999  | 2500765 |
# | de_a_91           | -3.3170285 | 41.251926  | -89.88964    | 89.91136  | 2500765 |
# | ASCC_GROUP_SIZE   | 2.0281894  | 0.17002448 | 2            | 4         | 31714   |
# | SIMBAD_GROUP_SIZE | 2.008097   | 0.08961759 | 2            | 3         | 13709   |
# | main_id           |            |            |              |           | 2440900 |
# | sp_type           |            |            |              |           | 488455  |
# | main_type         |            |            |              |           | 2440900 |
# | other_types       |            |            |              |           | 2439454 |
# | V                 | 11.063682  | 1.1081516  | -1.46        | 15.87     | 2440900 |
# | XM_SIMBAD_SEP     | 0.11578731 | 0.11118041 | 4.0487063E-5 | 1.3919119 | 2440900 |
# +-------------------+------------+------------+--------------+-----------+---------+
fi


# 2. Crossmatch ASCC / SIMBAD / MDFC (SIMBAD main_id):
CAT_ASCC_SIMBAD_MDFC_XMATCH="2_xmatch_ASCC_SIMBAD_MDFC_full.fits"
if [ $CAT_ASCC_SIMBAD_XMATCH -nt $CAT_ASCC_SIMBAD_MDFC_XMATCH ] || [ $CAT_MDFC_MIN -nt $CAT_ASCC_SIMBAD_MDFC_XMATCH ]
then
    stilts ${STILTS_JAVA_OPTIONS} tmatch2 matcher='exact' values1='main_id' values2='Name' join='all1' in1=$CAT_ASCC_SIMBAD_XMATCH in2=$CAT_MDFC_MIN out=$CAT_ASCC_SIMBAD_MDFC_XMATCH
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_ASCC_SIMBAD_MDFC_XMATCH cmd='progress ; delcols "Name" ' out=$CAT_ASCC_SIMBAD_MDFC_XMATCH
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_ASCC_SIMBAD_MDFC_XMATCH > $CAT_ASCC_SIMBAD_MDFC_XMATCH.stats.log

# Total Rows: 2500765
# +-------------------+------------+------------+--------------+-----------+---------+
# | column            | mean       | stdDev     | min          | max       | good    |
# +-------------------+------------+------------+--------------+-----------+---------+
# | _RAJ2000          | 188.873    | 100.47335  | 3.4094515E-4 | 359.99988 | 2500765 |
# | _DEJ2000          | -3.3170435 | 41.251923  | -89.88966    | 89.911354 | 2500765 |
# | pmRA              | -1.7544895 | 32.22942   | -4417.35     | 6773.35   | 2500765 |
# | pmDE              | -6.0867248 | 30.385548  | -5813.7      | 10308.08  | 2500765 |
# | Vmag              | 11.069625  | 1.1319602  | -1.121       | 16.746    | 2500626 |
# | ra_a_91           | 188.873    | 100.47335  | 2.829278E-4  | 359.9999  | 2500765 |
# | de_a_91           | -3.3170285 | 41.251926  | -89.88964    | 89.91136  | 2500765 |
# | ASCC_GROUP_SIZE   | 2.0281894  | 0.17002448 | 2            | 4         | 31714   |
# | SIMBAD_GROUP_SIZE | 2.008097   | 0.08961759 | 2            | 3         | 13709   |
# | main_id           |            |            |              |           | 2440900 |
# | sp_type           |            |            |              |           | 488455  |
# | main_type         |            |            |              |           | 2440900 |
# | other_types       |            |            |              |           | 2439454 |
# | V                 | 11.063682  | 1.1081516  | -1.46        | 15.87     | 2440900 |
# | XM_SIMBAD_SEP     | 0.11578731 | 0.11118041 | 4.0487063E-5 | 1.3919119 | 2440900 |
# | IRflag            | 1.0801827  | 1.5521469  | 0            | 7         | 456246  |
# | Lflux_med         | 1.4305483  | 38.167324  | 2.5337382E-4 | 15720.791 | 456152  |
# | e_Lflux_med       | 0.38149986 | 11.931825  | 3.0239256E-8 | 3939.0857 | 423985  |
# | Mflux_med         | 0.836951   | 17.407589  | 1.2762281E-4 | 7141.7    | 456154  |
# | e_Mflux_med       | 0.29414588 | 9.36655    | 8.423023E-10 | 3063.0676 | 424045  |
# | Nflux_med         | 0.2753638  | 10.084012  | 9.8775825E-5 | 3850.1    | 456147  |
# | e_Nflux_med       | 0.10006758 | 3.2933166  | 9.378568E-10 | 1230.4095 | 425646  |
# +-------------------+------------+------------+--------------+-----------+---------+
fi


# 3. Prepare candidates: columns, sort, then filters:
FULL_CANDIDATES="3_xmatch_ASCC_SIMBAD_MDFC_full_cols.fits"
if [ $CAT_ASCC_SIMBAD_MDFC_XMATCH -nt $FULL_CANDIDATES ]
then
    # note: merge (main_type,other_types) as other_types can be NULL:
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_ASCC_SIMBAD_MDFC_XMATCH cmd='progress ; addcol -after sp_type -ucd OBJ_TYPES OTYPES NULL_main_type?\"\":replaceAll(concat(\",\",main_type,\",\",replaceAll(other_types,\"[|]\",\",\"),\",\"),\"\ \",\"\") ; delcols "main_type other_types Vmag V" ; addcol -before _RAJ2000 -ucd POS_EQ_RA_MAIN RAJ2000 degreesToHms(_RAJ2000,6) ; addcol -after RAJ2000 -ucd POS_EQ_DEC_MAIN DEJ2000 degreesToDms(_DEJ2000,6) ; colmeta -ucd POS_EQ_PMRA pmRA ; colmeta -ucd POS_EQ_PMDEC pmDE ; colmeta -name MAIN_ID -ucd ID_MAIN main_id ; colmeta -name SP_TYPE -ucd SPECT_TYPE_MK sp_type ; addcol -after XM_SIMBAD_SEP -ucd GROUP_SIZE GROUP_SIZE_3 max((!NULL_ASCC_GROUP_SIZE?ASCC_GROUP_SIZE:0),(!NULL_SIMBAD_GROUP_SIZE?SIMBAD_GROUP_SIZE:0)) ; sort _DEJ2000 ; delcols "_RAJ2000 _DEJ2000 ASCC_GROUP_SIZE SIMBAD_GROUP_SIZE " ' out=$FULL_CANDIDATES
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$FULL_CANDIDATES > $FULL_CANDIDATES.stats.log

# Total Rows: 2500765
# +---------------+-------------+------------+--------------+-----------+---------+
# | column        | mean        | stdDev     | min          | max       | good    |
# +---------------+-------------+------------+--------------+-----------+---------+
# | RAJ2000       |             |            |              |           | 2500765 |
# | DEJ2000       |             |            |              |           | 2500765 |
# | pmRA          | -1.7544895  | 32.22942   | -4417.35     | 6773.35   | 2500765 |
# | pmDE          | -6.0867248  | 30.385548  | -5813.7      | 10308.08  | 2500765 |
# | ra_a_91       | 188.873     | 100.47335  | 2.829278E-4  | 359.9999  | 2500765 |
# | de_a_91       | -3.3170285  | 41.251926  | -89.88964    | 89.91136  | 2500765 |
# | MAIN_ID       |             |            |              |           | 2440900 |
# | SP_TYPE       |             |            |              |           | 488455  |
# | OTYPES        |             |            |              |           | 2440900 |
# | XM_SIMBAD_SEP | 0.11578731  | 0.11118041 | 4.0487063E-5 | 1.3919119 | 2440900 |
# | GROUP_SIZE_3  | 0.025730526 | 0.22779521 | 0            | 4         | 2500765 |
# | IRflag        | 1.0801827   | 1.5521469  | 0            | 7         | 456246  |
# | Lflux_med     | 1.4305483   | 38.167324  | 2.5337382E-4 | 15720.791 | 456152  |
# | e_Lflux_med   | 0.38149986  | 11.931825  | 3.0239256E-8 | 3939.0857 | 423985  |
# | Mflux_med     | 0.836951    | 17.407589  | 1.2762281E-4 | 7141.7    | 456154  |
# | e_Mflux_med   | 0.29414588  | 9.36655    | 8.423023E-10 | 3063.0676 | 424045  |
# | Nflux_med     | 0.2753638   | 10.084012  | 9.8775825E-5 | 3850.1    | 456147  |
# | e_Nflux_med   | 0.10006758  | 3.2933166  | 9.378568E-10 | 1230.4095 | 425646  |
# +---------------+-------------+------------+--------------+-----------+---------+
fi


# Prepare candidate catalogs using the SearchCal cfg format:

# Convert to VOTables:
# - bright: select stars WITH SP_TYPE + neighbours
CATALOG_BRIGHT="3_xmatch_ASCC_SIMBAD_MDFC_full_sptype.vot"
if [ $FULL_CANDIDATES -nt $CATALOG_BRIGHT ]
then
    CATALOG_BRIGHT_0="3_xmatch_ASCC_SIMBAD_MDFC_full_sptype_0.fits"

    # Get all objects with SP_TYPE:
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$FULL_CANDIDATES cmd='progress; select !NULL_SP_TYPE ' out=$CATALOG_BRIGHT_0
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CATALOG_BRIGHT_0 > ${CATALOG_BRIGHT_0}.stats.log

# Total Rows: 488455
# +---------------+-------------+------------+--------------+-----------+--------+
# | column        | mean        | stdDev     | min          | max       | good   |
# +---------------+-------------+------------+--------------+-----------+--------+
# | RAJ2000       |             |            |              |           | 488455 |
# | DEJ2000       |             |            |              |           | 488455 |
# | pmRA          | -1.63245    | 47.13354   | -3775.62     | 6773.35   | 488455 |
# | pmDE          | -7.9467325  | 44.812267  | -5813.06     | 10308.08  | 488455 |
# | ra_a_91       | 192.01067   | 100.77186  | 2.829278E-4  | 359.9991  | 488455 |
# | de_a_91       | -0.78180057 | 40.39382   | -89.831245   | 89.77407  | 488455 |
# | MAIN_ID       |             |            |              |           | 488455 |
# | SP_TYPE       |             |            |              |           | 488455 |
# | OTYPES        |             |            |              |           | 488455 |
# | XM_SIMBAD_SEP | 0.063742615 | 0.08831784 | 6.167478E-5  | 1.3919119 | 488455 |
# | GROUP_SIZE_3  | 0.04337554  | 0.29339322 | 0            | 4         | 488455 |
# | IRflag        | 1.0779454   | 1.5549566  | 0            | 7         | 442015 |
# | Lflux_med     | 1.4562808   | 38.77219   | 2.5337382E-4 | 15720.791 | 441924 |
# | e_Lflux_med   | 0.3885407   | 12.124469  | 3.0239256E-8 | 3939.0857 | 410309 |
# | Mflux_med     | 0.8516749   | 17.681673  | 1.2762281E-4 | 7141.7    | 441926 |
# | e_Mflux_med   | 0.3000101   | 9.51841    | 8.423023E-10 | 3063.0676 | 410368 |
# | Nflux_med     | 0.2804733   | 10.243082  | 9.8775825E-5 | 3850.1    | 441918 |
# | e_Nflux_med   | 0.102010764 | 3.3465314  | 9.378568E-10 | 1230.4095 | 411951 |
# +---------------+-------------+------------+--------------+-----------+--------+

    # Get all neightbours within 3 as:
    # use all to get all objects within 3 as in ASCC FULL for every ASCC BRIGHT source at epoch 1991.25
    stilts ${STILTS_JAVA_OPTIONS} tmatch2 matcher='sky' params='3.0' values1='ra_a_91 de_a_91' values2='ra_a_91 de_a_91' find='all' join='all1' fixcols='all' suffix2='' in1=$CATALOG_BRIGHT_0 in2=$FULL_CANDIDATES out=$CATALOG_BRIGHT
    # remove duplicates (on de_a_91)
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CATALOG_BRIGHT cmd='progress ; sort de_a_91 ; uniq de_a_91 ; delcols "*_1 ra_a_91 de_a_91 GroupID GroupSize Separation" ' out=$CATALOG_BRIGHT
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CATALOG_BRIGHT > ${CATALOG_BRIGHT}.stats.log

# Total Rows: 498557
# +---------------+-------------+-------------+--------------+-----------+--------+
# | column        | mean        | stdDev      | min          | max       | good   |
# +---------------+-------------+-------------+--------------+-----------+--------+
# | RAJ2000       |             |             |              |           | 498557 |
# | DEJ2000       |             |             |              |           | 498557 |
# | pmRA          | -1.6241819  | 47.685818   | -3775.62     | 6773.35   | 498557 |
# | pmDE          | -8.021805   | 45.536568   | -5813.06     | 10308.08  | 498557 |
# | MAIN_ID       |             |             |              |           | 492320 |
# | SP_TYPE       |             |             |              |           | 488455 |
# | OTYPES        |             |             |              |           | 492320 |
# | XM_SIMBAD_SEP | 0.064446196 | 0.090264134 | 6.167478E-5  | 1.3919119 | 492320 |
# | GROUP_SIZE_3  | 0.0835411   | 0.403558    | 0            | 4         | 498557 |
# | IRflag        | 1.0779454   | 1.5549566   | 0            | 7         | 442015 |
# | Lflux_med     | 1.4562808   | 38.77219    | 2.5337382E-4 | 15720.791 | 441924 |
# | e_Lflux_med   | 0.3885407   | 12.124469   | 3.0239256E-8 | 3939.0857 | 410309 |
# | Mflux_med     | 0.8516749   | 17.681673   | 1.2762281E-4 | 7141.7    | 441926 |
# | e_Mflux_med   | 0.3000101   | 9.51841     | 8.423023E-10 | 3063.0676 | 410368 |
# | Nflux_med     | 0.2804733   | 10.243082   | 9.8775825E-5 | 3850.1    | 441918 |
# | e_Nflux_med   | 0.102010764 | 3.3465314   | 9.378568E-10 | 1230.4095 | 411951 |
# +---------------+-------------+-------------+--------------+-----------+--------+
fi


# - faint: select stars WITHOUT SP_TYPE nor neighbours (not in CATALOG_BRIGHT)
CATALOG_FAINT="3_xmatch_ASCC_SIMBAD_MDFC_full_NO_sptype.vot"
if [ $FULL_CANDIDATES -nt $CATALOG_FAINT ]
then
    CATALOG_FAINT_0="3_xmatch_ASCC_SIMBAD_MDFC_full_NO_sptype_0.fits"

    # delete any MDFC columns as it should correspond to JSDC BRIGHT
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$FULL_CANDIDATES cmd='progress; delcols "ra_a_91 de_a_91 SP_TYPE IRflag Lflux_med e_Lflux_med Mflux_med e_Mflux_med Nflux_med e_Nflux_med" ' out=$CATALOG_FAINT_0
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CATALOG_FAINT_0 > ${CATALOG_FAINT_0}.stats.log

# Total Rows: 2500765
# +---------------+-------------+------------+--------------+-----------+---------+
# | column        | mean        | stdDev     | min          | max       | good    |
# +---------------+-------------+------------+--------------+-----------+---------+
# | RAJ2000       |             |            |              |           | 2500765 |
# | DEJ2000       |             |            |              |           | 2500765 |
# | pmRA          | -1.7544895  | 32.22942   | -4417.35     | 6773.35   | 2500765 |
# | pmDE          | -6.0867248  | 30.385548  | -5813.7      | 10308.08  | 2500765 |
# | MAIN_ID       |             |            |              |           | 2440900 |
# | OTYPES        |             |            |              |           | 2440900 |
# | XM_SIMBAD_SEP | 0.11578731  | 0.11118041 | 4.0487063E-5 | 1.3919119 | 2440900 |
# | GROUP_SIZE_3  | 0.025730526 | 0.22779521 | 0            | 4         | 2500765 |
# +---------------+-------------+------------+--------------+-----------+---------+

    # use DEJ2000 (unique)
    stilts ${STILTS_JAVA_OPTIONS} tmatch2 matcher='exact' values1='DEJ2000' values2='DEJ2000' join='1not2' in1=$CATALOG_FAINT_0 in2=$CATALOG_BRIGHT out=$CATALOG_FAINT
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CATALOG_FAINT > $CATALOG_FAINT.stats.log

# Total Rows: 2002208
# +---------------+-------------+------------+--------------+-----------+---------+
# | column        | mean        | stdDev     | min          | max       | good    |
# +---------------+-------------+------------+--------------+-----------+---------+
# | RAJ2000       |             |            |              |           | 2002208 |
# | DEJ2000       |             |            |              |           | 2002208 |
# | pmRA          | -1.7869366  | 27.039982  | -4417.35     | 4147.35   | 2002208 |
# | pmDE          | -5.604905   | 25.212875  | -5813.7      | 3203.63   | 2002208 |
# | MAIN_ID       |             |            |              |           | 1948580 |
# | OTYPES        |             |            |              |           | 1948580 |
# | XM_SIMBAD_SEP | 0.1287589   | 0.11221139 | 4.0487063E-5 | 1.3556274 | 1948580 |
# | GROUP_SIZE_3  | 0.011335486 | 0.15238021 | 0            | 4         | 2002208 |
# +---------------+-------------+------------+--------------+-----------+---------+
fi


# - test: select only stars with neighbours (GROUP_SIZE_3 > 0)
CATALOG_TEST="3_xmatch_ASCC_SIMBAD_MDFC_full_test.vot"
if [ $FULL_CANDIDATES -nt $CATALOG_TEST ]
then
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$FULL_CANDIDATES cmd='progress; select GROUP_SIZE_3>0 ; delcols "ra_a_91 de_a_91 IRflag Lflux_med e_Lflux_med Mflux_med e_Mflux_med Nflux_med e_Nflux_med" ' out=$CATALOG_TEST
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CATALOG_TEST > $CATALOG_TEST.stats.log

# Total Rows: 31726
# +---------------+------------+------------+--------------+-----------+-------+
# | column        | mean       | stdDev     | min          | max       | good  |
# +---------------+------------+------------+--------------+-----------+-------+
# | RAJ2000       |            |            |              |           | 31726 |
# | DEJ2000       |            |            |              |           | 31726 |
# | pmRA          | -1.0692146 | 72.92933   | -1357.93     | 2183.43   | 31726 |
# | pmDE          | -12.140333 | 70.340744  | -3567.01     | 1449.13   | 31726 |
# | MAIN_ID       |            |            |              |           | 23343 |
# | SP_TYPE       |            |            |              |           | 10496 |
# | OTYPES        |            |            |              |           | 23343 |
# | XM_SIMBAD_SEP | 0.14285932 | 0.18375169 | 1.0709482E-4 | 1.3919119 | 23343 |
# | GROUP_SIZE_3  | 2.0281787  | 0.1699932  | 2            | 4         | 31726 |
# +---------------+------------+------------+--------------+-----------+-------+
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

CONFIG="vobsascc_simbad_test.cfg"
if [ $CATALOG_TEST -nt $CONFIG ]
then
    xsltproc -o $CONFIG sclguiVOTableToTSV.xsl $CATALOG_TEST
fi


echo "That's all folks ! @ date: `date -u`"


