#!/bin/bash
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************

# Note on pm correction: use 'pmRa/cos(DE)' not directly 'pmRa' as pmRa(catalog) is expressed as pmRa*cos(DE)!

echo "processing @ date: `date -u`"

# Use large heap + CMS GC:
STILTS_JAVA_OPTIONS="-Xms4g -Xmx8g"
# use /home/... as /tmp is full:
# STILTS_JAVA_OPTIONS="$STILTS_JAVA_OPTIONS -Djava.io.tmpdir=/home/bourgesl/tmp/ -disk"
# other options:   -verbose -debug

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
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_SIMBAD cmd='progress; select !NULL_sp_type||(!NULL_main_type&&(main_type.contains(\"*\")||main_type.equals(\"Star\")))||(!NULL_other_types&&other_types.contains(\"*\")) ' out=$CAT_SIMBAD_OBJ_STAR
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_SIMBAD_OBJ_STAR > $CAT_SIMBAD_OBJ_STAR.stats.log

# Total Rows: 12302242
# +---------------+-------------+------------+------------+-------------+----------+
# | column        | mean        | stdDev     | min        | max         | good     |
# +---------------+-------------+------------+------------+-------------+----------+
# | main_id       |             |            |            |             | 12302242 |
# | ra            | 198.98378   | 97.95351   | 0.0        | 359.99997   | 12302242 |
# | dec           | -7.064629   | 39.405483  | -89.88967  | 89.89818    | 12302242 |
# | coo_err_maj   | 0.022067549 | 2.4494498  | 0.0        | 3380.0      | 11266150 |
# | coo_err_min   | 0.020178165 | 1.7181944  | 0.0        | 1930.0      | 11266150 |
# | coo_err_angle | 95.62668    | 25.611408  | 0.0        | 179.0       | 12302242 |
# | nbref         | 2.5262625   | 12.09377   | 0          | 12733       | 12302242 |
# | ra_sexa       |             |            |            |             | 12302242 |
# | dec_sexa      |             |            |            |             | 12302242 |
# | coo_qual      |             |            |            |             | 12302242 |
# | coo_bibcode   |             |            |            |             | 12255850 |
# | main_type     |             |            |            |             | 12302242 |
# | other_types   |             |            |            |             | 12302151 |
# | radvel        | 21523.771   | 60876.496  | -58663.2   | 293760.8    | 5136869  |
# | radvel_err    | 110.04608   | 85893.11   | -1798754.8 | 1.8011624E8 | 4720233  |
# | redshift      | 0.15766798  | 0.5294984  | -9.99898   | 8.92        | 5136905  |
# | redshift_err  | 3.670725E-4 | 0.28650856 | -6.0       | 600.8031    | 4720233  |
# | sp_type       |             |            |            |             | 1024614  |
# | morph_type    |             |            |            |             | 77455    |
# | plx           | 1.7859637   | 3.5592594  | -0.418     | 768.0665    | 8079809  |
# | plx_err       | 0.14229146  | 0.27736816 | 0.0        | 81.0        | 8078305  |
# | pmra          | -1.8384652  | 29.937449  | -8118.9    | 6765.995    | 9535278  |
# | pmdec         | -5.683915   | 28.465357  | -5817.8    | 10362.394   | 9535278  |
# | pm_err_maj    | 0.2167751   | 1.8187616  | 0.0        | 3852.0      | 9532767  |
# | pm_err_min    | 0.1933665   | 5.3743486  | 0.0        | 16383.0     | 9532764  |
# | pm_err_pa     | 909.59265   | 5110.0493  | 0          | 32767       | 9777918  |
# | size_maj      | 0.285897    | 6.269658   | 0.0        | 3438.0      | 801491   |
# | size_min      | 0.21931434  | 4.8477373  | 0.0        | 3438.0      | 801488   |
# | size_angle    | 50.868484   | 342.5096   | -89        | 32767       | 789677   |
# | B             | 13.613509   | 2.802858   | -1.46      | 45.38       | 4425148  |
# | V             | 13.669959   | 3.4456966  | -1.46      | 31.92       | 5057356  |
# | R             | 14.030344   | 2.2288759  | -1.46      | 50.0        | 1639126  |
# | J             | 11.4784775  | 2.440421   | -3.0       | 29.25       | 7053858  |
# | H             | 10.790859   | 2.44296    | -3.73      | 30.397      | 7035874  |
# | K             | 10.56634    | 2.5069788  | -4.05      | 27.59       | 7056403  |
# | u             | 20.380487   | 1.9708546  | 0.0        | 33.273      | 1232483  |
# | g             | 16.937086   | 3.1061625  | 0.0        | 32.368      | 3051327  |
# | r             | 16.158766   | 3.0865455  | 0.003      | 31.197      | 3100825  |
# | i             | 15.488433   | 3.3725872  | 0.0        | 30.516      | 2398594  |
# | z             | 17.87247    | 2.009043   | 0.0        | 31.741      | 1293285  |
# +---------------+-------------+------------+------------+-------------+----------+

    # filter on V > 16 (not in TYCHO2)
    MAG_FILTER="V<=16.0"
    
    CAT_SIMBAD_OBJ_Vgt16="1_obj_star_Vgt16_$CAT_SIMBAD"
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_SIMBAD_OBJ_STAR cmd="progress; select !(${MAG_FILTER}) " out=$CAT_SIMBAD_OBJ_Vgt16
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_SIMBAD_OBJ_Vgt16 > $CAT_SIMBAD_OBJ_Vgt16.stats.log

