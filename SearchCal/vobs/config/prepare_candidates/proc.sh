#!/bin/bash
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************

# Note on pm correction: use 'pmRa/cos(DE)' not directly 'pmRa' as pmRa(catalog) is expressed as pmRa*cos(DE)!

echo "processing @ date: `date -u`"

# Use large heap + CMS GC:
STILTS_JAVA_OPTIONS="-Xms4g -Xmx4g"
# use /home/... as /tmp is full:
# STILTS_JAVA_OPTIONS="$STILTS_JAVA_OPTIONS -Djava.io.tmpdir=/home/bourgesl/tmp/"
# other options:   -verbose -debug
STILTS_JAVA_OPTIONS="$STILTS_JAVA_OPTIONS -disk -verbose"


# debug bash commands:
#set -eux;


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
    
# Total Rows: 2501313
# +----------+------------+------------+--------------+-----------+---------+
# | column   | mean       | stdDev     | min          | max       | good    |
# +----------+------------+------------+--------------+-----------+---------+
# | _RAJ2000 | 188.87096  | 100.47709  | 3.4094515E-4 | 359.99988 | 2501313 |
# | _DEJ2000 | -3.317228  | 41.249565  | -89.88966    | 89.911354 | 2501313 |
# | pmRA     | -1.7540642 | 32.2306    | -4417.35     | 6773.35   | 2501313 |
# | pmDE     | -6.0875754 | 30.390753  | -5813.7      | 10308.08  | 2501313 |
# | Vmag     | 11.069625  | 1.1319602  | -1.121       | 16.746    | 2500626 |
# | TYC1     | 4933.0464  | 2840.6257  | 0            | 9537      | 2501313 |
# | TYC2     | 1085.0278  | 858.9906   | 0            | 12121     | 2501313 |
# | TYC3     | 0.9864971  | 0.15440936 | 0            | 4         | 2501313 |
# +----------+------------+------------+--------------+-----------+---------+
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
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_MDFC > $CAT_MDFC.stats.log
    
# Total Rows: 465857
# +--------------+-------------+-------------+--------------+-----------+--------+
# | column       | mean        | stdDev      | min          | max       | good   |
# +--------------+-------------+-------------+--------------+-----------+--------+
# | Name         |             |             |              |           | 465857 |
# | SpType       |             |             |              |           | 465857 |
# | RAJ2000      |             |             |              |           | 465857 |
# | DEJ2000      |             |             |              |           | 465857 |
# | distance     | 567.73395   | 660.71625   | 1.3011867    | 18404.096 | 460564 |
# | teff_midi    | 4384.0073   | 1514.4413   | 3327         | 25645     | 403    |
# | teff_gaia    | 6099.031    | 1522.948    | 2241.0       | 64977.0   | 463781 |
# | Comp         |             |             |              |           | 4946   |
# | mean_sep     | 27.04654    | 96.56517    | -1.0         | 999.9     | 28003  |
# | mag1         | 9.140074    | 1.6275178   | -1.47        | 16.2      | 28003  |
# | mag2         | 11.752113   | 2.1467986   | 0.18         | 24.6      | 27611  |
# | diam_midi    | 3.2239852   | 1.9918951   | 0.92         | 20.398    | 402    |
# | e_diam_midi  | 0.016890548 | 0.011792271 | 0.004        | 0.087     | 402    |
# | diam_cohen   | 2.7435546   | 1.189701    | 1.59         | 10.03     | 422    |
# | e_diam_cohen | 0.036658768 | 0.016024187 | 0.018        | 0.121     | 422    |
# | diam_gaia    | 0.17881162  | 0.3026772   | 9.893455E-4  | 43.475067 | 429824 |
# | LDD_meas     | 2.67697     | 3.2892106   | 0.225        | 39.759    | 567    |
# | e_diam_meas  | 0.34178808  | 4.2889524   | 0.001        | 120.0     | 788    |
# | UDD_meas     | 4.4887686   | 16.708101   | 0.05         | 420.0     | 700    |
# | band_meas    |             |             |              |           | 781    |
# | LDD_est      | 0.18713047  | 0.33603582  | 0.002        | 44.85     | 465605 |
# | e_diam_est   | 0.007890246 | 0.029105874 | 0.0          | 2.885     | 465605 |
# | UDDL_est     | 0.1849376   | 0.33243763  | 0.002        | 44.459    | 465605 |
# | UDDM_est     | 0.18540157  | 0.3329401   | 0.002        | 44.476    | 465605 |
# | UDDN_est     | 0.18608275  | 0.33407295  | 0.002        | 44.603    | 465605 |
# | Jmag         | 8.347455    | 1.5624622   | -2.989       | 15.447    | 465188 |
# | Hmag         | 8.036082    | 1.7315052   | -4.007       | 15.439    | 465857 |
# | Kmag         | 7.9406104   | 1.7788626   | -4.378       | 15.638    | 465857 |
# | W4mag        | 7.4334354   | 1.5221424   | -6.859       | 10.149    | 465113 |
# | CalFlag      | 0.09299205  | 0.55026287  | 0            | 7         | 465857 |
# | IRflag       | 1.0827936   | 1.554538    | 0            | 7         | 465857 |
# | nb_Lflux     | 1.9706455   | 0.34433657  | 0            | 5         | 465857 |
# | med_Lflux    | 1.5254774   | 42.61173    | 1.6299028E-4 | 15720.791 | 465755 |
# | disp_Lflux   | 0.30919802  | 15.989581   | 2.0396099E-8 | 7742.2144 | 432072 |
# | nb_Mflux     | 1.9757823   | 0.36098757  | 0            | 6         | 465857 |
# | med_Mflux    | 0.8892675   | 19.973076   | 8.886453E-5  | 7141.7    | 465757 |
# | disp_Mflux   | 0.22529167  | 9.646683    | 5.681251E-10 | 4292.073  | 432130 |
# | nb_Nflux     | 2.4884224   | 0.94565606  | 0            | 9         | 465857 |
# | med_Nflux    | 0.33924013  | 18.71475    | 9.8775825E-5 | 9278.1    | 465750 |
# | disp_Nflux   | 0.0845671   | 4.924083    | 6.325757E-10 | 2386.5    | 433969 |
# | Lcorflux_30  | 1.2305617   | 10.585987   | 1.6299028E-4 | 1681.6018 | 465521 |
# | Lcorflux_100 | 1.0548543   | 5.1469135   | 1.6299027E-4 | 956.6236  | 465521 |
# | Lcorflux_130 | 0.9943006   | 4.3013268   | 1.6299025E-4 | 872.3458  | 465521 |
# | Mcorflux_30  | 0.75047773  | 5.9194183   | 8.886453E-5  | 1237.3656 | 465507 |
# | Mcorflux_100 | 0.6851532   | 3.415813    | 8.886452E-5  | 521.01306 | 465507 |
# | Mcorflux_130 | 0.65762204  | 2.9565108   | 8.886452E-5  | 496.79926 | 465507 |
# | Ncorflux_30  | 0.23722996  | 5.3514657   | 9.877582E-5  | 2300.2861 | 465501 |
# | Ncorflux_100 | 0.22025737  | 3.0437691   | 9.877579E-5  | 994.1644  | 465501 |
# | Ncorflux_130 | 0.21549404  | 2.8604133   | 9.877577E-5  | 994.1628  | 465501 |
# +--------------+-------------+-------------+--------------+-----------+--------+
fi


# 1. Convert catalogs to FITS:
#    Convert SIMBAD to FITS:
CAT_SIMBAD="$CAT_SIMBAD_ORIG.fits"
if [ $CAT_SIMBAD_ORIG -nt $CAT_SIMBAD ]
then
    stilts ${STILTS_JAVA_OPTIONS} tcopy ifmt=votable in=$CAT_SIMBAD_ORIG out=$CAT_SIMBAD
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_SIMBAD > $CAT_SIMBAD.stats.log

