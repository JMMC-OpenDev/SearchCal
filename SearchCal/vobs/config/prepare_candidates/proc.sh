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


# ASCC: internal match within 3 as to identify inner groups and keep all (TYC1 OR groups)
CAT_ASCC_MIN_GRP="1_min_Grp_$CAT_ASCC"
if [ $CAT_ASCC_MIN -nt $CAT_ASCC_MIN_GRP ]
then
    stilts ${STILTS_JAVA_OPTIONS} tmatch1 matcher='sky' params='3.0' values='ra_a_91 de_a_91' action='identify' in=$CAT_ASCC_MIN out=$CAT_ASCC_MIN_GRP
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_ASCC_MIN_GRP cmd='progress ; colmeta -name ASCC_GROUP_SIZE GroupSize ; select (TYC1>0&&!NULL_Vmag)||ASCC_GROUP_SIZE>0 ; delcols "GroupID TYC1" ' out=$CAT_ASCC_MIN_GRP
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_ASCC_MIN_GRP > $CAT_ASCC_MIN_GRP.stats.log

# Total Rows: 2461804
# +-----------------+------------+------------+--------------+-----------+---------+
# | column          | mean       | stdDev     | min          | max       | good    |
# +-----------------+------------+------------+--------------+-----------+---------+
# | _RAJ2000        | 188.46666  | 100.358795 | 3.4094515E-4 | 359.99988 | 2461804 |
# | _DEJ2000        | -3.4710667 | 41.44095   | -89.88966    | 89.83233  | 2461804 |
# | pmRA            | -1.7444646 | 29.870487  | -4417.35     | 6516.79   | 2461804 |
# | pmDE            | -5.849084  | 28.615948  | -5813.7      | 10308.08  | 2461804 |
# | Vmag            | 11.044764  | 1.1174847  | -1.121       | 15.38     | 2461665 |
# | ra_a_91         | 188.46666  | 100.358795 | 2.829278E-4  | 359.9999  | 2461804 |
# | de_a_91         | -3.4710524 | 41.440952  | -89.88964    | 89.83232  | 2461804 |
# | ASCC_GROUP_SIZE | 2.0281894  | 0.17002448 | 2            | 4         | 31714   |
# +-----------------+------------+------------+--------------+-----------+---------+
fi


# Filter SIMBAD objects:
CAT_SIMBAD_OBJ="1_obj_$CAT_SIMBAD"
if [ $CAT_SIMBAD -nt $CAT_SIMBAD_OBJ ]
then
    # filter SIMBAD main_type (stars only) to exclude Planet, Galaxy ...
    # filter on V < 15 (max in TYCHO2)
    # convert SIMBAD coords to epoch 1991.25:
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_SIMBAD cmd='progress; select other_types.contains(\"*\")||main_type.contains(\"*\")||main_type.equals(\"Star\") ; select V<=15.0 ; addcol ra_s_91 "ra  + ((NULL_pmra ) ? 0.0 : (1991.25 - 2000.0) * (0.001 * pmra  / 3600.0))" ; addcol de_s_91 "dec + ((NULL_pmdec) ? 0.0 : (1991.25 - 2000.0) * (0.001 * pmdec / 3600.0))" ' out=$CAT_SIMBAD_OBJ
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_SIMBAD_OBJ > $CAT_SIMBAD_OBJ.stats.log