# Total Rows: 8535727
# +---------------+-------------+------------+------------+-------------+---------+
# | column        | mean        | stdDev     | min        | max         | good    |
# +---------------+-------------+------------+------------+-------------+---------+
# | main_id       |             |            |            |             | 8535727 |
# | ra            | 203.9913    | 96.876     | 0.0        | 359.99997   | 8535727 |
# | dec           | -7.3526874  | 38.46633   | -89.86853  | 89.89818    | 8535727 |
# | coo_err_maj   | 0.029383436 | 2.923364   | 0.0        | 3380.0      | 7543272 |
# | coo_err_min   | 0.026643919 | 1.999315   | 0.0        | 1930.0      | 7543272 |
# | coo_err_angle | 97.82546    | 29.721815  | 0.0        | 179.0       | 8535727 |
# | nbref         | 2.384658    | 8.333707   | 0          | 5279        | 8535727 |
# | ra_sexa       |             |            |            |             | 8535727 |
# | dec_sexa      |             |            |            |             | 8535727 |
# | coo_qual      |             |            |            |             | 8535727 |
# | coo_bibcode   |             |            |            |             | 8506298 |
# | main_type     |             |            |            |             | 8535727 |
# | other_types   |             |            |            |             | 8535639 |
# | radvel        | 44476.832   | 81491.44   | -58663.2   | 293760.8    | 2484259 |
# | radvel_err    | 247.57417   | 129338.77  | -1798754.8 | 1.8011624E8 | 2081708 |
# | redshift      | 0.32589334  | 0.7244631  | -9.99898   | 8.92        | 2484295 |
# | redshift_err  | 8.258169E-4 | 0.43142772 | -6.0       | 600.8031    | 2081708 |
# | sp_type       |             |            |            |             | 434233  |
# | morph_type    |             |            |            |             | 74757   |
# | plx           | 1.4720036   | 3.6993442  | -0.418     | 546.9759    | 4476225 |
# | plx_err       | 0.2301827   | 0.3303279  | 6.0E-4     | 81.0        | 4474729 |
# | pmra          | -1.9129038  | 31.622643  | -8118.9    | 6765.995    | 5819859 |
# | pmdec         | -5.989262   | 30.004957  | -5708.614  | 10362.394   | 5819859 |
# | pm_err_maj    | 0.29040834  | 2.2878487  | 0.0        | 3852.0      | 5817991 |
# | pm_err_min    | 0.2560808   | 6.867452   | 0.0        | 16383.0     | 5817988 |
# | pm_err_pa     | 1397.3536   | 6404.125   | 0          | 32767       | 6060356 |
# | size_maj      | 0.27598047  | 6.264802   | 0.0        | 3438.0      | 792400  |
# | size_min      | 0.21034837  | 4.828325   | 0.0        | 3438.0      | 792398  |
# | size_angle    | 47.769577   | 207.56409  | -89        | 32767       | 780788  |
# | B             | 18.081186   | 2.2392578  | 3.4        | 45.38       | 847412  |
# | V             | 18.835817   | 1.6898685  | 16.001     | 31.92       | 1290841 |
# | R             | 15.726154   | 2.2796097  | 0.0        | 50.0        | 631596  |
# | J             | 12.668099   | 2.478327   | -2.3       | 29.25       | 3514637 |
# | H             | 11.682403   | 2.6606476  | -3.22      | 30.397      | 3496470 |
# | K             | 11.334819   | 2.8236716  | -3.51      | 27.59       | 3520265 |
# | u             | 20.472343   | 1.8688333  | 0.0        | 33.273      | 1208114 |
# | g             | 18.828987   | 1.8976971  | 0.0        | 32.368      | 1981546 |
# | r             | 17.989437   | 1.9557776  | 0.004      | 31.197      | 2030751 |
# | i             | 18.028997   | 2.035946   | 0.0        | 30.516      | 1339379 |
# | z             | 17.974072   | 1.8992294  | 0.0        | 30.878      | 1263887 |
# +---------------+-------------+------------+------------+-------------+---------+

    # filter on V <= 16 (max in TYCHO2)
    # convert SIMBAD coords to epoch 1991.25:
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_SIMBAD_OBJ_STAR cmd="progress; select ${MAG_FILTER} ; addcol ra_s_91 'ra  + ((NULL_pmra ) ? 0.0 : (1991.25 - 2000.0) * (0.001 * pmra / 3600.0) / cos(degreesToRadians(dec)))' ; addcol de_s_91 'dec + ((NULL_pmdec) ? 0.0 : (1991.25 - 2000.0) * (0.001 * pmdec / 3600.0))' " out=$CAT_SIMBAD_OBJ
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_SIMBAD_OBJ > $CAT_SIMBAD_OBJ.stats.log