# processing @ date: jeu. 20 juin 2024 12:40:38 UTC
# Total Rows: 17949227
# +---------------+-------------+------------+------------+--------------+----------+
# | column        | mean        | stdDev     | min        | max          | good     |
# +---------------+-------------+------------+------------+--------------+----------+
# | main_id       |             |            |            |              | 17949227 |
# | ra            | 189.23225   | 99.20806   | 0.0        | 359.99997    | 17949227 |
# | dec           | -2.545165   | 37.449253  | -89.88967  | 90.0         | 17949227 |
# | coo_err_maj   | 10.6171255  | 1263.0912  | 0.0        | 636847.25    | 12729099 |
# | coo_err_min   | 10.259488   | 1263.0443  | 0.0        | 636847.25    | 12729097 |
# | coo_err_angle | 110.51879   | 42.964275  | 0.0        | 179.0        | 17949227 |
# | nbref         | 2.289866    | 13.201764  | 0          | 17594        | 17949227 |
# | ra_sexa       |             |            |            |              | 17949227 |
# | dec_sexa      |             |            |            |              | 17949227 |
# | coo_qual      |             |            |            |              | 17949227 |
# | coo_bibcode   |             |            |            |              | 17795999 |
# | main_type     |             |            |            |              | 17949227 |
# | other_types   |             |            |            |              | 17929077 |
# | radvel        | 55587.547   | 83065.4    | -84723.35  | 299297.47    | 7782342  |
# | radvel_err    | 1336.7487   | 141707.39  | -1798754.8 | 2.62345392E8 | 5555886  |
# | redshift      | 0.3758983   | 0.74321496 | -9.99898   | 33.79        | 7782581  |
# | redshift_err  | 0.004458912 | 0.47268495 | -6.0       | 875.09       | 5555886  |
# | sp_type       |             |            |            |              | 1024614  |
# | morph_type    |             |            |            |              | 147361   |
# | plx           | 1.7882215   | 3.5990138  | -0.418     | 768.0665     | 8089801  |
# | plx_err       | 0.14216563  | 0.27729768 | 0.0        | 81.0         | 8088223  |
# | pmra          | -1.8373286  | 30.083538  | -8118.9    | 6765.995     | 9546075  |
# | pmdec         | -5.6863594  | 28.547852  | -5817.8    | 10362.394    | 9546075  |
# | pm_err_maj    | 0.2175607   | 1.8380009  | 0.0        | 3852.0       | 9543490  |
# | pm_err_min    | 0.19351049  | 5.3729534  | 0.0        | 16383.0      | 9543413  |
# | pm_err_pa     | 909.3635    | 5109.3535  | 0          | 32767        | 9788707  |
# | size_maj      | 5.0902324   | 400.1215   | -0.017833  | 429050.0     | 1769434  |
# | size_min      | 4.70405     | 354.4493   | -0.017833  | 429050.0     | 1769413  |
# | size_angle    | 82.967445   | 780.4385   | -267       | 32767        | 1677003  |
# | B             | 15.006896   | 4.395028   | -1.46      | 45.38        | 5227917  |
# | V             | 14.64559    | 4.5483494  | -1.46      | 38.76        | 5563828  |
# | R             | 16.192873   | 4.326317   | -1.46      | 50.0         | 2296309  |
# | J             | 12.375996   | 3.906772   | -3.0       | 35.847       | 7703395  |
# | H             | 11.816386   | 4.167884   | -3.73      | 34.92        | 7729060  |
# | K             | 11.513771   | 4.0147085  | -4.05      | 33.274       | 7742051  |
# | u             | 21.632421   | 2.2180352  | 0.0        | 37.003       | 3054247  |
# | g             | 18.676052   | 3.3995552  | 0.0        | 34.298       | 5059848  |
# | r             | 17.800125   | 3.311706   | 0.0        | 35.667       | 5107644  |
# | i             | 17.64346    | 3.636459   | 0.0        | 35.288       | 4482122  |
# | z             | 19.038084   | 2.1845028  | 0.0        | 36.46        | 3326480  |
# +---------------+-------------+------------+------------+--------------+----------+
fi


# ASCC: filter columns: (ra/dec J2000 corrected with pm) - from epoch 1991.25
CAT_ASCC_MIN="1_min_$CAT_ASCC"
if [ $CAT_ASCC -nt $CAT_ASCC_MIN ]
then
    # convert ASCC coords J2000 to epoch 1991.25 (as in original table):
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_ASCC cmd='progress ; keepcols "_RAJ2000 _DEJ2000 pmRA pmDE Vmag" ; addcol ra_a_91 "_RAJ2000 + (1991.25 - 2000.0) * (0.001 * pmRA / 3600.0) / cos(degreesToRadians(_DEJ2000))" ; addcol de_a_91 "_DEJ2000 + (1991.25 - 2000.0) * (0.001 * pmDE / 3600.0)" ' out=$CAT_ASCC_MIN
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
# | ra_a_91  | 188.87096  | 100.4771  | 2.8266E-4    | 359.9999  | 2501313 |
# | de_a_91  | -3.3172133 | 41.24957  | -89.88964    | 89.91136  | 2501313 |
# +----------+------------+-----------+--------------+-----------+---------+
fi


# ASCC: internal match within 3 as to identify inner groups and keep all (V defined)
CAT_ASCC_MIN_GRP="1_min_Grp_$CAT_ASCC"
if [ $CAT_ASCC_MIN -nt $CAT_ASCC_MIN_GRP ]
then
    stilts ${STILTS_JAVA_OPTIONS} tmatch1 matcher='sky' params='3.0' values='ra_a_91 de_a_91' action='identify' in=$CAT_ASCC_MIN out=$CAT_ASCC_MIN_GRP
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_ASCC_MIN_GRP cmd='progress ; colmeta -name ASCC_GROUP_SIZE GroupSize ; select !NULL_Vmag||ASCC_GROUP_SIZE>0 ; delcols "GroupID" ' out=$CAT_ASCC_MIN_GRP
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_ASCC_MIN_GRP > $CAT_ASCC_MIN_GRP.stats.log

# Total Rows: 2500764
# +-----------------+------------+------------+--------------+-----------+---------+
# | column          | mean       | stdDev     | min          | max       | good    |
# +-----------------+------------+------------+--------------+-----------+---------+
# | _RAJ2000        | 188.87296  | 100.47335  | 3.4094515E-4 | 359.99988 | 2500764 |
# | _DEJ2000        | -3.3170354 | 41.25193   | -89.88966    | 89.911354 | 2500764 |
# | pmRA            | -1.754506  | 32.229416  | -4417.35     | 6773.35   | 2500764 |
# | pmDE            | -6.086718  | 30.385551  | -5813.7      | 10308.08  | 2500764 |
# | Vmag            | 11.069625  | 1.1319602  | -1.121       | 16.746    | 2500626 |
# | ra_a_91         | 188.87297  | 100.47336  | 2.8266E-4    | 359.9999  | 2500764 |
# | de_a_91         | -3.3170204 | 41.251934  | -89.88964    | 89.91136  | 2500764 |
# | ASCC_GROUP_SIZE | 2.0283375  | 0.17042844 | 2            | 4         | 31760   |
# +-----------------+------------+------------+--------------+-----------+---------+
fi


# Filter SIMBAD objects:
CAT_SIMBAD_OBJ="1_obj_$CAT_SIMBAD"
if [ $CAT_SIMBAD -nt $CAT_SIMBAD_OBJ ]
then
    # filter SIMBAD main_type (stars only) to exclude Planet, Galaxy ...
    CAT_SIMBAD_OBJ_STAR="1_obj_star_$CAT_SIMBAD"
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_SIMBAD cmd='progress; select !NULL_sp_type||NULL_morph_type&&!NULL_main_type&&(main_type.equals(\"Star\")||!(main_type.contains(\"Galaxy\")||main_type.contains(\"EmG\")||main_type.contains(\"Seyfert\")||main_type.contains(\"GroupG\")||main_type.contains(\"QSO\")||main_type.contains(\"Radio\")||main_type.contains(\"AGN\")||main_type.contains(\"Cl\")||main_type.contains(\"in\")||main_type.contains(\"Inexistent\")||main_type.contains(\"Planet\")||main_type.contains(\"EmObj\")||main_type.contains(\"ISM\")||main_type.contains(\"HII\")||main_type.contains(\"SFR\")||main_type.contains(\"SNR\"))) ' out=$CAT_SIMBAD_OBJ_STAR
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_SIMBAD_OBJ_STAR > $CAT_SIMBAD_OBJ_STAR.stats.log