# Total Rows: 2979582
# +---------------+--------------+------------+-----------+-----------+---------+
# | column        | mean         | stdDev     | min       | max       | good    |
# +---------------+--------------+------------+-----------+-----------+---------+
# | main_id       |              |            |           |           | 2979582 |
# | ra            | 188.05627    | 100.480415 | 0.0       | 359.9999  | 2979582 |
# | dec           | -4.8848443   | 41.370842  | -89.88967 | 89.83233  | 2979582 |
# | coo_err_maj   | 0.007407614  | 0.8273406  | 0.0       | 180.001   | 2941382 |
# | coo_err_min   | 0.0066458248 | 0.8207797  | 0.0       | 180.0     | 2941382 |
# | coo_err_angle | 89.92397     | 13.8204155 | 0.0       | 179.0     | 2979582 |
# | nbref         | 1.7680107    | 15.172445  | 0         | 5333      | 2979582 |
# | ra_sexa       |              |            |           |           | 2979582 |
# | dec_sexa      |              |            |           |           | 2979582 |
# | main_type     |              |            |           |           | 2979582 |
# | other_types   |              |            |           |           | 2979582 |
# | radvel        | 10.111269    | 807.6564   | -1761.35  | 248857.27 | 2067055 |
# | redshift      | 4.2210435E-5 | 0.00482786 | -0.005858 | 2.282     | 2067055 |
# | sp_type       |              |            |           |           | 551218  |
# | morph_type    |              |            |           |           | 1107    |
# | plx           | 2.3651907    | 3.5365798  | 0.0       | 768.5004  | 2811248 |
# | pmra          | -1.6156564   | 28.748816  | -4339.89  | 6766.0    | 2927526 |
# | pmdec         | -5.5149455   | 27.393785  | -5817.86  | 10362.54  | 2927526 |
# | size_maj      | 0.5526408    | 1.7285714  | 0.0       | 60.0      | 6337    |
# | size_min      | 0.43812144   | 1.4879111  | 0.0       | 60.0      | 6336    |
# | size_angle    | 1111.2899    | 5538.1357  | -64       | 32767     | 6366    |
# | B             | 12.043779    | 1.2955753  | -1.46     | 29.24     | 2880794 |
# | V             | 11.363382    | 1.2977239  | -1.46     | 15.0      | 2979582 |
# | R             | 11.924998    | 1.3466221  | -1.46     | 25.55     | 285601  |
# | J             | 9.829083     | 1.4981346  | -3.0      | 18.26     | 2764015 |
# | H             | 9.461935     | 1.6288674  | -3.73     | 18.0      | 2764210 |
# | K             | 9.359436     | 1.6719955  | -4.05     | 17.9      | 2763346 |
# | u             | 15.4271145   | 1.3244288  | 7.51      | 26.23     | 13345   |
# | g             | 12.396041    | 1.1312586  | 3.941     | 25.118    | 379863  |
# | r             | 11.593645    | 1.0896236  | 4.341     | 24.817    | 381619  |
# | i             | 11.1943      | 1.1579121  | 5.23      | 27.3      | 392050  |
# | z             | 12.950209    | 1.427395   | 3.198     | 31.741    | 15394   |
# | ra_s_91       | 188.05629    | 100.48042  | 8.506944E-6 | 359.9999  | 2979582 |
# | de_s_91       | -4.8848314   | 41.370842  | -89.889656  | 89.83232  | 2979582 |
# +---------------+--------------+------------+-----------+-----------+---------+
fi


# SIMBAD: internal match within 3 as to identify inner groups and filter columns:
# (main_id, ra/dec 1991) only to identify groups
CAT_SIMBAD_MIN="1_min_$CAT_SIMBAD_OBJ"
if [ $CAT_SIMBAD_OBJ -nt $CAT_SIMBAD_MIN ]
then
    CAT_SIMBAD_MIN_0="1_min_0_$CAT_SIMBAD_OBJ"

    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_SIMBAD_OBJ cmd='progress ; keepcols "ra_s_91 de_s_91 main_id" ' out=$CAT_SIMBAD_MIN_0
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_SIMBAD_MIN_0 > $CAT_SIMBAD_MIN_0.stats.log

# Total Rows: 2979582
# +---------+------------+-----------+-------------+----------+---------+
# | column  | mean       | stdDev    | min         | max      | good    |
# +---------+------------+-----------+-------------+----------+---------+
# | ra_s_91 | 188.05629  | 100.48042 | 8.506944E-6 | 359.9999 | 2979582 |
# | de_s_91 | -4.8848314 | 41.370842 | -89.889656  | 89.83232 | 2979582 |
# | main_id |            |           |             |          | 2979582 |
# +---------+------------+-----------+-------------+----------+---------+

    # identify inner groups in SIMBAD:
    stilts ${STILTS_JAVA_OPTIONS} tmatch1 matcher='sky' params='3.0' values='ra_s_91 de_s_91' action='identify' in=$CAT_SIMBAD_MIN_0 out=$CAT_SIMBAD_MIN
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_SIMBAD_MIN cmd='progress ; colmeta -name SIMBAD_GROUP_SIZE GroupSize ; delcols "GroupID" ' out=$CAT_SIMBAD_MIN
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_SIMBAD_MIN > $CAT_SIMBAD_MIN.stats.log