# Total Rows: 3766515
# +---------------+--------------+--------------+--------------+-----------+---------+
# | column        | mean         | stdDev       | min          | max       | good    |
# +---------------+--------------+--------------+--------------+-----------+---------+
# | main_id       |              |              |              |           | 3766515 |
# | ra            | 187.63565    | 99.42351     | 2.043984E-4  | 359.9999  | 3766515 |
# | dec           | -6.411827    | 41.447723    | -89.88967    | 89.83233  | 3766515 |
# | coo_err_maj   | 0.0072441422 | 0.91666603   | 0.0          | 180.0     | 3722878 |
# | coo_err_min   | 0.007077293  | 0.913468     | 0.0          | 180.0     | 3722878 |
# | coo_err_angle | 90.64377     | 10.233584    | 0.0          | 179.0     | 3766515 |
# | nbref         | 2.847168     | 17.89345     | 0            | 12733     | 3766515 |
# | ra_sexa       |              |              |              |           | 3766515 |
# | dec_sexa      |              |              |              |           | 3766515 |
# | coo_qual      |              |              |              |           | 3766515 |
# | coo_bibcode   |              |              |              |           | 3749552 |
# | main_type     |              |              |              |           | 3766515 |
# | other_types   |              |              |              |           | 3766512 |
# | radvel        | 27.451525    | 1345.609     | -9998.95     | 272958.66 | 2652610 |
# | radvel_err    | 1.5410199    | 241.99826    | -9.99        | 359750.97 | 2638525 |
# | redshift      | 1.1694223E-4 | 0.008754879  | -0.0328148   | 3.62      | 2652610 |
# | redshift_err  | 5.1385578E-6 | 8.0721924E-4 | -3.33E-5     | 1.2       | 2638525 |
# | sp_type       |              |              |              |           | 590381  |
# | morph_type    |              |              |              |           | 2698    |
# | plx           | 2.1759522    | 3.3362768    | -0.001       | 768.0665  | 3603584 |
# | plx_err       | 0.033152778  | 0.12436181   | 0.0          | 36.13     | 3603576 |
# | pmra          | -1.7218641   | 27.087421    | -4339.85     | 6765.995  | 3715419 |
# | pmdec         | -5.205618    | 25.862953    | -5817.8      | 10362.394 | 3715419 |
# | pm_err_maj    | 0.10145253   | 0.51873666   | 0.001        | 89.8      | 3714776 |
# | pm_err_min    | 0.095144935  | 0.49075395   | 0.001        | 85.4      | 3714776 |
# | pm_err_pa     | 114.44665    | 895.6737     | 0            | 32767     | 3717562 |
# | size_maj      | 1.1502523    | 6.6225405    | 0.0          | 360.0     | 9091    |
# | size_min      | 1.0009001    | 6.2655044    | 0.0          | 360.0     | 9090    |
# | size_angle    | 323.06885    | 2561.7488    | -87          | 32767     | 8889    |
# | B             | 12.555308    | 1.6375624    | -1.46        | 23.071    | 3577736 |
# | V             | 11.899542    | 1.6379995    | -1.46        | 16.0      | 3766515 |
# | R             | 12.967281    | 1.3753715    | -1.46        | 24.75     | 1007530 |
# | J             | 10.297119    | 1.723231     | -3.0         | 18.26     | 3539221 |
# | H             | 9.910131     | 1.8192446    | -3.73        | 18.23     | 3539404 |
# | K             | 9.8013115    | 1.8523401    | -4.05        | 18.22     | 3536138 |
# | u             | 15.826622    | 1.4658209    | 1.25         | 31.431    | 24369   |
# | g             | 13.432733    | 1.3923575    | 0.008        | 25.118    | 1069781 |
# | r             | 12.684579    | 1.3846256    | 0.003        | 27.5      | 1070074 |
# | i             | 12.275885    | 1.4260488    | 0.004        | 27.98     | 1059215 |
# | z             | 13.504446    | 1.7217093    | 0.004        | 31.741    | 29398   |
# | ra_s_91       | 187.63567    | 99.42351     | 2.0395867E-4 | 359.99994 | 3766515 |
# | de_s_91       | -6.411814    | 41.447727    | -89.889656   | 89.83232  | 3766515 |
# +---------------+--------------+--------------+--------------+-----------+---------+
fi


# SIMBAD: internal match within 3 as to identify inner groups and filter columns:
# (main_id, ra/dec 1991) only to identify groups
CAT_SIMBAD_MIN="1_min_$CAT_SIMBAD_OBJ"
if [ $CAT_SIMBAD_OBJ -nt $CAT_SIMBAD_MIN ]
then
    CAT_SIMBAD_MIN_0="1_min_0_$CAT_SIMBAD_OBJ"

    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_SIMBAD_OBJ cmd='progress ; keepcols "ra_s_91 de_s_91 main_id" ' out=$CAT_SIMBAD_MIN_0
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_SIMBAD_MIN_0 > $CAT_SIMBAD_MIN_0.stats.log

# Total Rows: 3766515
# +---------+-----------+-----------+--------------+-----------+---------+
# | column  | mean      | stdDev    | min          | max       | good    |
# +---------+-----------+-----------+--------------+-----------+---------+
# | ra_s_91 | 187.63567 | 99.42351  | 2.0395867E-4 | 359.99994 | 3766515 |
# | de_s_91 | -6.411814 | 41.447727 | -89.889656   | 89.83232  | 3766515 |
# | main_id |           |           |              |           | 3766515 |
# +---------+-----------+-----------+--------------+-----------+---------+

    # identify inner groups in SIMBAD:
    stilts ${STILTS_JAVA_OPTIONS} tmatch1 matcher='sky' params='3.0' values='ra_s_91 de_s_91' action='identify' in=$CAT_SIMBAD_MIN_0 out=$CAT_SIMBAD_MIN
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_SIMBAD_MIN cmd='progress ; colmeta -name SIMBAD_GROUP_SIZE GroupSize ; delcols "GroupID" ' out=$CAT_SIMBAD_MIN
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_SIMBAD_MIN > $CAT_SIMBAD_MIN.stats.log

# Total Rows: 3766515
# +-------------------+-----------+-----------+--------------+-----------+---------+
# | column            | mean      | stdDev    | min          | max       | good    |
# +-------------------+-----------+-----------+--------------+-----------+---------+
# | ra_s_91           | 187.63567 | 99.42351  | 2.0395867E-4 | 359.99994 | 3766515 |
# | de_s_91           | -6.411814 | 41.447727 | -89.889656   | 89.83232  | 3766515 |
# | main_id           |           |           |              |           | 3766515 |
# | SIMBAD_GROUP_SIZE | 3.3709695 | 9.834469  | 2            | 164       | 70712   |
# +-------------------+-----------+-----------+--------------+-----------+---------+
fi