# Total Rows: 11319958
# +---------------+-------------+------------+-----------+-------------+----------+
# | column        | mean        | stdDev     | min       | max         | good     |
# +---------------+-------------+------------+-----------+-------------+----------+
# | main_id       |             |            |           |             | 11319958 |
# | ra            | 201.50653   | 98.17003   | 0.0       | 359.99997   | 11319958 |
# | dec           | -9.109257   | 39.43584   | -89.88967 | 90.0        | 11319958 |
# | coo_err_maj   | 13.095628   | 1432.5789  | 0.0       | 636847.25   | 9894475  |
# | coo_err_min   | 12.762501   | 1432.5452  | 0.0       | 636847.25   | 9894473  |
# | coo_err_angle | 98.867165   | 30.242893  | 0.0       | 179.0       | 11319958 |
# | nbref         | 2.1612914   | 10.499432  | 0         | 14586       | 11319958 |
# | ra_sexa       |             |            |           |             | 11319958 |
# | dec_sexa      |             |            |           |             | 11319958 |
# | coo_qual      |             |            |           |             | 11319958 |
# | coo_bibcode   |             |            |           |             | 11239753 |
# | main_type     |             |            |           |             | 11319958 |
# | other_types   |             |            |           |             | 11299813 |
# | radvel        | 1149.6407   | 12769.102  | -76618.44 | 297820.84   | 4223402  |
# | radvel_err    | 186.62312   | 24544.459  | -9.99     | 3.5740656E7 | 4091076  |
# | redshift      | 0.007986292 | 0.14932962 | -0.23     | 16.41       | 4223444  |
# | redshift_err  | 6.225058E-4 | 0.0818715  | -3.33E-5  | 119.217995  | 4091076  |
# | sp_type       |             |            |           |             | 1024614  |
# | morph_type    |             |            |           |             | 21       |
# | plx           | 1.8209642   | 3.4917495  | -0.418    | 768.0665    | 7804770  |
# | plx_err       | 0.123237446 | 0.24457942 | 0.0       | 81.0        | 7803300  |
# | pmra          | -1.9418833  | 29.753256  | -8118.9   | 6765.995    | 9038794  |
# | pmdec         | -5.9695005  | 28.327156  | -5817.8   | 10362.394   | 9038794  |
# | pm_err_maj    | 0.18860748  | 1.8617074  | 0.0       | 3852.0      | 9036291  |
# | pm_err_min    | 0.16602176  | 5.518585   | 0.0       | 16383.0     | 9036288  |
# | pm_err_pa     | 171.8845    | 1634.5348  | 0         | 32767       | 9058913  |
# | size_maj      | 43.855255   | 1140.7987  | 0.0       | 429050.0    | 168223   |
# | size_min      | 43.157207   | 1140.4644  | 0.0       | 429050.0    | 168221   |
# | size_angle    | 95.1399     | 690.59265  | -173      | 32767       | 162038   |
# | B             | 13.346442   | 2.7403564  | -1.46     | 45.38       | 4136748  |
# | V             | 13.544195   | 3.4352129  | -1.46     | 32.41       | 4920673  |
# | R             | 13.980618   | 2.593283   | -1.46     | 50.0        | 1513228  |
# | J             | 11.332125   | 2.4195454  | -3.0      | 31.107      | 6695041  |
# | H             | 10.673821   | 2.5020423  | -3.73     | 31.13       | 6695908  |
# | K             | 10.427748   | 2.4951258  | -4.05     | 29.58       | 6698268  |
# | u             | 20.210203   | 2.835515   | 0.003     | 31.431      | 376263   |
# | g             | 15.985093   | 3.071023   | 0.008     | 30.97       | 2178741  |
# | r             | 15.181683   | 2.961821   | 0.003     | 31.197      | 2229973  |
# | i             | 14.004024   | 3.2334564  | 0.004     | 30.405      | 1546908  |
# | z             | 17.64587    | 2.7074056  | 0.004     | 31.866      | 436093   |
# +---------------+-------------+------------+-----------+-------------+----------+

    # filter on V > 16 (not in TYCHO2)
    MAG_FILTER="NULL_V||V<=16.0"
    
    CAT_SIMBAD_OBJ_Vgt16="1_obj_star_Vgt16_$CAT_SIMBAD"
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_SIMBAD_OBJ_STAR cmd="progress; select !(${MAG_FILTER}) " out=$CAT_SIMBAD_OBJ_Vgt16
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_SIMBAD_OBJ_Vgt16 > $CAT_SIMBAD_OBJ_Vgt16.stats.log

# Total Rows: 1158773
# +---------------+--------------+------------+------------+-----------+---------+
# | column        | mean         | stdDev     | min        | max       | good    |
# +---------------+--------------+------------+------------+-----------+---------+
# | main_id       |              |            |            |           | 1158773 |
# | ra            | 172.00421    | 109.33626  | 0.0        | 359.99997 | 1158773 |
# | dec           | -31.593895   | 34.448376  | -87.763954 | 89.08628  | 1158773 |
# | coo_err_maj   | 0.071500905  | 2.6577     | 0.0        | 1080.0    | 939523  |
# | coo_err_min   | 0.06981766   | 2.65607    | 0.0        | 1080.0    | 939523  |
# | coo_err_angle | 103.59387    | 35.95005   | 0.0        | 179.0     | 1158773 |
# | nbref         | 2.714271     | 7.976907   | 0          | 3069      | 1158773 |
# | ra_sexa       |              |            |            |           | 1158773 |
# | dec_sexa      |              |            |            |           | 1158773 |
# | coo_qual      |              |            |            |           | 1158773 |
# | coo_bibcode   |              |            |            |           | 1144424 |
# | main_type     |              |            |            |           | 1158773 |
# | other_types   |              |            |            |           | 1156694 |
# | radvel        | 5792.9263    | 31191.416  | -3588.756  | 295050.4  | 81372   |
# | radvel_err    | 760.2694     | 26576.564  | -9.99      | 2997918.5 | 67591   |
# | redshift      | 0.039870232  | 0.2854097  | -0.0119    | 10.2      | 81375   |
# | redshift_err  | 0.0025359835 | 0.08864988 | -3.33E-5   | 9.99998   | 67591   |
# | sp_type       |              |            |            |           | 35326   |
# | morph_type    |              |            |            |           | 10      |
# | plx           | 0.67531073   | 2.1777518  | 0.0        | 496.0     | 560743  |
# | plx_err       | 0.21119791   | 0.26085845 | 0.0027     | 37.0      | 560743  |
# | pmra          | -0.6329603   | 20.574223  | -2759.0    | 3981.977  | 772591  |
# | pmdec         | -3.7220216   | 19.250458  | -2466.832  | 1680.502  | 772591  |
# | pm_err_maj    | 0.28500172   | 0.5000789  | 0.0        | 75.0      | 772526  |
# | pm_err_min    | 0.24225564   | 18.644283  | 0.0        | 16383.0   | 772526  |
# | pm_err_pa     | 188.42996    | 1791.4901  | 0          | 32767     | 774860  |
# | size_maj      | 0.15473954   | 0.5292663  | 0.0        | 16.78     | 2621    |
# | size_min      | 0.12868503   | 0.41257307 | 0.0        | 8.8       | 2620    |
# | size_angle    | 79.104645    | 1298.8676  | -90        | 32767     | 2542    |
# | B             | 19.449007    | 2.5253253  | 8.09       | 45.38     | 301236  |
# | V             | 18.89002     | 2.010475   | 16.001     | 32.41     | 1158773 |
# | R             | 17.9108      | 3.7991965  | 0.0        | 29.5      | 133489  |
# | J             | 14.713166    | 2.812558   | 1.715      | 30.012    | 413886  |
# | H             | 14.04719     | 3.5463827  | 0.075      | 30.394    | 391801  |
# | K             | 13.691368    | 3.2465434  | -0.935     | 28.074    | 392493  |
# | u             | 22.481693    | 3.9536502  | 14.5       | 28.285    | 28476   |
# | g             | 17.875559    | 1.5853955  | 13.595     | 26.91     | 68619   |
# | r             | 17.235643    | 1.4595783  | 12.383     | 29.39     | 69011   |
# | i             | 19.502174    | 4.3018193  | 9.146      | 28.691    | 67462   |
# | z             | 20.229954    | 4.0303884  | 11.749     | 29.6      | 45789   |
# +---------------+--------------+------------+------------+-----------+---------+

    # filter on V <= 16 (max in TYCHO2)
    # convert SIMBAD coords to epoch 1991.25:
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_SIMBAD_OBJ_STAR cmd="progress; select ${MAG_FILTER} ; addcol ra_s_91 'ra  + ((NULL_pmra ) ? 0.0 : (1991.25 - 2000.0) * (0.001 * pmra / 3600.0) / cos(degreesToRadians(dec)))' ; addcol de_s_91 'dec + ((NULL_pmdec) ? 0.0 : (1991.25 - 2000.0) * (0.001 * pmdec / 3600.0))' " out=$CAT_SIMBAD_OBJ
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_SIMBAD_OBJ > $CAT_SIMBAD_OBJ.stats.log