# Total Rows: 2979582
# +-------------------+------------+-----------+-------------+----------+---------+
# | column            | mean       | stdDev    | min         | max      | good    |
# +-------------------+------------+-----------+-------------+----------+---------+
# | ra_s_91           | 188.05629  | 100.48042 | 8.506944E-6 | 359.9999 | 2979582 |
# | de_s_91           | -4.8848314 | 41.370842 | -89.889656  | 89.83232 | 2979582 |
# | main_id           |            |           |             |          | 2979582 |
# | SIMBAD_GROUP_SIZE | 2.2399654  | 1.8990138 | 2           | 49       | 66197   |
# +-------------------+------------+-----------+-------------+----------+---------+
fi


# SIMBAD: column subset for candidates
CAT_SIMBAD_SUB="1_sub_$CAT_SIMBAD_OBJ"
if [ $CAT_SIMBAD_OBJ -nt $CAT_SIMBAD_SUB ]
then
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_SIMBAD_OBJ cmd='progress ; keepcols "ra_s_91 de_s_91 main_id sp_type main_type other_types V" ' out=$CAT_SIMBAD_SUB
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_SIMBAD_SUB > $CAT_SIMBAD_SUB.stats.log

# Total Rows: 2979582
# +-------------+------------+-----------+-------------+----------+---------+
# | column      | mean       | stdDev    | min         | max      | good    |
# +-------------+------------+-----------+-------------+----------+---------+
# | ra_s_91     | 188.05629  | 100.48042 | 8.506944E-6 | 359.9999 | 2979582 |
# | de_s_91     | -4.8848314 | 41.370842 | -89.889656  | 89.83232 | 2979582 |
# | main_id     |            |           |             |          | 2979582 |
# | sp_type     |            |           |             |          | 551218  |
# | main_type   |            |           |             |          | 2979582 |
# | other_types |            |           |             |          | 2979582 |
# | V           | 11.363382  | 1.2977239 | -1.46       | 15.0     | 2979582 |
# +-------------+------------+-----------+-------------+----------+---------+
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

# Total Rows: 2461804
# +-------------------+-------------+------------+--------------+-----------+---------+
# | column            | mean        | stdDev     | min          | max       | good    |
# +-------------------+-------------+------------+--------------+-----------+---------+
# | _RAJ2000          | 188.46666   | 100.358795 | 3.4094515E-4 | 359.99988 | 2461804 |
# | _DEJ2000          | -3.4710667  | 41.44095   | -89.88966    | 89.83233  | 2461804 |
# | pmRA              | -1.7444646  | 29.870487  | -4417.35     | 6516.79   | 2461804 |
# | pmDE              | -5.849084   | 28.615948  | -5813.7      | 10308.08  | 2461804 |
# | Vmag              | 11.044764   | 1.1174847  | -1.121       | 15.38     | 2461665 |
# | ra_a_91           | 188.46666   | 100.358795 | 2.829278E-4  | 359.9999  | 2461804 |
# | de_a_91           | -3.4710524  | 41.440952  | -89.88964    | 89.83232  | 2461804 |
# | ASCC_GROUP_SIZE   | 2.0281894   | 0.17002448 | 2            | 4         | 31714   |
# | ra_s_91           | 188.49695   | 100.35609  | 2.8021954E-4 | 359.9999  | 2450519 |
# | de_s_91           | -3.5590212  | 41.430428  | -89.889656   | 89.83232  | 2450519 |
# | main_id           |             |            |              |           | 2450519 |
# | SIMBAD_GROUP_SIZE | 2.1197248   | 0.47138155 | 2            | 49        | 38079   |
# | GroupID           | 3562.9397   | 2056.4219  | 1            | 7124      | 14287   |
# | GroupSize         | 2.0081892   | 0.09012324 | 2            | 3         | 14287   |
# | Separation        | 0.104191095 | 0.13728002 | 4.721107E-6  | 2.9999418 | 2450519 |
# +-------------------+-------------+------------+--------------+-----------+---------+

    # Only consider groups between ASCC x SIMBAD within 3as:
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_ASCC_SIMBAD_GRP_0 cmd='progress ; delcols "ra_s_91 de_s_91 main_id SIMBAD_GROUP_SIZE GroupID Separation" ; colmeta -name SIMBAD_GROUP_SIZE GroupSize ' out=$CAT_ASCC_SIMBAD_GRP
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_ASCC_SIMBAD_GRP > $CAT_ASCC_SIMBAD_GRP.stats.log