# SIMBAD: column subset for candidates
CAT_SIMBAD_SUB="1_sub_$CAT_SIMBAD_OBJ"
if [ $CAT_SIMBAD_OBJ -nt $CAT_SIMBAD_SUB ]
then
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_SIMBAD_OBJ cmd='progress ; colmeta -name ra_s ra ; colmeta -name de_s dec ; keepcols "ra_s de_s ra_s_91 de_s_91 main_id sp_type main_type other_types V" ' out=$CAT_SIMBAD_SUB
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_SIMBAD_SUB > $CAT_SIMBAD_SUB.stats.log

# Total Rows: 3766515
# +-------------+-----------+-----------+--------------+-----------+---------+
# | column      | mean      | stdDev    | min          | max       | good    |
# +-------------+-----------+-----------+--------------+-----------+---------+
# | ra_s        | 187.63565 | 99.42351  | 2.043984E-4  | 359.9999  | 3766515 |
# | de_s        | -6.411827 | 41.447723 | -89.88967    | 89.83233  | 3766515 |
# | ra_s_91     | 187.63567 | 99.42351  | 2.0395867E-4 | 359.99994 | 3766515 |
# | de_s_91     | -6.411814 | 41.447727 | -89.889656   | 89.83232  | 3766515 |
# | main_id     |           |           |              |           | 3766515 |
# | sp_type     |           |           |              |           | 590381  |
# | main_type   |           |           |              |           | 3766515 |
# | other_types |           |           |              |           | 3766512 |
# | V           | 11.899542 | 1.6379995 | -1.46        | 16.0      | 3766515 |
# +-------------+-----------+-----------+--------------+-----------+---------+
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
# | ra_s_91           | 188.50204  | 100.37165   | 2.7943638E-4 | 359.99994 | 2457305 |
# | de_s_91           | -3.5502443 | 41.38819    | -89.889656   | 89.83232  | 2457305 |
# | main_id           |            |             |              |           | 2457305 |
# | SIMBAD_GROUP_SIZE | 2.5459611  | 1.042172    | 2            | 164       | 33561   |
# | GroupID           | 3767.4526  | 2174.6858   | 1            | 7532      | 15106   |
# | GroupSize         | 2.008341   | 0.090947695 | 2            | 3         | 15106   |
# | Separation        | 0.10462015 | 0.15149729  | 1.8650917E-7 | 2.9982488 | 2457305 |
# +-------------------+------------+-------------+--------------+-----------+---------+

    # Only consider groups between ASCC x SIMBAD within 3as:
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_ASCC_SIMBAD_GRP_0 cmd='progress ; delcols "ra_s_91 de_s_91 main_id SIMBAD_GROUP_SIZE GroupID Separation" ; colmeta -name SIMBAD_GROUP_SIZE GroupSize ' out=$CAT_ASCC_SIMBAD_GRP
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_ASCC_SIMBAD_GRP > $CAT_ASCC_SIMBAD_GRP.stats.log

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
# | SIMBAD_GROUP_SIZE | 2.008341   | 0.090947695 | 2            | 3         | 15106   |
# +-------------------+------------+-------------+--------------+-----------+---------+
fi


# 2. Crossmatch ASCC / SIMBAD within 1.5 as to get both DATA
CAT_ASCC_SIMBAD_XMATCH="2_xmatch_ASCC_SIMBAD_full.fits"
if [ $CAT_ASCC_SIMBAD_GRP -nt $CAT_ASCC_SIMBAD_XMATCH ] || [ $CAT_SIMBAD_SUB -nt $CAT_ASCC_SIMBAD_XMATCH ]
then
    # use best to have the best ASCC x SIMBAD object: 
    # Use V-Vmag in matcher (up to 0.5 mag), note Separation becomes sqrt((dist/max(dist))^2+(delta_mag/max(mag))^2)
    
    # xmatch on COO(1991.25):
    # note: simbad may have multiple entries (children) for X, X A, X B so main_id & sp_type may differ.
    stilts ${STILTS_JAVA_OPTIONS} tmatch2 matcher='sky+1d' params='1.5 0.5' values1='ra_a_91 de_a_91 Vmag' values2='ra_s_91 de_s_91 V' find='best' join='all1' in1=$CAT_ASCC_SIMBAD_GRP in2=$CAT_SIMBAD_SUB out=$CAT_ASCC_SIMBAD_XMATCH 
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_ASCC_SIMBAD_XMATCH cmd='progress ; addcol XM_SIMBAD_SEP_2000 "skyDistanceDegrees(_RAJ2000,_DEJ2000,ra_s,de_s) * 3600.0" ; addcol XM_SIMBAD_SEP_91 "skyDistanceDegrees(ra_a_91,de_a_91,ra_s_91,de_s_91) * 3600.0" ; delcols "ra_s de_s ra_s_91 de_s_91" ; colmeta -name XM_SIMBAD_SEP -ucd XM_SIMBAD_SEP Separation ' out=$CAT_ASCC_SIMBAD_XMATCH
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_ASCC_SIMBAD_XMATCH > $CAT_ASCC_SIMBAD_XMATCH.stats.log