# Total Rows: 10161185
# +---------------+--------------+------------+------------+-------------+----------+
# | column        | mean         | stdDev     | min        | max         | good     |
# +---------------+--------------+------------+------------+-------------+----------+
# | main_id       |              |            |            |             | 10161185 |
# | ra            | 204.87094    | 96.242096  | 0.0        | 359.9999    | 10161185 |
# | dec           | -6.5451274   | 39.15329   | -89.88967  | 90.0        | 10161185 |
# | coo_err_maj   | 14.462074    | 1505.8486  | 0.0        | 636847.25   | 8954952  |
# | coo_err_min   | 14.094174    | 1505.8136  | 0.0        | 636847.25   | 8954950  |
# | coo_err_angle | 98.32813     | 29.473942  | 0.0        | 179.0       | 10161185 |
# | nbref         | 2.09823      | 10.747755  | 0          | 14586       | 10161185 |
# | ra_sexa       |              |            |            |             | 10161185 |
# | dec_sexa      |              |            |            |             | 10161185 |
# | coo_qual      |              |            |            |             | 10161185 |
# | coo_bibcode   |              |            |            |             | 10095329 |
# | main_type     |              |            |            |             | 10161185 |
# | other_types   |              |            |            |             | 10143119 |
# | radvel        | 1058.4214    | 12112.313  | -76618.44  | 297820.84   | 4142030  |
# | radvel_err    | 176.98637    | 24508.768  | -9.99      | 3.5740656E7 | 4023485  |
# | redshift      | 0.0073599014 | 0.14531596 | -0.23      | 16.41       | 4142069  |
# | redshift_err  | 5.903611E-4  | 0.08175245 | -3.33E-5   | 119.217995  | 4023485  |
# | sp_type       |              |            |            |             | 989288   |
# | morph_type    |              |            |            |             | 11       |
# | plx           | 1.9096465    | 3.5580213  | -0.418     | 768.0665    | 7244027  |
# | plx_err       | 0.11642725   | 0.24194348 | 0.0        | 81.0        | 7242557  |
# | pmra          | -2.0642202   | 30.467318  | -8118.9    | 6765.995    | 8266203  |
# | pmdec         | -6.1795583   | 29.021952  | -5817.8    | 10362.394   | 8266203  |
# | pm_err_maj    | 0.1795962    | 1.9405247  | 0.0        | 3852.0      | 8263765  |
# | pm_err_min    | 0.15889513   | 0.8974675  | 0.0        | 787.0       | 8263762  |
# | pm_err_pa     | 170.3369     | 1619.0671  | 0          | 32767       | 8284053  |
# | size_maj      | 44.546906    | 1149.7777  | 0.0        | 429050.0    | 165602   |
# | size_min      | 43.837967    | 1149.4377  | 0.0        | 429050.0    | 165601   |
# | size_angle    | 95.39546     | 676.4815   | -173       | 32767       | 159496   |
# | B             | 12.867154    | 2.1080518  | -1.46      | 36.65       | 3835512  |
# | V             | 11.897528    | 1.6359329  | -1.46      | 16.0        | 3761900  |
# | R             | 13.600374    | 2.0833354  | -1.46      | 50.0        | 1379739  |
# | J             | 11.109336    | 2.2171638  | -3.0       | 31.107      | 6281155  |
# | H             | 10.464167    | 2.2619543  | -3.73      | 31.13       | 6304107  |
# | K             | 10.224608    | 2.2919157  | -4.05      | 29.58       | 6305775  |
# | u             | 20.02422     | 2.6384737  | 0.003      | 31.431      | 347787   |
# | g             | 15.923617    | 3.0880644  | 0.008      | 30.97       | 2110122  |
# | r             | 15.116089    | 2.9741333  | 0.003      | 31.197      | 2160962  |
# | i             | 13.753311    | 2.9405494  | 0.004      | 30.405      | 1479446  |
# | z             | 17.342716    | 2.3257277  | 0.004      | 31.866      | 390304   |
# | ra_s_91       | 204.87096    | 96.242096  | 0.0        | 359.99994   | 10161185 |
# | de_s_91       | -6.5451155   | 39.15329   | -89.889656 | 90.0        | 10161185 |
# +---------------+--------------+------------+------------+-------------+----------+
fi


# SIMBAD: internal match within 3 as to identify inner groups and filter columns:
# (main_id, ra/dec 1991) only to identify groups
CAT_SIMBAD_MIN="1_min_$CAT_SIMBAD_OBJ"
if [ $CAT_SIMBAD_OBJ -nt $CAT_SIMBAD_MIN ]
then
    CAT_SIMBAD_MIN_0="1_min_0_$CAT_SIMBAD_OBJ"

    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_SIMBAD_OBJ cmd='progress ; keepcols "ra_s_91 de_s_91 main_id" ' out=$CAT_SIMBAD_MIN_0
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_SIMBAD_MIN_0 > $CAT_SIMBAD_MIN_0.stats.log

# Total Rows: 10161185
# +---------+------------+-----------+------------+-----------+----------+
# | column  | mean       | stdDev    | min        | max       | good     |
# +---------+------------+-----------+------------+-----------+----------+
# | ra_s_91 | 204.87096  | 96.242096 | 0.0        | 359.99994 | 10161185 |
# | de_s_91 | -6.5451155 | 39.15329  | -89.889656 | 90.0      | 10161185 |
# | main_id |            |           |            |           | 10161185 |
# +---------+------------+-----------+------------+-----------+----------+
# 

    # identify inner groups in SIMBAD:
    stilts ${STILTS_JAVA_OPTIONS} tmatch1 matcher='sky' params='3.0' values='ra_s_91 de_s_91' action='identify' in=$CAT_SIMBAD_MIN_0 out=$CAT_SIMBAD_MIN
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_SIMBAD_MIN cmd='progress ; colmeta -name SIMBAD_GROUP_SIZE GroupSize ; delcols "GroupID" ' out=$CAT_SIMBAD_MIN
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_SIMBAD_MIN > $CAT_SIMBAD_MIN.stats.log

# Total Rows: 10161185
# +-------------------+------------+-----------+------------+-----------+----------+
# | column            | mean       | stdDev    | min        | max       | good     |
# +-------------------+------------+-----------+------------+-----------+----------+
# | ra_s_91           | 204.87096  | 96.242096 | 0.0        | 359.99994 | 10161185 |
# | de_s_91           | -6.5451155 | 39.15329  | -89.889656 | 90.0      | 10161185 |
# | main_id           |            |           |            |           | 10161185 |
# | SIMBAD_GROUP_SIZE | 64.669334  | 415.8102  | 2          | 3537      | 288715   |
# +-------------------+------------+-----------+------------+-----------+----------+
# 
fi


# SIMBAD: column subset for candidates
CAT_SIMBAD_SUB="1_sub_$CAT_SIMBAD_OBJ"
if [ $CAT_SIMBAD_OBJ -nt $CAT_SIMBAD_SUB ]
then
    # sort V first to ensure brightest source in groups ...
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_SIMBAD_OBJ cmd='progress ; colmeta -name ra_s ra ; colmeta -name de_s dec ; keepcols "ra_s de_s ra_s_91 de_s_91 main_id sp_type main_type other_types V"; sort V ' out=$CAT_SIMBAD_SUB
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_SIMBAD_SUB > $CAT_SIMBAD_SUB.stats.log

# Total Rows: 10161185
# +-------------+------------+-----------+------------+-----------+----------+
# | column      | mean       | stdDev    | min        | max       | good     |
# +-------------+------------+-----------+------------+-----------+----------+
# | ra_s        | 204.87094  | 96.242096 | 0.0        | 359.9999  | 10161185 |
# | de_s        | -6.5451274 | 39.15329  | -89.88967  | 90.0      | 10161185 |
# | ra_s_91     | 204.87096  | 96.242096 | 0.0        | 359.99994 | 10161185 |
# | de_s_91     | -6.5451155 | 39.15329  | -89.889656 | 90.0      | 10161185 |
# | main_id     |            |           |            |           | 10161185 |
# | sp_type     |            |           |            |           | 989288   |
# | main_type   |            |           |            |           | 10161185 |
# | other_types |            |           |            |           | 10143119 |
# | V           | 11.897528  | 1.6359329 | -1.46      | 16.0      | 3761900  |
# +-------------+------------+-----------+------------+-----------+----------+
# 
fi


# MDFC: filter columns for candidates
CAT_MDFC_MIN="1_min_$CAT_MDFC"
if [ $CAT_MDFC -nt $CAT_MDFC_MIN ]
then
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_MDFC cmd='progress ; keepcols "Name RAJ2000 DEJ2000 IRflag med_Lflux disp_Lflux med_Mflux disp_Mflux med_Nflux disp_Nflux" ; colmeta -ucd IR_FLAG IRflag ; colmeta -name Lflux_med -ucd PHOT_FLUX_L med_Lflux ; addcol -after Lflux_med -ucd PHOT_FLUX_L_ERROR e_Lflux_med 1.4826*disp_Lflux ; colmeta -name Mflux_med -ucd PHOT_FLUX_M med_Mflux ; addcol -after Mflux_med -ucd PHOT_FLUX_M_ERROR e_Mflux_med 1.4826*disp_Mflux ; colmeta -name Nflux_med -ucd PHOT_FLUX_N med_Nflux ; addcol -after Nflux_med -ucd PHOT_FLUX_N_ERROR e_Nflux_med 1.4826*disp_Nflux; delcols "disp_Lflux disp_Mflux disp_Nflux" ' out=$CAT_MDFC_MIN
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_MDFC_MIN > $CAT_MDFC_MIN.stats.log