# Total Rows: 2461804
# +-------------------+------------+------------+--------------+-----------+---------+
# | column            | mean       | stdDev     | min          | max       | good    |
# +-------------------+------------+------------+--------------+-----------+---------+
# | _RAJ2000          | 188.46666  | 100.358795 | 3.4094515E-4 | 359.99988 | 2461804 |
# | _DEJ2000          | -3.4710667 | 41.44095   | -89.88966    | 89.83233  | 2461804 |
# | pmRA              | -1.7444646 | 29.870487  | -4417.35     | 6516.79   | 2461804 |
# | pmDE              | -5.849084  | 28.615948  | -5813.7      | 10308.08  | 2461804 |
# | Vmag              | 11.044764  | 1.1174847  | -1.121       | 15.38     | 2461665 |
# | ra_a_91           | 188.46666  | 100.358795 | 2.829278E-4  | 359.9999  | 2461804 |
# | de_a_91           | -3.4710524 | 41.440952  | -89.88964    | 89.83232  | 2461804 |
# | ASCC_GROUP_SIZE   | 2.0281894  | 0.17002448 | 2            | 4         | 31714   |
# | SIMBAD_GROUP_SIZE | 2.0081892  | 0.09012324 | 2            | 3         | 14287   |
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

# Total Rows: 2461804
# +-------------------+------------+-------------+--------------+-----------+---------+
# | column            | mean       | stdDev      | min          | max       | good    |
# +-------------------+------------+-------------+--------------+-----------+---------+
# | _RAJ2000          | 188.46666  | 100.358795  | 3.4094515E-4 | 359.99988 | 2461804 |
# | _DEJ2000          | -3.4710667 | 41.44095    | -89.88966    | 89.83233  | 2461804 |
# | pmRA              | -1.7444646 | 29.870487   | -4417.35     | 6516.79   | 2461804 |
# | pmDE              | -5.849084  | 28.615948   | -5813.7      | 10308.08  | 2461804 |
# | Vmag              | 11.044764  | 1.1174847   | -1.121       | 15.38     | 2461665 |
# | ra_a_91           | 188.46666  | 100.358795  | 2.829278E-4  | 359.9999  | 2461804 |
# | de_a_91           | -3.4710524 | 41.440952   | -89.88964    | 89.83232  | 2461804 |
# | ASCC_GROUP_SIZE   | 2.0281894  | 0.17002448  | 2            | 4         | 31714   |
# | SIMBAD_GROUP_SIZE | 2.0081892  | 0.09012324  | 2            | 3         | 14287   |
# | main_id           |            |             |              |           | 2434926 |
# | sp_type           |            |             |              |           | 487194  |
# | main_type         |            |             |              |           | 2434926 |
# | other_types       |            |             |              |           | 2434926 |
# | V                 | 11.061619  | 1.1057166   | -1.46        | 14.86     | 2434926 |
# | XM_SIMBAD_SEP     | 0.11550127 | 0.110879995 | 4.0487063E-5 | 1.396678  | 2434926 |
# +-------------------+------------+-------------+--------------+-----------+---------+
fi


# 2. Crossmatch ASCC / SIMBAD / MDFC (SIMBAD main_id):
CAT_ASCC_SIMBAD_MDFC_XMATCH="2_xmatch_ASCC_SIMBAD_MDFC_full.fits"
if [ $CAT_ASCC_SIMBAD_XMATCH -nt $CAT_ASCC_SIMBAD_MDFC_XMATCH ] || [ $CAT_MDFC_MIN -nt $CAT_ASCC_SIMBAD_MDFC_XMATCH ]
then
    stilts ${STILTS_JAVA_OPTIONS} tmatch2 matcher='exact' values1='main_id' values2='Name' join='all1' in1=$CAT_ASCC_SIMBAD_XMATCH in2=$CAT_MDFC_MIN out=$CAT_ASCC_SIMBAD_MDFC_XMATCH
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_ASCC_SIMBAD_MDFC_XMATCH cmd='progress ; delcols "Name" ' out=$CAT_ASCC_SIMBAD_MDFC_XMATCH
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_ASCC_SIMBAD_MDFC_XMATCH > $CAT_ASCC_SIMBAD_MDFC_XMATCH.stats.log