# Total Rows: 2500764
# +--------------------+-------------+-------------+--------------+-----------+---------+
# | column             | mean        | stdDev      | min          | max       | good    |
# +--------------------+-------------+-------------+--------------+-----------+---------+
# | _RAJ2000           | 188.87296   | 100.47335   | 3.4094515E-4 | 359.99988 | 2500764 |
# | _DEJ2000           | -3.3170354  | 41.25193    | -89.88966    | 89.911354 | 2500764 |
# | pmRA               | -1.754506   | 32.229416   | -4417.35     | 6773.35   | 2500764 |
# | pmDE               | -6.086718   | 30.385551   | -5813.7      | 10308.08  | 2500764 |
# | Vmag               | 11.069625   | 1.1319602   | -1.121       | 16.746    | 2500626 |
# | ra_a_91            | 188.87297   | 100.47336   | 2.8266E-4    | 359.9999  | 2500764 |
# | de_a_91            | -3.3170204  | 41.251934   | -89.88964    | 89.91136  | 2500764 |
# | ASCC_GROUP_SIZE    | 2.0283375   | 0.17042844  | 2            | 4         | 31760   |
# | SIMBAD_GROUP_SIZE  | 2.008341    | 0.090947695 | 2            | 3         | 15106   |
# | main_id            |             |             |              |           | 2428771 |
# | sp_type            |             |             |              |           | 497178  |
# | main_type          |             |             |              |           | 2428771 |
# | other_types        |             |             |              |           | 2428770 |
# | V                  | 11.0639925  | 1.1078387   | -1.46        | 15.87     | 2428771 |
# | XM_SIMBAD_SEP      | 0.108560905 | 0.12304193  | 8.73148E-6   | 1.382638  | 2428771 |
# | XM_SIMBAD_SEP_2000 | 0.11599633  | 0.13843668  | 2.2025993E-6 | 12.224764 | 2428771 |
# | XM_SIMBAD_SEP_91   | 0.09794034  | 0.106304415 | 1.6589219E-6 | 1.4999654 | 2428771 |
# +--------------------+-------------+-------------+--------------+-----------+---------+
fi

# ~2500 rows where XM_SIMBAD_SEP_2000 > 1.5 as (mostly because of pmRa/De difference or ASCC mistake = wrong association)

# Compare crossmatch on 1991 vs 2000:
CAT_ASCC_SIMBAD_XMATCH_J2K="2_xmatch_ASCC_SIMBAD_full_J2000.fits"
if [ $CAT_ASCC_SIMBAD_XMATCH -nt $CAT_ASCC_SIMBAD_XMATCH_J2K ] || [ $CAT_SIMBAD_SUB -nt $CAT_ASCC_SIMBAD_XMATCH_J2K ]
then
    # use best to have the best ASCC x SIMBAD object: 
    # Use V-Vmag in matcher (up to 0.5 mag), note Separation becomes sqrt(dist^2+delta_mag^2)
    
    # xmatch on COO(J2000):
    stilts ${STILTS_JAVA_OPTIONS} tmatch2 matcher='sky+1d' params='1.5 0.5' values1='_RAJ2000 _DEJ2000 Vmag' values2='ra_s de_s V' find='best' join='all1' in1=$CAT_ASCC_SIMBAD_XMATCH in2=$CAT_SIMBAD_SUB out=$CAT_ASCC_SIMBAD_XMATCH_J2K 
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_ASCC_SIMBAD_XMATCH_J2K cmd='progress ; addcol XM_SIMBAD_FLAG "!main_Id_1.equals(main_Id_2)?1:NULL" ; delcols "XM_SIMBAD_SEP_91 XM_SIMBAD_SEP_2000 ra_s de_s ra_s_91 de_s_91 main_id_2 sp_type_2 main_type_2 other_types_2 V_2 Separation" ; colmeta -name main_id main_id_1 ; colmeta -name sp_type sp_type_1 ; colmeta -name main_type main_type_1 ; colmeta -name other_types other_types_1 ; colmeta -name V V_1 ' out=$CAT_ASCC_SIMBAD_XMATCH_J2K
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_ASCC_SIMBAD_XMATCH_J2K > $CAT_ASCC_SIMBAD_XMATCH_J2K.stats.log
    
# Total Rows: 2500764
# +-------------------+-------------+-------------+--------------+-----------+---------+
# | column            | mean        | stdDev      | min          | max       | good    |
# +-------------------+-------------+-------------+--------------+-----------+---------+
# | _RAJ2000          | 188.87296   | 100.47335   | 3.4094515E-4 | 359.99988 | 2500764 |
# | _DEJ2000          | -3.3170354  | 41.25193    | -89.88966    | 89.911354 | 2500764 |
# | pmRA              | -1.754506   | 32.229416   | -4417.35     | 6773.35   | 2500764 |
# | pmDE              | -6.086718   | 30.385551   | -5813.7      | 10308.08  | 2500764 |
# | Vmag              | 11.069625   | 1.1319602   | -1.121       | 16.746    | 2500626 |
# | ra_a_91           | 188.87297   | 100.47336   | 2.8266E-4    | 359.9999  | 2500764 |
# | de_a_91           | -3.3170204  | 41.251934   | -89.88964    | 89.91136  | 2500764 |
# | ASCC_GROUP_SIZE   | 2.0283375   | 0.17042844  | 2            | 4         | 31760   |
# | SIMBAD_GROUP_SIZE | 2.008341    | 0.090947695 | 2            | 3         | 15106   |
# | main_id           |             |             |              |           | 2428771 |
# | sp_type           |             |             |              |           | 497178  |
# | main_type         |             |             |              |           | 2428771 |
# | other_types       |             |             |              |           | 2428770 |
# | V                 | 11.0639925  | 1.1078387   | -1.46        | 15.87     | 2428771 |
# | XM_SIMBAD_SEP     | 0.108560905 | 0.12304193  | 8.73148E-6   | 1.382638  | 2428771 |
# | XM_SIMBAD_FLAG    | 1.0         | 0.0         | 1            | 1         | 2618    |
# +-------------------+-------------+-------------+--------------+-----------+---------+
fi