# Total Rows: 465857
# +-------------+------------+-----------+--------------+-----------+--------+
# | column      | mean       | stdDev    | min          | max       | good   |
# +-------------+------------+-----------+--------------+-----------+--------+
# | Name        |            |           |              |           | 465857 |
# | RAJ2000     |            |           |              |           | 465857 |
# | DEJ2000     |            |           |              |           | 465857 |
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

# Total Rows: 2500764
# +-------------------+------------+------------+--------------+-----------+---------+
# | column            | mean       | stdDev     | min          | max       | good    |
# +-------------------+------------+------------+--------------+-----------+---------+
# | _RAJ2000          | 188.87296  | 100.47335  | 3.4094515E-4 | 359.99988 | 2500764 |
# | _DEJ2000          | -3.3170354 | 41.25193   | -89.88966    | 89.911354 | 2500764 |
# | pmRA              | -1.754506  | 32.229416  | -4417.35     | 6773.35   | 2500764 |
# | pmDE              | -6.086718  | 30.385551  | -5813.7      | 10308.08  | 2500764 |
# | Vmag              | 11.069625  | 1.1319602  | -1.121       | 16.746    | 2500626 |
# | ra_a_91           | 188.87297  | 100.47336  | 2.8266E-4    | 359.9999  | 2500764 |
# | de_a_91           | -3.3170204 | 41.251934  | -89.88964    | 89.91136  | 2500764 |
# | ASCC_GROUP_SIZE   | 2.0283375  | 0.17042844 | 2            | 4         | 31760   |
# | ra_s_91           | 188.4954   | 100.38276  | 2.7943638E-4 | 359.99994 | 2473644 |
# | de_s_91           | -3.4155102 | 41.383537  | -89.889656   | 89.83232  | 2473644 |
# | main_id           |            |            |              |           | 2473644 |
# | SIMBAD_GROUP_SIZE | 2.7460039  | 3.0601673  | 2            | 647       | 55178   |
# | GroupID           | 2985.8335  | 1723.312   | 1            | 5970      | 11961   |
# | GroupSize         | 2.0052671  | 0.07238353 | 2            | 3         | 11961   |
# | Separation        | 0.10310553 | 0.14512001 | 1.8650917E-7 | 2.9982488 | 2473644 |
# +-------------------+------------+------------+--------------+-----------+---------+
# 

    # Only consider groups between ASCC x SIMBAD within 3as:
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_ASCC_SIMBAD_GRP_0 cmd='progress ; delcols "ra_s_91 de_s_91 main_id SIMBAD_GROUP_SIZE GroupID Separation" ; colmeta -name SIMBAD_GROUP_SIZE GroupSize ' out=$CAT_ASCC_SIMBAD_GRP
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_ASCC_SIMBAD_GRP > $CAT_ASCC_SIMBAD_GRP.stats.log

# Total Rows: 2500764
# +-------------------+------------+------------+--------------+-----------+---------+
# | column            | mean       | stdDev     | min          | max       | good    |
# +-------------------+------------+------------+--------------+-----------+---------+
# | _RAJ2000          | 188.87296  | 100.47335  | 3.4094515E-4 | 359.99988 | 2500764 |
# | _DEJ2000          | -3.3170354 | 41.25193   | -89.88966    | 89.911354 | 2500764 |
# | pmRA              | -1.754506  | 32.229416  | -4417.35     | 6773.35   | 2500764 |
# | pmDE              | -6.086718  | 30.385551  | -5813.7      | 10308.08  | 2500764 |
# | Vmag              | 11.069625  | 1.1319602  | -1.121       | 16.746    | 2500626 |
# | ra_a_91           | 188.87297  | 100.47336  | 2.8266E-4    | 359.9999  | 2500764 |
# | de_a_91           | -3.3170204 | 41.251934  | -89.88964    | 89.91136  | 2500764 |
# | ASCC_GROUP_SIZE   | 2.0283375  | 0.17042844 | 2            | 4         | 31760   |
# | SIMBAD_GROUP_SIZE | 2.0052671  | 0.07238353 | 2            | 3         | 11961   |
# +-------------------+------------+------------+--------------+-----------+---------+
# 
fi


# 2. Crossmatch ASCC / SIMBAD within 1.5 as to get both DATA
CAT_ASCC_SIMBAD_XMATCH="2_xmatch_ASCC_SIMBAD_full.fits"
if [ $CAT_ASCC_SIMBAD_GRP -nt $CAT_ASCC_SIMBAD_XMATCH ] || [ $CAT_SIMBAD_SUB -nt $CAT_ASCC_SIMBAD_XMATCH ]
then
    # use best to have the best ASCC x SIMBAD object: 
    # can not rely on SIMBAD V that may be missing or wrong ... (could help if missing value could be considered too)
    
    # xmatch on COO(1991.25):
    # note: simbad may have multiple entries (children) for X, X A, X B so main_id & sp_type may differ.
    stilts ${STILTS_JAVA_OPTIONS} tmatch2 matcher='sky' params='1.5' values1='ra_a_91 de_a_91' values2='ra_s_91 de_s_91' find='best' join='all1' in1=$CAT_ASCC_SIMBAD_GRP in2=$CAT_SIMBAD_SUB out=$CAT_ASCC_SIMBAD_XMATCH 
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_ASCC_SIMBAD_XMATCH cmd='progress ; addcol XM_SIMBAD_SEP_2000 "skyDistanceDegrees(_RAJ2000,_DEJ2000,ra_s,de_s) * 3600.0" ; addcol XM_SIMBAD_SEP_91 "skyDistanceDegrees(ra_a_91,de_a_91,ra_s_91,de_s_91) * 3600.0" ; delcols "ra_s de_s ra_s_91 de_s_91" ; colmeta -name XM_SIMBAD_SEP -ucd XM_SIMBAD_SEP Separation ' out=$CAT_ASCC_SIMBAD_XMATCH
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_ASCC_SIMBAD_XMATCH > $CAT_ASCC_SIMBAD_XMATCH.stats.log

# Total Rows: 2500764
# +--------------------+------------+-------------+--------------+-----------+---------+
# | column             | mean       | stdDev      | min          | max       | good    |
# +--------------------+------------+-------------+--------------+-----------+---------+
# | _RAJ2000           | 188.87296  | 100.47335   | 3.4094515E-4 | 359.99988 | 2500764 |
# | _DEJ2000           | -3.3170354 | 41.25193    | -89.88966    | 89.911354 | 2500764 |
# | pmRA               | -1.754506  | 32.229416   | -4417.35     | 6773.35   | 2500764 |
# | pmDE               | -6.086718  | 30.385551   | -5813.7      | 10308.08  | 2500764 |
# | Vmag               | 11.069625  | 1.1319602   | -1.121       | 16.746    | 2500626 |
# | ra_a_91            | 188.87297  | 100.47336   | 2.8266E-4    | 359.9999  | 2500764 |
# | de_a_91            | -3.3170204 | 41.251934   | -89.88964    | 89.91136  | 2500764 |
# | ASCC_GROUP_SIZE    | 2.0283375  | 0.17042844  | 2            | 4         | 31760   |
# | SIMBAD_GROUP_SIZE  | 2.0052671  | 0.07238353  | 2            | 3         | 11961   |
# | main_id            |            |             |              |           | 2462762 |
# | sp_type            |            |             |              |           | 508464  |
# | main_type          |            |             |              |           | 2462762 |
# | other_types        |            |             |              |           | 2462736 |
# | V                  | 11.067058  | 1.1076136   | -1.46        | 15.79     | 2434987 |
# | XM_SIMBAD_SEP      | 0.09782921 | 0.107170224 | 1.8650917E-7 | 1.499898  | 2462762 |
# | XM_SIMBAD_SEP_2000 | 0.11608709 | 0.1416425   | 2.2025993E-6 | 21.158558 | 2462762 |
# | XM_SIMBAD_SEP_91   | 0.09782921 | 0.107170224 | 1.8622927E-7 | 1.499898  | 2462762 |
# +--------------------+------------+-------------+--------------+-----------+---------+
# 
fi

# ~2500 rows where XM_SIMBAD_SEP_2000 > 1.5 as (mostly because of pmRa/De difference or ASCC mistake = wrong association)