# Total Rows: 2461804
# +-------------------+------------+-------------+--------------+-----------+---------+
# | column            | mean       | stdDev      | min          | max       | good    |
# +-------------------+------------+-------------+--------------+-----------+---------+
# | _RAJ2000          | 188.46666  | 100.358795  | 3.4094515E-4 | 359.99988 | 2461804 |
# | _DEJ2000          | -3.4710667 | 41.44095    | -89.88966    | 89.83233  | 2461804 |
# | pmRA              | -1.7444646 | 29.870487   | -4417.35     | 6516.79   | 2461804 |
# | pmDE              | -5.849084  | 28.615948   | -5813.7      | 10308.08  | 2461804 |
# | Vmag              | 11.044764  | 1.1174847   | -1.121       | 15.38     | 2461665 |
# | ra_a_91           | 188.46666  | 100.358795  | 2.829278E-4  | 359.9999  | 2461804 |
# | de_a_91           | -3.4710524 | 41.440952   | -89.88964    | 89.83232  | 2461804 |
# | ASCC_GROUP_SIZE   | 2.0281894  | 0.17002448  | 2            | 4         | 31714   |
# | SIMBAD_GROUP_SIZE | 2.0081892  | 0.09012324  | 2            | 3         | 14287   |
# | main_id           |            |             |              |           | 2434926 |
# | sp_type           |            |             |              |           | 487194  |
# | main_type         |            |             |              |           | 2434926 |
# | other_types       |            |             |              |           | 2434926 |
# | V                 | 11.061619  | 1.1057166   | -1.46        | 14.86     | 2434926 |
# | XM_SIMBAD_SEP     | 0.11550127 | 0.110879995 | 4.0487063E-5 | 1.396678  | 2434926 |
# | IRflag            | 1.0794463  | 1.5521843   | 0            | 7         | 455314  |
# | Lflux_med         | 1.4329661  | 38.206036   | 4.2477768E-4 | 15720.791 | 455226  |
# | e_Lflux_med       | 0.38191628 | 11.940007   | 3.0239256E-8 | 3939.0857 | 423411  |
# | Mflux_med         | 0.83840597 | 17.42548    | 2.456566E-4  | 7141.7    | 455228  |
# | e_Mflux_med       | 0.29451185 | 9.37296     | 8.423023E-10 | 3063.0676 | 423471  |
# | Nflux_med         | 0.27495852 | 10.071913   | 9.8775825E-5 | 3850.1    | 455220  |
# | e_Nflux_med       | 0.09860231 | 3.126555    | 9.378568E-10 | 1230.4095 | 425056  |
# +-------------------+------------+-------------+--------------+-----------+---------+
fi


# 3. Prepare candidates: columns, sort, then filters:
FULL_CANDIDATES="3_xmatch_ASCC_SIMBAD_MDFC_full_cols.fits"
if [ $CAT_ASCC_SIMBAD_MDFC_XMATCH -nt $FULL_CANDIDATES ]
then
    # note: merge (main_type,other_types) as other_types can be NULL:
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_ASCC_SIMBAD_MDFC_XMATCH cmd='progress ; addcol -after sp_type -ucd OBJ_TYPES OTYPES NULL_main_type?\"\":replaceAll(concat(\",\",main_type,\",\",replaceAll(other_types,\"[|]\",\",\"),\",\"),\"\ \",\"\") ; delcols "main_type other_types Vmag V" ; addcol -before _RAJ2000 -ucd POS_EQ_RA_MAIN RAJ2000 degreesToHms(_RAJ2000,6) ; addcol -after RAJ2000 -ucd POS_EQ_DEC_MAIN DEJ2000 degreesToDms(_DEJ2000,6) ; colmeta -ucd POS_EQ_PMRA pmRA ; colmeta -ucd POS_EQ_PMDEC pmDE ; colmeta -name MAIN_ID -ucd ID_MAIN main_id ; colmeta -name SP_TYPE -ucd SPECT_TYPE_MK sp_type ; addcol -after XM_SIMBAD_SEP -ucd GROUP_SIZE GROUP_SIZE_3 max((!NULL_ASCC_GROUP_SIZE?ASCC_GROUP_SIZE:0),(!NULL_SIMBAD_GROUP_SIZE?SIMBAD_GROUP_SIZE:0)) ; sort _DEJ2000 ; delcols "_RAJ2000 _DEJ2000 ASCC_GROUP_SIZE SIMBAD_GROUP_SIZE " ' out=$FULL_CANDIDATES
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$FULL_CANDIDATES > $FULL_CANDIDATES.stats.log