# 2. Crossmatch ASCC / SIMBAD / MDFC (SIMBAD main_id):
CAT_ASCC_SIMBAD_MDFC_XMATCH="2_xmatch_ASCC_SIMBAD_MDFC_full.fits"
if [ $CAT_ASCC_SIMBAD_XMATCH_J2K -nt $CAT_ASCC_SIMBAD_MDFC_XMATCH ] || [ $CAT_MDFC_MIN -nt $CAT_ASCC_SIMBAD_MDFC_XMATCH ]
then
    stilts ${STILTS_JAVA_OPTIONS} tmatch2 matcher='exact' values1='main_id' values2='Name' join='all1' in1=$CAT_ASCC_SIMBAD_XMATCH_J2K in2=$CAT_MDFC_MIN out=$CAT_ASCC_SIMBAD_MDFC_XMATCH
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_ASCC_SIMBAD_MDFC_XMATCH cmd='progress ; delcols "RAJ2000 DEJ2000 Name" ' out=$CAT_ASCC_SIMBAD_MDFC_XMATCH
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_ASCC_SIMBAD_MDFC_XMATCH > $CAT_ASCC_SIMBAD_MDFC_XMATCH.stats.log

# Total Rows: 2500764
# +-------------------+-------------+-------------+--------------+-----------+---------+
# | column            | mean        | stdDev      | min          | max       | good    |
# +-------------------+-------------+-------------+--------------+-----------+---------+
# | _RAJ2000          | 188.87296   | 100.47335   | 3.4094515E-4 | 359.99988 | 2500764 |
# | _DEJ2000          | -3.3170354  | 41.25193    | -89.88966    | 89.911354 | 2500764 |
# | pmRA              | -1.754506   | 32.229416   | -4417.35     | 6773.35   | 2500764 |
# | pmDE              | -6.086718   | 30.385551   | -5813.7      | 10308.08  | 2500764 |
# | Vmag              | 11.069625   | 1.1319602   | -1.121       | 16.746    | 2500626 |
# | ra_a_91           | 188.87297   | 100.47336   | 2.8266E-4    | 359.9999  | 2500764 |
# | de_a_91           | -3.3170204  | 41.251934   | -89.88964    | 89.91136  | 2500764 |
# | ASCC_GROUP_SIZE   | 2.0283375   | 0.17042844  | 2            | 4         | 31760   |
# | SIMBAD_GROUP_SIZE | 2.008341    | 0.090947695 | 2            | 3         | 15106   |
# | main_id           |             |             |              |           | 2428771 |
# | sp_type           |             |             |              |           | 497178  |
# | main_type         |             |             |              |           | 2428771 |
# | other_types       |             |             |              |           | 2428770 |
# | V                 | 11.0639925  | 1.1078387   | -1.46        | 15.87     | 2428771 |
# | XM_SIMBAD_SEP     | 0.108560905 | 0.12304193  | 8.73148E-6   | 1.382638  | 2428771 |
# | XM_SIMBAD_FLAG    | 1.0         | 0.0         | 1            | 1         | 2618    |
# | IRflag            | 1.0761011   | 1.5494595   | 0            | 7         | 450703  |
# | Lflux_med         | 1.4357013   | 38.40481    | 2.5337382E-4 | 15720.791 | 450614  |
# | e_Lflux_med       | 0.3838692   | 12.23474    | 3.0239256E-8 | 3939.0857 | 419024  |
# | Mflux_med         | 0.8375168   | 17.485638   | 1.2762281E-4 | 7141.7    | 450617  |
# | e_Mflux_med       | 0.29276657  | 9.398459    | 8.423023E-10 | 3063.0676 | 419080  |
# | Nflux_med         | 0.291941    | 17.0981     | 9.8775825E-5 | 9278.1    | 450610  |
# | e_Nflux_med       | 0.10133947  | 3.6229558   | 9.378568E-10 | 1230.4095 | 420610  |
# +-------------------+-------------+-------------+--------------+-----------+---------+
fi

# xmatch on RA/DE (J2000) within 1.5as (as mdfc coords are different):

CAT_ASCC_SIMBAD_MDFC_XMATCH_DIST="2_xmatch_ASCC_SIMBAD_MDFC_full_dist.fits"
if [ $CAT_ASCC_SIMBAD_XMATCH_J2K -nt $CAT_ASCC_SIMBAD_MDFC_XMATCH_DIST ] || [ $CAT_MDFC_MIN -nt $CAT_ASCC_SIMBAD_MDFC_XMATCH_DIST ]
then
    stilts ${STILTS_JAVA_OPTIONS} tmatch2 matcher='sky' params='1.5' values1='_RAJ2000 _DEJ2000' values2='hmsToDegrees(RAJ2000) dmsToDegrees(DEJ2000)' find='best' join='all1' in1=$CAT_ASCC_SIMBAD_XMATCH_J2K in2=$CAT_MDFC_MIN out=$CAT_ASCC_SIMBAD_MDFC_XMATCH_DIST
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_ASCC_SIMBAD_MDFC_XMATCH_DIST cmd='progress ; delcols "RAJ2000 DEJ2000 Name Separation" ' out=$CAT_ASCC_SIMBAD_MDFC_XMATCH_DIST
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_ASCC_SIMBAD_MDFC_XMATCH_DIST > $CAT_ASCC_SIMBAD_MDFC_XMATCH_DIST.stats.log
    