# Compare crossmatch on 1991 vs 2000:
CAT_ASCC_SIMBAD_XMATCH_J2K="2_xmatch_ASCC_SIMBAD_full_J2000.fits"
if [ $CAT_ASCC_SIMBAD_XMATCH -nt $CAT_ASCC_SIMBAD_XMATCH_J2K ] || [ $CAT_SIMBAD_SUB -nt $CAT_ASCC_SIMBAD_XMATCH_J2K ]
then
    # use best to have the best ASCC x SIMBAD object: 
    # can not rely on SIMBAD V that may be missing or wrong ... (could help if missing value could be considered too)
    
    # xmatch on COO(J2000):
    stilts ${STILTS_JAVA_OPTIONS} tmatch2 matcher='sky' params='1.5' values1='_RAJ2000 _DEJ2000' values2='ra_s de_s' find='best' join='all1' in1=$CAT_ASCC_SIMBAD_XMATCH in2=$CAT_SIMBAD_SUB out=$CAT_ASCC_SIMBAD_XMATCH_J2K 
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_ASCC_SIMBAD_XMATCH_J2K cmd='progress ; addcol XM_SIMBAD_FLAG "!main_Id_1.equals(main_Id_2)?1:NULL" ; delcols "XM_SIMBAD_SEP_91 XM_SIMBAD_SEP_2000 ra_s de_s ra_s_91 de_s_91 main_id_2 sp_type_2 main_type_2 other_types_2 Separation" ; colmeta -name main_id main_id_1 ; colmeta -name sp_type sp_type_1 ; colmeta -name main_type main_type_1 ; colmeta -name other_types other_types_1 ; colmeta -name V V_1 ; colmeta -name V_SIMBAD V_2' out=$CAT_ASCC_SIMBAD_XMATCH_J2K
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_ASCC_SIMBAD_XMATCH_J2K > $CAT_ASCC_SIMBAD_XMATCH_J2K.stats.log

# Total Rows: 2500764
# +-------------------+------------+-------------+--------------+-----------+---------+
# | column            | mean       | stdDev      | min          | max       | good    |
# +-------------------+------------+-------------+--------------+-----------+---------+
# | _RAJ2000          | 188.87296  | 100.47335   | 3.4094515E-4 | 359.99988 | 2500764 |
# | _DEJ2000          | -3.3170354 | 41.25193    | -89.88966    | 89.911354 | 2500764 |
# | pmRA              | -1.754506  | 32.229416   | -4417.35     | 6773.35   | 2500764 |
# | pmDE              | -6.086718  | 30.385551   | -5813.7      | 10308.08  | 2500764 |
# | Vmag              | 11.069625  | 1.1319602   | -1.121       | 16.746    | 2500626 |
# | ra_a_91           | 188.87297  | 100.47336   | 2.8266E-4    | 359.9999  | 2500764 |
# | de_a_91           | -3.3170204 | 41.251934   | -89.88964    | 89.91136  | 2500764 |
# | ASCC_GROUP_SIZE   | 2.0283375  | 0.17042844  | 2            | 4         | 31760   |
# | SIMBAD_GROUP_SIZE | 2.0052671  | 0.07238353  | 2            | 3         | 11961   |
# | main_id           |            |             |              |           | 2462762 |
# | sp_type           |            |             |              |           | 508464  |
# | main_type         |            |             |              |           | 2462762 |
# | other_types       |            |             |              |           | 2462736 |
# | V                 | 11.067058  | 1.1076136   | -1.46        | 15.79     | 2434987 |
# | XM_SIMBAD_SEP     | 0.09782921 | 0.107170224 | 1.8650917E-7 | 1.499898  | 2462762 |
# | V_SIMBAD          | 11.065956  | 1.108435    | -1.46        | 15.88     | 2432423 |
# | XM_SIMBAD_FLAG    | 1.0        | 0.0         | 1            | 1         | 7207    |
# +-------------------+------------+-------------+--------------+-----------+---------+
# 
fi


# 2. Crossmatch ASCC / SIMBAD / MDFC (SIMBAD main_id):
CAT_ASCC_SIMBAD_MDFC_XMATCH="2_xmatch_ASCC_SIMBAD_MDFC_full.fits"
if [ $CAT_ASCC_SIMBAD_XMATCH_J2K -nt $CAT_ASCC_SIMBAD_MDFC_XMATCH ] || [ $CAT_MDFC_MIN -nt $CAT_ASCC_SIMBAD_MDFC_XMATCH ]
then
    stilts ${STILTS_JAVA_OPTIONS} tmatch2 matcher='exact' values1='main_id' values2='Name' join='all1' in1=$CAT_ASCC_SIMBAD_XMATCH_J2K in2=$CAT_MDFC_MIN out=$CAT_ASCC_SIMBAD_MDFC_XMATCH
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_ASCC_SIMBAD_MDFC_XMATCH cmd='progress ; delcols "RAJ2000 DEJ2000 Name" ' out=$CAT_ASCC_SIMBAD_MDFC_XMATCH
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_ASCC_SIMBAD_MDFC_XMATCH > $CAT_ASCC_SIMBAD_MDFC_XMATCH.stats.log

# Total Rows: 2500764
# +-------------------+------------+-------------+--------------+-----------+---------+
# | column            | mean       | stdDev      | min          | max       | good    |
# +-------------------+------------+-------------+--------------+-----------+---------+
# | _RAJ2000          | 188.87296  | 100.47335   | 3.4094515E-4 | 359.99988 | 2500764 |
# | _DEJ2000          | -3.3170354 | 41.25193    | -89.88966    | 89.911354 | 2500764 |
# | pmRA              | -1.754506  | 32.229416   | -4417.35     | 6773.35   | 2500764 |
# | pmDE              | -6.086718  | 30.385551   | -5813.7      | 10308.08  | 2500764 |
# | Vmag              | 11.069625  | 1.1319602   | -1.121       | 16.746    | 2500626 |
# | ra_a_91           | 188.87297  | 100.47336   | 2.8266E-4    | 359.9999  | 2500764 |
# | de_a_91           | -3.3170204 | 41.251934   | -89.88964    | 89.91136  | 2500764 |
# | ASCC_GROUP_SIZE   | 2.0283375  | 0.17042844  | 2            | 4         | 31760   |
# | SIMBAD_GROUP_SIZE | 2.0052671  | 0.07238353  | 2            | 3         | 11961   |
# | main_id           |            |             |              |           | 2462762 |
# | sp_type           |            |             |              |           | 508464  |
# | main_type         |            |             |              |           | 2462762 |
# | other_types       |            |             |              |           | 2462736 |
# | V                 | 11.067058  | 1.1076136   | -1.46        | 15.79     | 2434987 |
# | XM_SIMBAD_SEP     | 0.09782921 | 0.107170224 | 1.8650917E-7 | 1.499898  | 2462762 |
# | V_SIMBAD          | 11.065956  | 1.108435    | -1.46        | 15.88     | 2432423 |
# | XM_SIMBAD_FLAG    | 1.0        | 0.0         | 1            | 1         | 7207    |
# | IRflag            | 1.0827082  | 1.5544553   | 0            | 7         | 465818  |
# | Lflux_med         | 1.5206355  | 42.534527   | 1.6299028E-4 | 15720.791 | 465717  |
# | e_Lflux_med       | 0.45798847 | 23.706205   | 3.0239256E-8 | 11478.607 | 432047  |
# | Mflux_med         | 0.8862267  | 19.91887    | 8.886453E-5  | 7141.7    | 465720  |
# | e_Mflux_med       | 0.3335035  | 14.301642   | 8.423023E-10 | 6363.4277 | 432102  |
# | Nflux_med         | 0.33530098 | 18.591125   | 9.8775825E-5 | 9278.1    | 465712  |
# | e_Nflux_med       | 0.12085117 | 6.6976385   | 9.378568E-10 | 3538.2249 | 433938  |
# +-------------------+------------+-------------+--------------+-----------+---------+
# 
fi

# xmatch on RA/DE (J2000) within 1.5as (as mdfc coords are different):

CAT_ASCC_SIMBAD_MDFC_XMATCH_DIST="2_xmatch_ASCC_SIMBAD_MDFC_full_dist.fits"
if [ $CAT_ASCC_SIMBAD_XMATCH_J2K -nt $CAT_ASCC_SIMBAD_MDFC_XMATCH_DIST ] || [ $CAT_MDFC_MIN -nt $CAT_ASCC_SIMBAD_MDFC_XMATCH_DIST ]
then
    stilts ${STILTS_JAVA_OPTIONS} tmatch2 matcher='sky' params='1.5' values1='_RAJ2000 _DEJ2000' values2='hmsToDegrees(RAJ2000) dmsToDegrees(DEJ2000)' find='best' join='all1' in1=$CAT_ASCC_SIMBAD_XMATCH_J2K in2=$CAT_MDFC_MIN out=$CAT_ASCC_SIMBAD_MDFC_XMATCH_DIST
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_ASCC_SIMBAD_MDFC_XMATCH_DIST cmd='progress ; delcols "RAJ2000 DEJ2000 Name Separation" ' out=$CAT_ASCC_SIMBAD_MDFC_XMATCH_DIST
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_ASCC_SIMBAD_MDFC_XMATCH_DIST > $CAT_ASCC_SIMBAD_MDFC_XMATCH_DIST.stats.log