# Total Rows: 2461804
# +---------------+-------------+-------------+--------------+-----------+---------+
# | column        | mean        | stdDev      | min          | max       | good    |
# +---------------+-------------+-------------+--------------+-----------+---------+
# | RAJ2000       |             |             |              |           | 2461804 |
# | DEJ2000       |             |             |              |           | 2461804 |
# | pmRA          | -1.7444646  | 29.870487   | -4417.35     | 6516.79   | 2461804 |
# | pmDE          | -5.849084   | 28.615948   | -5813.7      | 10308.08  | 2461804 |
# | ra_a_91       | 188.46666   | 100.358795  | 2.829278E-4  | 359.9999  | 2461804 |
# | de_a_91       | -3.4710524  | 41.440952   | -89.88964    | 89.83232  | 2461804 |
# | MAIN_ID       |             |             |              |           | 2434926 |
# | SP_TYPE       |             |             |              |           | 487194  |
# | OTYPES        |             |             |              |           | 2434926 |
# | XM_SIMBAD_SEP | 0.11550127  | 0.110879995 | 4.0487063E-5 | 1.396678  | 2434926 |
# | GROUP_SIZE_3  | 0.026136119 | 0.22956063  | 0            | 4         | 2461804 |
# | IRflag        | 1.0794463   | 1.5521843   | 0            | 7         | 455314  |
# | Lflux_med     | 1.4329661   | 38.206036   | 4.2477768E-4 | 15720.791 | 455226  |
# | e_Lflux_med   | 0.38191628  | 11.940007   | 3.0239256E-8 | 3939.0857 | 423411  |
# | Mflux_med     | 0.83840597  | 17.42548    | 2.456566E-4  | 7141.7    | 455228  |
# | e_Mflux_med   | 0.29451185  | 9.37296     | 8.423023E-10 | 3063.0676 | 423471  |
# | Nflux_med     | 0.27495852  | 10.071913   | 9.8775825E-5 | 3850.1    | 455220  |
# | e_Nflux_med   | 0.09860231  | 3.126555    | 9.378568E-10 | 1230.4095 | 425056  |
# +---------------+-------------+-------------+--------------+-----------+---------+
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