# Total Rows: 2500764
# +-------------------+-------------+-------------+--------------+-----------+---------+
# | column            | mean        | stdDev      | min          | max       | good    |
# +-------------------+-------------+-------------+--------------+-----------+---------+
# | _RAJ2000          | 188.87296   | 100.47335   | 3.4094515E-4 | 359.99988 | 2500764 |
# | _DEJ2000          | -3.3170354  | 41.25193    | -89.88966    | 89.911354 | 2500764 |
# | pmRA              | -1.754506   | 32.229416   | -4417.35     | 6773.35   | 2500764 |
# | pmDE              | -6.086718   | 30.385551   | -5813.7      | 10308.08  | 2500764 |
# | Vmag              | 11.069625   | 1.1319602   | -1.121       | 16.746    | 2500626 |
# | ra_a_91           | 188.87297   | 100.47336   | 2.8266E-4    | 359.9999  | 2500764 |
# | de_a_91           | -3.3170204  | 41.251934   | -89.88964    | 89.91136  | 2500764 |
# | ASCC_GROUP_SIZE   | 2.0283375   | 0.17042844  | 2            | 4         | 31760   |
# | SIMBAD_GROUP_SIZE | 2.008341    | 0.090947695 | 2            | 3         | 15106   |
# | main_id           |             |             |              |           | 2428771 |
# | sp_type           |             |             |              |           | 497178  |
# | main_type         |             |             |              |           | 2428771 |
# | other_types       |             |             |              |           | 2428770 |
# | V                 | 11.0639925  | 1.1078387   | -1.46        | 15.87     | 2428771 |
# | XM_SIMBAD_SEP     | 0.108560905 | 0.12304193  | 8.73148E-6   | 1.382638  | 2428771 |
# | XM_SIMBAD_FLAG    | 1.0         | 0.0         | 1            | 1         | 2618    |
# | IRflag            | 1.0827082   | 1.5544553   | 0            | 7         | 465818  |
# | Lflux_med         | 1.5206355   | 42.534527   | 1.6299028E-4 | 15720.791 | 465717  |
# | e_Lflux_med       | 0.45798847  | 23.706205   | 3.0239256E-8 | 11478.607 | 432047  |
# | Mflux_med         | 0.8862267   | 19.91887    | 8.886453E-5  | 7141.7    | 465720  |
# | e_Mflux_med       | 0.3335035   | 14.301642   | 8.423023E-10 | 6363.4277 | 432102  |
# | Nflux_med         | 0.33530098  | 18.591125   | 9.8775825E-5 | 9278.1    | 465712  |
# | e_Nflux_med       | 0.12085117  | 6.6976385   | 9.378568E-10 | 3538.2249 | 433938  |
# +-------------------+-------------+-------------+--------------+-----------+---------+
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

# Total Rows: 497178
# +---------------+-------------+-------------+--------------+-----------+--------+
# | column        | mean        | stdDev      | min          | max       | good   |
# +---------------+-------------+-------------+--------------+-----------+--------+
# | RAJ2000       |             |             |              |           | 497178 |
# | DEJ2000       |             |             |              |           | 497178 |
# | pmRA          | -1.682416   | 46.747475   | -3775.62     | 6773.35   | 497178 |
# | pmDE          | -7.9101152  | 44.84385    | -5813.06     | 10308.08  | 497178 |
# | ra_a_91       | 191.35976   | 100.58824   | 2.8266E-4    | 359.99915 | 497178 |
# | de_a_91       | -0.5936697  | 40.4593     | -89.831245   | 89.77407  | 497178 |
# | MAIN_ID       |             |             |              |           | 497178 |
# | SP_TYPE       |             |             |              |           | 497178 |
# | OTYPES        |             |             |              |           | 497178 |
# | XM_SIMBAD_SEP | 0.07265869  | 0.104659356 | 3.3558343E-5 | 1.3232833 | 497178 |
# | GROUP_SIZE_3  | 0.035321355 | 0.2654338   | 0            | 4         | 497178 |
# | IRflag        | 1.0782772   | 1.5539879   | 0            | 7         | 442619 |
# | Lflux_med     | 1.4933428   | 42.10878    | 2.5337382E-4 | 15720.791 | 442532 |
# | e_Lflux_med   | 0.42079765  | 21.810495   | 3.0239256E-8 | 11478.607 | 410786 |
# | Mflux_med     | 0.8692303   | 19.63513    | 1.2762281E-4 | 7141.7    | 442535 |
# | e_Mflux_med   | 0.31669903  | 13.877608   | 8.423023E-10 | 6363.4277 | 410844 |
# | Nflux_med     | 0.30870658  | 18.407635   | 9.8775825E-5 | 9278.1    | 442526 |
# | e_Nflux_med   | 0.11219076  | 6.6267576   | 9.378568E-10 | 3538.2249 | 412445 |
# +---------------+-------------+-------------+--------------+-----------+--------+

    # Get all neightbours within 3 as:
    # use all to get all objects within 3 as in ASCC FULL for every ASCC BRIGHT source at epoch 1991.25
    stilts ${STILTS_JAVA_OPTIONS} tmatch2 matcher='sky' params='3.0' values1='ra_a_91 de_a_91' values2='ra_a_91 de_a_91' find='all' join='all1' fixcols='all' suffix2='' in1=$CATALOG_BRIGHT_0 in2=$FULL_CANDIDATES out=$CATALOG_BRIGHT
    # remove duplicates (on de_a_91)
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CATALOG_BRIGHT cmd='progress ; sort de_a_91 ; uniq de_a_91 ; delcols "*_1 ra_a_91 de_a_91 GroupID GroupSize Separation" ' out=$CATALOG_BRIGHT
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CATALOG_BRIGHT > ${CATALOG_BRIGHT}.stats.log