# Total Rows: 2500764
# +-------------------+------------+-------------+--------------+-----------+---------+
# | column            | mean       | stdDev      | min          | max       | good    |
# +-------------------+------------+-------------+--------------+-----------+---------+
# | _RAJ2000          | 188.87296  | 100.47335   | 3.4094515E-4 | 359.99988 | 2500764 |
# | _DEJ2000          | -3.3170354 | 41.25193    | -89.88966    | 89.911354 | 2500764 |
# | pmRA              | -1.754506  | 32.229416   | -4417.35     | 6773.35   | 2500764 |
# | pmDE              | -6.086718  | 30.385551   | -5813.7      | 10308.08  | 2500764 |
# | Vmag              | 11.069625  | 1.1319602   | -1.121       | 16.746    | 2500626 |
# | ra_a_91           | 188.87297  | 100.47336   | 2.8266E-4    | 359.9999  | 2500764 |
# | de_a_91           | -3.3170204 | 41.251934   | -89.88964    | 89.91136  | 2500764 |
# | ASCC_GROUP_SIZE   | 2.0283375  | 0.17042844  | 2            | 4         | 31760   |
# | SIMBAD_GROUP_SIZE | 2.0052671  | 0.07238353  | 2            | 3         | 11961   |
# | main_id           |            |             |              |           | 2462762 |
# | sp_type           |            |             |              |           | 508464  |
# | main_type         |            |             |              |           | 2462762 |
# | other_types       |            |             |              |           | 2462736 |
# | V                 | 11.067058  | 1.1076136   | -1.46        | 15.79     | 2434987 |
# | XM_SIMBAD_SEP     | 0.09782921 | 0.107170224 | 1.8650917E-7 | 1.499898  | 2462762 |
# | V_SIMBAD          | 11.065956  | 1.108435    | -1.46        | 15.88     | 2432423 |
# | XM_SIMBAD_FLAG    | 1.0        | 0.0         | 1            | 1         | 7207    |
# | IRflag            | 1.0827082  | 1.5544553   | 0            | 7         | 465818  |
# | Lflux_med         | 1.5206355  | 42.534527   | 1.6299028E-4 | 15720.791 | 465717  |
# | e_Lflux_med       | 0.45798847 | 23.706205   | 3.0239256E-8 | 11478.607 | 432047  |
# | Mflux_med         | 0.8862267  | 19.91887    | 8.886453E-5  | 7141.7    | 465720  |
# | e_Mflux_med       | 0.3335035  | 14.301642   | 8.423023E-10 | 6363.4277 | 432102  |
# | Nflux_med         | 0.33530098 | 18.591125   | 9.8775825E-5 | 9278.1    | 465712  |
# | e_Nflux_med       | 0.12085117 | 6.6976385   | 9.378568E-10 | 3538.2249 | 433938  |
# +-------------------+------------+-------------+--------------+-----------+---------+
# 
fi

# Use xmatch(COO) for MDFC:
CAT_ASCC_SIMBAD_MDFC_XMATCH=$CAT_ASCC_SIMBAD_MDFC_XMATCH_DIST


# 3. Prepare candidates: columns, sort, then filters:
FULL_CANDIDATES="3_xmatch_ASCC_SIMBAD_MDFC_full_cols.fits"
if [ $CAT_ASCC_SIMBAD_MDFC_XMATCH -nt $FULL_CANDIDATES ]
then
    # note: merge (main_type,other_types) as other_types can be NULL:
    # convert XM_SIMBAD_FLAG (bad xm on 91 vs 2000?) into GROUP_SIZE_3:
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_ASCC_SIMBAD_MDFC_XMATCH cmd='progress ; addcol -after sp_type -ucd OBJ_TYPES OTYPES NULL_main_type?\"\":replaceAll(concat(\",\",main_type,\",\",replaceAll(other_types,\"[|]\",\",\"),\",\"),\"\ \",\"\") ; delcols "main_type other_types Vmag V" ; addcol -before _RAJ2000 -ucd POS_EQ_RA_MAIN RAJ2000 degreesToHms(_RAJ2000,6) ; addcol -after RAJ2000 -ucd POS_EQ_DEC_MAIN DEJ2000 degreesToDms(_DEJ2000,6) ; colmeta -ucd POS_EQ_PMRA pmRA ; colmeta -ucd POS_EQ_PMDEC pmDE ; colmeta -name MAIN_ID -ucd ID_MAIN main_id ; colmeta -name SP_TYPE -ucd SPECT_TYPE_MK sp_type ; addcol -after XM_SIMBAD_SEP -ucd GROUP_SIZE GROUP_SIZE_3 max((!NULL_ASCC_GROUP_SIZE?ASCC_GROUP_SIZE:0),(!NULL_SIMBAD_GROUP_SIZE?SIMBAD_GROUP_SIZE:0))+(!NULL_XM_SIMBAD_FLAG?1:0) ; sort _DEJ2000 ; delcols "_RAJ2000 _DEJ2000 ASCC_GROUP_SIZE SIMBAD_GROUP_SIZE XM_SIMBAD_FLAG" ' out=$FULL_CANDIDATES
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$FULL_CANDIDATES > $FULL_CANDIDATES.stats.log

# Total Rows: 2500764
# +---------------+-------------+------------+--------------+-----------+---------+
# | column        | mean        | stdDev     | min          | max       | good    |
# +---------------+-------------+------------+--------------+-----------+---------+
# | RAJ2000       |             |            |              |           | 2500764 |
# | DEJ2000       |             |            |              |           | 2500764 |
# | pmRA          | -1.754506   | 32.229416  | -4417.35     | 6773.35   | 2500764 |
# | pmDE          | -6.086718   | 30.385551  | -5813.7      | 10308.08  | 2500764 |
# | ra_a_91       | 188.87297   | 100.47336  | 2.8266E-4    | 359.9999  | 2500764 |
# | de_a_91       | -3.3170204  | 41.251934  | -89.88964    | 89.91136  | 2500764 |
# | MAIN_ID       |             |            |              |           | 2428771 |
# | SP_TYPE       |             |            |              |           | 497178  |
# | OTYPES        |             |            |              |           | 2428771 |
# | XM_SIMBAD_SEP | 0.108560905 | 0.12304193 | 8.73148E-6   | 1.382638  | 2428771 |
# | GROUP_SIZE_3  | 0.026821403 | 0.2305579  | 0            | 4         | 2500764 |
# | IRflag        | 1.0827082   | 1.5544553  | 0            | 7         | 465818  |
# | Lflux_med     | 1.5206355   | 42.534527  | 1.6299028E-4 | 15720.791 | 465717  |
# | e_Lflux_med   | 0.45798847  | 23.706205  | 3.0239256E-8 | 11478.607 | 432047  |
# | Mflux_med     | 0.8862267   | 19.91887   | 8.886453E-5  | 7141.7    | 465720  |
# | e_Mflux_med   | 0.3335035   | 14.301642  | 8.423023E-10 | 6363.4277 | 432102  |
# | Nflux_med     | 0.33530098  | 18.591125  | 9.8775825E-5 | 9278.1    | 465712  |
# | e_Nflux_med   | 0.12085117  | 6.6976385  | 9.378568E-10 | 3538.2249 | 433938  |
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

# Total Rows: 508464
# +---------------+-------------+-------------+--------------+-----------+--------+
# | column        | mean        | stdDev      | min          | max       | good   |
# +---------------+-------------+-------------+--------------+-----------+--------+
# | RAJ2000       |             |             |              |           | 508464 |
# | DEJ2000       |             |             |              |           | 508464 |
# | pmRA          | -1.6801732  | 53.189293   | -4417.35     | 6773.35   | 508464 |
# | pmDE          | -8.926331   | 50.0674     | -5813.06     | 10308.08  | 508464 |
# | ra_a_91       | 191.13023   | 100.668076  | 2.8266E-4    | 359.99915 | 508464 |
# | de_a_91       | -0.35351607 | 40.475056   | -89.831245   | 89.77407  | 508464 |
# | MAIN_ID       |             |             |              |           | 508464 |
# | SP_TYPE       |             |             |              |           | 508464 |
# | OTYPES        |             |             |              |           | 508464 |
# | XM_SIMBAD_SEP | 0.043346085 | 0.066441275 | 1.8650917E-7 | 1.497625  | 508464 |
# | GROUP_SIZE_3  | 0.046644405 | 0.31546915  | 0            | 5         | 508464 |
# | V_SIMBAD      | 9.719448    | 1.2888868   | -1.46        | 15.88     | 499906 |
# | IRflag        | 1.0780823   | 1.5560942   | 0            | 7         | 449423 |
# | Lflux_med     | 1.5545044   | 43.29874    | 1.6299028E-4 | 15720.791 | 449325 |
# | e_Lflux_med   | 0.46855867  | 24.129604   | 3.0239256E-8 | 11478.607 | 416933 |
# | Mflux_med     | 0.9054484   | 20.274939   | 8.886453E-5  | 7141.7    | 449329 |
# | e_Mflux_med   | 0.34119678  | 14.555591   | 8.423023E-10 | 6363.4277 | 416987 |
# | Nflux_med     | 0.34295824  | 18.921856   | 9.8775825E-5 | 9278.1    | 449321 |
# | e_Nflux_med   | 0.12372045  | 6.8172946   | 9.378568E-10 | 3538.2249 | 418763 |
# +---------------+-------------+-------------+--------------+-----------+--------+
# 

    # Get all neightbours within 3 as:
    # use all to get all objects within 3 as in ASCC FULL for every ASCC BRIGHT source at epoch 1991.25
    stilts ${STILTS_JAVA_OPTIONS} tmatch2 matcher='sky' params='3.0' values1='ra_a_91 de_a_91' values2='ra_a_91 de_a_91' find='all' join='all1' fixcols='all' suffix2='' in1=$CATALOG_BRIGHT_0 in2=$FULL_CANDIDATES out=$CATALOG_BRIGHT
    # remove duplicates (on de_a_91)
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CATALOG_BRIGHT cmd='progress ; sort de_a_91 ; uniq de_a_91 ; delcols "*_1 ra_a_91 de_a_91 GroupID GroupSize Separation" ' out=$CATALOG_BRIGHT
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CATALOG_BRIGHT > ${CATALOG_BRIGHT}.stats.log