# Total Rows: 487194
# +---------------+-------------+-------------+--------------+-----------+--------+
# | column        | mean        | stdDev      | min          | max       | good   |
# +---------------+-------------+-------------+--------------+-----------+--------+
# | RAJ2000       |             |             |              |           | 487194 |
# | DEJ2000       |             |             |              |           | 487194 |
# | pmRA          | -1.6164962  | 43.69633    | -3678.16     | 6516.79   | 487194 |
# | pmDE          | -7.8362947  | 42.95018    | -5745.04     | 10308.08  | 487194 |
# | ra_a_91       | 192.04796   | 100.76177   | 2.829278E-4  | 359.9991  | 487194 |
# | de_a_91       | -0.81987256 | 40.424625   | -89.831245   | 89.77407  | 487194 |
# | MAIN_ID       |             |             |              |           | 487194 |
# | SP_TYPE       |             |             |              |           | 487194 |
# | OTYPES        |             |             |              |           | 487194 |
# | XM_SIMBAD_SEP | 0.06332975  | 0.087469876 | 6.167478E-5  | 1.3919119 | 487194 |
# | GROUP_SIZE_3  | 0.044608515 | 0.29738894  | 0            | 4         | 487194 |
# | IRflag        | 1.0772922   | 1.5548832   | 0            | 7         | 441209 |
# | Lflux_med     | 1.4584284   | 38.807247   | 4.2477768E-4 | 15720.791 | 441124 |
# | e_Lflux_med   | 0.38890016  | 12.131771   | 3.0239256E-8 | 3939.0857 | 409822 |
# | Mflux_med     | 0.8529724   | 17.6979     | 2.456566E-4  | 7141.7    | 441126 |
# | e_Mflux_med   | 0.30033338  | 9.524129    | 8.423023E-10 | 3063.0676 | 409881 |
# | Nflux_med     | 0.27998748  | 10.229658   | 9.8775825E-5 | 3850.1    | 441117 |
# | e_Nflux_med   | 0.100478515 | 3.1766999   | 9.378568E-10 | 1230.4095 | 411448 |
# +---------------+-------------+-------------+--------------+-----------+--------+

    # Get all neightbours within 3 as:
    # use all to get all objects within 3 as in ASCC FULL for every ASCC BRIGHT source at epoch 1991.25
    stilts ${STILTS_JAVA_OPTIONS} tmatch2 matcher='sky' params='3.0' values1='ra_a_91 de_a_91' values2='ra_a_91 de_a_91' find='all' join='all1' fixcols='all' suffix2='' in1=$CATALOG_BRIGHT_0 in2=$FULL_CANDIDATES out=$CATALOG_BRIGHT
    # remove duplicates (on de_a_91)
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CATALOG_BRIGHT cmd='progress ; sort de_a_91 ; uniq de_a_91 ; delcols "*_1 ra_a_91 de_a_91 GroupID GroupSize Separation" ' out=$CATALOG_BRIGHT
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CATALOG_BRIGHT > ${CATALOG_BRIGHT}.stats.log

# Total Rows: 497585
# +---------------+-------------+------------+--------------+-----------+--------+
# | column        | mean        | stdDev     | min          | max       | good   |
# +---------------+-------------+------------+--------------+-----------+--------+
# | RAJ2000       |             |            |              |           | 497585 |
# | DEJ2000       |             |            |              |           | 497585 |
# | pmRA          | -1.6102024  | 44.377213  | -3678.16     | 6516.79   | 497585 |
# | pmDE          | -7.917872   | 43.752426  | -5745.04     | 10308.08  | 497585 |
# | MAIN_ID       |             |            |              |           | 491294 |
# | SP_TYPE       |             |            |              |           | 487194 |
# | OTYPES        |             |            |              |           | 491294 |
# | XM_SIMBAD_SEP | 0.06408255  | 0.08956791 | 6.167478E-5  | 1.3919119 | 491294 |
# | GROUP_SIZE_3  | 0.08596521  | 0.40903273 | 0            | 4         | 497585 |
# | IRflag        | 1.0772922   | 1.5548832  | 0            | 7         | 441209 |
# | Lflux_med     | 1.4584284   | 38.807247  | 4.2477768E-4 | 15720.791 | 441124 |
# | e_Lflux_med   | 0.38890016  | 12.131771  | 3.0239256E-8 | 3939.0857 | 409822 |
# | Mflux_med     | 0.8529724   | 17.6979    | 2.456566E-4  | 7141.7    | 441126 |
# | e_Mflux_med   | 0.30033338  | 9.524129   | 8.423023E-10 | 3063.0676 | 409881 |
# | Nflux_med     | 0.27998748  | 10.229658  | 9.8775825E-5 | 3850.1    | 441117 |
# | e_Nflux_med   | 0.100478515 | 3.1766999  | 9.378568E-10 | 1230.4095 | 411448 |
# +---------------+-------------+------------+--------------+-----------+--------+
fi


# - faint: select stars WITHOUT SP_TYPE nor neighbours (not in CATALOG_BRIGHT)
CATALOG_FAINT="3_xmatch_ASCC_SIMBAD_MDFC_full_NO_sptype.vot"
if [ $FULL_CANDIDATES -nt $CATALOG_FAINT ]
then
    CATALOG_FAINT_0="3_xmatch_ASCC_SIMBAD_MDFC_full_NO_sptype_0.fits"

    # delete any MDFC columns as it should correspond to JSDC BRIGHT
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$FULL_CANDIDATES cmd='progress; delcols "ra_a_91 de_a_91 SP_TYPE IRflag Lflux_med e_Lflux_med Mflux_med e_Mflux_med Nflux_med e_Nflux_med" ' out=$CATALOG_FAINT_0
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CATALOG_FAINT_0 > ${CATALOG_FAINT_0}.stats.log