# Total Rows: 505303
# +---------------+------------+------------+--------------+-----------+--------+
# | column        | mean       | stdDev     | min          | max       | good   |
# +---------------+------------+------------+--------------+-----------+--------+
# | RAJ2000       |            |            |              |           | 505303 |
# | DEJ2000       |            |            |              |           | 505303 |
# | pmRA          | -1.6710426 | 47.44962   | -3775.62     | 6773.35   | 505303 |
# | pmDE          | -7.996691  | 45.645267  | -5813.06     | 10308.08  | 505303 |
# | MAIN_ID       |            |            |              |           | 499206 |
# | SP_TYPE       |            |            |              |           | 497178 |
# | OTYPES        |            |            |              |           | 499206 |
# | XM_SIMBAD_SEP | 0.07312232 | 0.10587266 | 3.3558343E-5 | 1.3232833 | 499206 |
# | GROUP_SIZE_3  | 0.06730417 | 0.36373737 | 0            | 4         | 505303 |
# | IRflag        | 1.078279   | 1.5539861  | 0            | 7         | 442622 |
# | Lflux_med     | 1.4949911  | 42.122124  | 2.5337382E-4 | 15720.791 | 442535 |
# | e_Lflux_med   | 0.42202207 | 21.824167  | 3.0239256E-8 | 11478.607 | 410789 |
# | Mflux_med     | 0.870207   | 19.64491   | 1.2762281E-4 | 7141.7    | 442538 |
# | e_Mflux_med   | 0.3171802  | 13.880481  | 8.423023E-10 | 6363.4277 | 410847 |
# | Nflux_med     | 0.30895248 | 18.40821   | 9.8775825E-5 | 9278.1    | 442529 |
# | e_Nflux_med   | 0.11234292 | 6.627447   | 9.378568E-10 | 3538.2249 | 412448 |
# +---------------+------------+------------+--------------+-----------+--------+
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
# +---------------+-------------+------------+------------+----------+---------+
# | column        | mean        | stdDev     | min        | max      | good    |
# +---------------+-------------+------------+------------+----------+---------+
# | RAJ2000       |             |            |            |          | 2500764 |
# | DEJ2000       |             |            |            |          | 2500764 |
# | pmRA          | -1.754506   | 32.229416  | -4417.35   | 6773.35  | 2500764 |
# | pmDE          | -6.086718   | 30.385551  | -5813.7    | 10308.08 | 2500764 |
# | MAIN_ID       |             |            |            |          | 2428771 |
# | OTYPES        |             |            |            |          | 2428771 |
# | XM_SIMBAD_SEP | 0.108560905 | 0.12304193 | 8.73148E-6 | 1.382638 | 2428771 |
# | GROUP_SIZE_3  | 0.026821403 | 0.2305579  | 0          | 4        | 2500764 |
# +---------------+-------------+------------+------------+----------+---------+

    # use DEJ2000 (unique)
    stilts ${STILTS_JAVA_OPTIONS} tmatch2 matcher='exact' values1='DEJ2000' values2='DEJ2000' join='1not2' in1=$CATALOG_FAINT_0 in2=$CATALOG_BRIGHT out=$CATALOG_FAINT
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CATALOG_FAINT > $CATALOG_FAINT.stats.log

# Total Rows: 1995461
# +---------------+-------------+------------+------------+----------+---------+
# | column        | mean        | stdDev     | min        | max      | good    |
# +---------------+-------------+------------+------------+----------+---------+
# | RAJ2000       |             |            |            |          | 1995461 |
# | DEJ2000       |             |            |            |          | 1995461 |
# | pmRA          | -1.7756411  | 27.048803  | -4417.35   | 4147.35  | 1995461 |
# | pmDE          | -5.6030855  | 25.066496  | -5813.7    | 3203.63  | 1995461 |
# | MAIN_ID       |             |            |            |          | 1929565 |
# | OTYPES        |             |            |            |          | 1929565 |
# | XM_SIMBAD_SEP | 0.11772921  | 0.12548754 | 8.73148E-6 | 1.382638 | 1929565 |
# | GROUP_SIZE_3  | 0.016570106 | 0.18053955 | 0          | 4        | 1995461 |
# +---------------+-------------+------------+------------+----------+---------+
fi


# - test: select only stars with neighbours (GROUP_SIZE_3 > 0)
CATALOG_TEST="3_xmatch_ASCC_SIMBAD_MDFC_full_test.vot"
if [ $FULL_CANDIDATES -nt $CATALOG_TEST ]
then
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$FULL_CANDIDATES cmd='progress; select GROUP_SIZE_3>0 ; delcols "ra_a_91 de_a_91 IRflag Lflux_med e_Lflux_med Mflux_med e_Mflux_med Nflux_med e_Nflux_med" ' out=$CATALOG_TEST
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CATALOG_TEST > $CATALOG_TEST.stats.log

# Total Rows: 34286
# +---------------+------------+------------+------------+----------+-------+
# | column        | mean       | stdDev     | min        | max      | good  |
# +---------------+------------+------------+------------+----------+-------+
# | RAJ2000       |            |            |            |          | 34286 |
# | DEJ2000       |            |            |            |          | 34286 |
# | pmRA          | -0.8930266 | 85.707375  | -1357.93   | 2183.43  | 34286 |
# | pmDE          | -11.42293  | 81.091576  | -3567.01   | 1449.13  | 34286 |
# | MAIN_ID       |            |            |            |          | 24492 |
# | SP_TYPE       |            |            |            |          | 8749  |
# | OTYPES        |            |            |            |          | 24492 |
# | XM_SIMBAD_SEP | 0.21674211 | 0.2604048  | 8.73148E-6 | 1.382638 | 24492 |
# | GROUP_SIZE_3  | 1.9563087  | 0.32016695 | 1          | 4        | 34286 |
# +---------------+------------+------------+------------+----------+-------+
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