# Total Rows: 518624
# +---------------+------------+------------+--------------+-----------+--------+
# | column        | mean       | stdDev     | min          | max       | good   |
# +---------------+------------+------------+--------------+-----------+--------+
# | RAJ2000       |            |            |              |           | 518624 |
# | DEJ2000       |            |            |              |           | 518624 |
# | pmRA          | -1.671666  | 53.731403  | -4417.35     | 6773.35   | 518624 |
# | pmDE          | -9.020842  | 50.7888    | -5813.06     | 10308.08  | 518624 |
# | MAIN_ID       |            |            |              |           | 513388 |
# | SP_TYPE       |            |            |              |           | 508464 |
# | OTYPES        |            |            |              |           | 513388 |
# | XM_SIMBAD_SEP | 0.04336556 | 0.06696961 | 1.8650917E-7 | 1.497625  | 513388 |
# | GROUP_SIZE_3  | 0.08555331 | 0.417141   | 0            | 5         | 518624 |
# | V_SIMBAD      | 9.720669   | 1.2878852  | -1.46        | 15.88     | 503965 |
# | IRflag        | 1.0780928  | 1.5560931  | 0            | 7         | 449427 |
# | Lflux_med     | 1.5546576  | 43.298584  | 1.6299028E-4 | 15720.791 | 449329 |
# | e_Lflux_med   | 0.4686577  | 24.129526  | 3.0239256E-8 | 11478.607 | 416937 |
# | Mflux_med     | 0.9055667  | 20.274897  | 8.886453E-5  | 7141.7    | 449333 |
# | e_Mflux_med   | 0.3412708  | 14.555553  | 8.423023E-10 | 6363.4277 | 416991 |
# | Nflux_med     | 0.3429848  | 18.921776  | 9.8775825E-5 | 9278.1    | 449325 |
# | e_Nflux_med   | 0.12372719 | 6.8172626  | 9.378568E-10 | 3538.2249 | 418767 |
# +---------------+------------+------------+--------------+-----------+--------+
# 
fi


# - faint: select stars WITHOUT SP_TYPE nor neighbours (not in CATALOG_BRIGHT)
CATALOG_FAINT="3_xmatch_ASCC_SIMBAD_MDFC_full_NO_sptype.vot"
if [ $FULL_CANDIDATES -nt $CATALOG_FAINT ]
then
    CATALOG_FAINT_0="3_xmatch_ASCC_SIMBAD_MDFC_full_NO_sptype_0.fits"

    # delete any MDFC columns as it should correspond to JSDC BRIGHT
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$FULL_CANDIDATES cmd='progress; delcols "ra_a_91 de_a_91 SP_TYPE IRflag Lflux_med e_Lflux_med Mflux_med e_Mflux_med Nflux_med e_Nflux_med" ' out=$CATALOG_FAINT_0
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CATALOG_FAINT_0 > ${CATALOG_FAINT_0}.stats.log

# Total Rows: 2500764
# +---------------+-------------+-------------+--------------+----------+---------+
# | column        | mean        | stdDev      | min          | max      | good    |
# +---------------+-------------+-------------+--------------+----------+---------+
# | RAJ2000       |             |             |              |          | 2500764 |
# | DEJ2000       |             |             |              |          | 2500764 |
# | pmRA          | -1.754506   | 32.229416   | -4417.35     | 6773.35  | 2500764 |
# | pmDE          | -6.086718   | 30.385551   | -5813.7      | 10308.08 | 2500764 |
# | MAIN_ID       |             |             |              |          | 2462762 |
# | OTYPES        |             |             |              |          | 2462762 |
# | XM_SIMBAD_SEP | 0.09782921  | 0.107170224 | 1.8650917E-7 | 1.499898 | 2462762 |
# | GROUP_SIZE_3  | 0.028648445 | 0.24030682  | 0            | 5        | 2500764 |
# | V_SIMBAD      | 11.065956   | 1.108435    | -1.46        | 15.88    | 2432423 |
# +---------------+-------------+-------------+--------------+----------+---------+
# 
    # use DEJ2000 (unique)
    stilts ${STILTS_JAVA_OPTIONS} tmatch2 matcher='exact' values1='DEJ2000' values2='DEJ2000' join='1not2' in1=$CATALOG_FAINT_0 in2=$CATALOG_BRIGHT out=$CATALOG_FAINT
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CATALOG_FAINT > $CATALOG_FAINT.stats.log

# Total Rows: 1982140
# +---------------+-------------+-------------+--------------+----------+---------+
# | column        | mean        | stdDev      | min          | max      | good    |
# +---------------+-------------+-------------+--------------+----------+---------+
# | RAJ2000       |             |             |              |          | 1982140 |
# | DEJ2000       |             |             |              |          | 1982140 |
# | pmRA          | -1.7761809  | 23.560997   | -3681.47     | 4147.35  | 1982140 |
# | pmDE          | -5.3190317  | 22.070175   | -5813.7      | 3203.63  | 1982140 |
# | MAIN_ID       |             |             |              |          | 1949374 |
# | OTYPES        |             |             |              |          | 1949374 |
# | XM_SIMBAD_SEP | 0.11217282  | 0.111095354 | 1.6588735E-6 | 1.499898 | 1949374 |
# | GROUP_SIZE_3  | 0.013759371 | 0.16204742  | 0            | 4        | 1982140 |
# | V_SIMBAD      | 11.41752    | 0.7209013   | 2.22         | 15.79    | 1928458 |
# +---------------+-------------+-------------+--------------+----------+---------+
# 
fi


# - test: select only stars with neighbours (GROUP_SIZE_3 > 0)
CATALOG_TEST="3_xmatch_ASCC_SIMBAD_MDFC_full_test.vot"
if [ $FULL_CANDIDATES -nt $CATALOG_TEST ]
then
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$FULL_CANDIDATES cmd='progress; select GROUP_SIZE_3>0 ; delcols "ra_a_91 de_a_91 IRflag Lflux_med e_Lflux_med Mflux_med e_Mflux_med Nflux_med e_Nflux_med" ' out=$CATALOG_TEST
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CATALOG_TEST > $CATALOG_TEST.stats.log

# Total Rows: 37101
# +---------------+-------------+------------+--------------+----------+-------+
# | column        | mean        | stdDev     | min          | max      | good  |
# +---------------+-------------+------------+--------------+----------+-------+
# | RAJ2000       |             |            |              |          | 37101 |
# | DEJ2000       |             |            |              |          | 37101 |
# | pmRA          | -1.05295    | 87.421394  | -3678.16     | 2183.43  | 37101 |
# | pmDE          | -11.266943  | 81.37067   | -3567.01     | 1806.86  | 37101 |
# | MAIN_ID       |             |            |              |          | 30740 |
# | SP_TYPE       |             |            |              |          | 11386 |
# | OTYPES        |             |            |              |          | 30740 |
# | XM_SIMBAD_SEP | 0.103412405 | 0.25830042 | 1.8650917E-7 | 1.499898 | 30740 |
# | GROUP_SIZE_3  | 1.9310261   | 0.4678412  | 1            | 5        | 37101 |
# | V_SIMBAD      | 9.780439    | 1.5308161  | -0.1         | 15.88    | 23697 |
# +---------------+-------------+------------+--------------+----------+-------+
# 
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