# Total Rows: 2461804
# +---------------+-------------+-------------+--------------+----------+---------+
# | column        | mean        | stdDev      | min          | max      | good    |
# +---------------+-------------+-------------+--------------+----------+---------+
# | RAJ2000       |             |             |              |          | 2461804 |
# | DEJ2000       |             |             |              |          | 2461804 |
# | pmRA          | -1.7444646  | 29.870487   | -4417.35     | 6516.79  | 2461804 |
# | pmDE          | -5.849084   | 28.615948   | -5813.7      | 10308.08 | 2461804 |
# | MAIN_ID       |             |             |              |          | 2434926 |
# | OTYPES        |             |             |              |          | 2434926 |
# | XM_SIMBAD_SEP | 0.11550127  | 0.110879995 | 4.0487063E-5 | 1.396678 | 2434926 |
# | GROUP_SIZE_3  | 0.026136119 | 0.22956063  | 0            | 4        | 2461804 |
# +---------------+-------------+-------------+--------------+----------+---------+

    # use DEJ2000 (unique)
    stilts ${STILTS_JAVA_OPTIONS} tmatch2 matcher='exact' values1='DEJ2000' values2='DEJ2000' join='1not2' in1=$CATALOG_FAINT_0 in2=$CATALOG_BRIGHT out=$CATALOG_FAINT
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CATALOG_FAINT > $CATALOG_FAINT.stats.log

# Total Rows: 1964219
# +---------------+-------------+------------+--------------+----------+---------+
# | column        | mean        | stdDev     | min          | max      | good    |
# +---------------+-------------+------------+--------------+----------+---------+
# | RAJ2000       |             |            |              |          | 1964219 |
# | DEJ2000       |             |            |              |          | 1964219 |
# | pmRA          | -1.7784766  | 24.887478  | -4417.35     | 4006.34  | 1964219 |
# | pmDE          | -5.3250318  | 23.238367  | -5813.7      | 1836.11  | 1964219 |
# | MAIN_ID       |             |            |              |          | 1943632 |
# | OTYPES        |             |            |              |          | 1943632 |
# | XM_SIMBAD_SEP | 0.1284984   | 0.11196864 | 4.0487063E-5 | 1.396678 | 1943632 |
# | GROUP_SIZE_3  | 0.010979936 | 0.15009376 | 0            | 4        | 1964219 |
# +---------------+-------------+------------+--------------+----------+---------+
fi


# - test: select only stars with neighbours (GROUP_SIZE_3 > 0)
CATALOG_TEST="3_xmatch_ASCC_SIMBAD_MDFC_full_test.vot"
if [ $FULL_CANDIDATES -nt $CATALOG_TEST ]
then
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$FULL_CANDIDATES cmd='progress; select GROUP_SIZE_3>0 ; delcols "IRflag Lflux_med e_Lflux_med Mflux_med e_Mflux_med Nflux_med e_Nflux_med" ' out=$CATALOG_TEST
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CATALOG_TEST > $CATALOG_TEST.stats.log

# Total Rows: 31440
# +---------------+------------+-----------+--------------+-----------+-------+
# | column        | mean       | stdDev    | min          | max       | good  |
# +---------------+------------+-----------+--------------+-----------+-------+
# | RAJ2000       |            |           |              |           | 31440 |
# | DEJ2000       |            |           |              |           | 31440 |
# | pmRA          | -1.0259211 | 71.92532  | -1357.93     | 2183.43   | 31440 |
# | pmDE          | -12.103182 | 69.781265 | -3567.01     | 1449.13   | 31440 |
# | MAIN_ID       |            |           |              |           | 22705 |
# | SP_TYPE       |            |           |              |           | 10696 |
# | OTYPES        |            |           |              |           | 22705 |
# | XM_SIMBAD_SEP | 0.15137193 | 0.1891548 | 2.2881188E-4 | 1.3144449 | 22705 |
# | GROUP_SIZE_3  | 2.0271947  | 0.1672777 | 2            | 4         | 31440 |
# +---------------+------------+-----------+--------------+-----------+-------+
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


