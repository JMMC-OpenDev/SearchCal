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
if [ ! -e $CAT_ASCC ]; then
    # wget -O $CAT_ASCC_ORIG "http://cdsxmatch.u-strasbg.fr/QueryCat/QueryCat?catName=I%2F280B%2Fascc&mode=allsky&format=votable"
    # stilts ${STILTS_JAVA_OPTIONS} tcopy in=$CAT_ASCC_ORIG out=$CAT_ASCC
    
    wget -O $CAT_ASCC "http://vizier.u-strasbg.fr/viz-bin/asu-binfits?-source=I%2F280B%2Fascc&-out.max=unlimited&-oc.form=dec&-sort=&-c.eq=J2000&-out.add=_RAJ2000&-out.add=_DEJ2000&-out=pmRA&-out=pmDE&-out=Vmag&-out=TYC1&-out=TYC2&-out=TYC3&-out=ASCC&"

    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_ASCC > $CAT_ASCC.stats.log
fi
echo "- STATS on $CAT_ASCC :"
cat $CAT_ASCC.stats.log


#    Download full SIMBAD (xmatch):
if [ ! -e $CAT_SIMBAD_ORIG ]; then
    wget -O $CAT_SIMBAD_ORIG "http://cdsxmatch.u-strasbg.fr/QueryCat/QueryCat?catName=SIMBAD&mode=allsky&format=votable"
fi


#    Download MDFC (OCA):
if [ ! -e $CAT_MDFC ]; then
    CAT_MDFC_ZIP="mdfc-v10.zip"
    if [ ! -e $CAT_MDFC_ZIP ]; then
        wget -O $CAT_MDFC_ZIP "https://matisse.oca.eu/foswiki/pub/Main/TheMid-infraredStellarDiametersAndFluxesCompilationCatalogue%28MDFC%29/mdfc-v10.zip"
    fi
    unzip $CAT_MDFC_ZIP $CAT_MDFC
    if [ ! -e $CAT_MDFC ]; then
        echo "missing MDFC catalog"
        exit 1
    fi
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_MDFC > $CAT_MDFC.stats.log
fi
echo "- STATS on $CAT_MDFC :"
cat $CAT_MDFC.stats.log


# 1. Convert catalogs to FITS:
#    Convert SIMBAD to FITS:
CAT_SIMBAD="$CAT_SIMBAD_ORIG.fits"
if [ $CAT_SIMBAD_ORIG -nt $CAT_SIMBAD ]; then
    stilts ${STILTS_JAVA_OPTIONS} tcopy ifmt=votable in=$CAT_SIMBAD_ORIG out=$CAT_SIMBAD
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_SIMBAD > $CAT_SIMBAD.stats.log
fi
echo "- STATS on $CAT_SIMBAD :"
cat $CAT_SIMBAD.stats.log


# ASCC: filter columns: (ra/dec J2000 corrected with pm) - from epoch 1991.25
CAT_ASCC_MIN="1_min_$CAT_ASCC"
if [ $CAT_ASCC -nt $CAT_ASCC_MIN ]; then
    # convert ASCC coords J2000 to epoch 1991.25 (as in original table):
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_ASCC cmd='progress ; keepcols "_RAJ2000 _DEJ2000 pmRA pmDE Vmag ASCC" ; colmeta -ucd ID_ASCC -name ASCC_ID ASCC; addcol ra_a_91 "_RAJ2000 + (1991.25 - 2000.0) * (0.001 * pmRA / 3600.0) / cos(degreesToRadians(_DEJ2000))" ; addcol de_a_91 "_DEJ2000 + (1991.25 - 2000.0) * (0.001 * pmDE / 3600.0)" ' out=$CAT_ASCC_MIN
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_ASCC_MIN > $CAT_ASCC_MIN.stats.log
fi
echo "- STATS on $CAT_ASCC_MIN :"
cat $CAT_ASCC_MIN.stats.log


# ASCC: internal match within 3 as to identify inner groups and keep all (V defined)
CAT_ASCC_MIN_GRP="1_min_Grp_$CAT_ASCC"
if [ $CAT_ASCC_MIN -nt $CAT_ASCC_MIN_GRP ]; then
    stilts ${STILTS_JAVA_OPTIONS} tmatch1 matcher='sky' params='3.0' values='ra_a_91 de_a_91' action='identify' in=$CAT_ASCC_MIN out=$CAT_ASCC_MIN_GRP
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_ASCC_MIN_GRP cmd='progress ; colmeta -name ASCC_GROUP_SIZE GroupSize ; select !NULL_Vmag||ASCC_GROUP_SIZE>0 ; delcols "GroupID" ' out=$CAT_ASCC_MIN_GRP
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_ASCC_MIN_GRP > $CAT_ASCC_MIN_GRP.stats.log
fi
echo "- STATS on $CAT_ASCC_MIN_GRP :"
cat $CAT_ASCC_MIN_GRP.stats.log


# Filter SIMBAD objects:
CAT_SIMBAD_OBJ_STAR="1_obj_star_$CAT_SIMBAD"
CAT_SIMBAD_OBJ_Vgt16="1_obj_star_Vgt16_$CAT_SIMBAD"
CAT_SIMBAD_OBJ="1_obj_$CAT_SIMBAD"

if [ $CAT_SIMBAD -nt $CAT_SIMBAD_OBJ ]; then
    # filter SIMBAD main_type (stars only) to exclude Planet, Galaxy ...
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_SIMBAD cmd='progress; select !NULL_sp_type||NULL_morph_type&&!NULL_main_type&&(main_type.equals(\"Star\")||!(main_type.contains(\"Galaxy\")||main_type.contains(\"EmG\")||main_type.contains(\"Seyfert\")||main_type.contains(\"GroupG\")||main_type.contains(\"QSO\")||main_type.contains(\"Radio\")||main_type.contains(\"AGN\")||main_type.contains(\"Cl\")||main_type.contains(\"in\")||main_type.contains(\"Inexistent\")||main_type.contains(\"Planet\")||main_type.contains(\"EmObj\")||main_type.contains(\"ISM\")||main_type.contains(\"HII\")||main_type.contains(\"SFR\")||main_type.contains(\"SNR\"))) ' out=$CAT_SIMBAD_OBJ_STAR
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_SIMBAD_OBJ_STAR > $CAT_SIMBAD_OBJ_STAR.stats.log

    # filter on V > 16 (not in TYCHO2)
    MAG_FILTER="NULL_V||V<=16.0"
    
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_SIMBAD_OBJ_STAR cmd="progress; select !(${MAG_FILTER}) " out=$CAT_SIMBAD_OBJ_Vgt16
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_SIMBAD_OBJ_Vgt16 > $CAT_SIMBAD_OBJ_Vgt16.stats.log

    # filter on V <= 16 (max in TYCHO2)
    # convert SIMBAD coords to epoch 1991.25:
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_SIMBAD_OBJ_STAR cmd="progress; select ${MAG_FILTER} ; addcol ra_s_91 'ra  + ((NULL_pmra ) ? 0.0 : (1991.25 - 2000.0) * (0.001 * pmra / 3600.0) / cos(degreesToRadians(dec)))' ; addcol de_s_91 'dec + ((NULL_pmdec) ? 0.0 : (1991.25 - 2000.0) * (0.001 * pmdec / 3600.0))' " out=$CAT_SIMBAD_OBJ
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_SIMBAD_OBJ > $CAT_SIMBAD_OBJ.stats.log
fi
echo "- STATS on $CAT_SIMBAD_OBJ_STAR :"
cat $CAT_SIMBAD_OBJ_STAR.stats.log

echo "- STATS on ${CAT_SIMBAD_OBJ_Vgt16} :"
cat ${CAT_SIMBAD_OBJ_Vgt16}.stats.log

echo "- STATS on $CAT_SIMBAD_OBJ :"
cat $CAT_SIMBAD_OBJ.stats.log


# SIMBAD: internal match within 3 as to identify inner groups and filter columns:
# (main_id, ra/dec 1991) only to identify groups
CAT_SIMBAD_MIN="1_min_$CAT_SIMBAD_OBJ"
CAT_SIMBAD_MIN_0="1_min_0_$CAT_SIMBAD_OBJ"
if [ $CAT_SIMBAD_OBJ -nt $CAT_SIMBAD_MIN ] || [ $CAT_SIMBAD_OBJ -nt $CAT_SIMBAD_MIN_0 ]; then
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_SIMBAD_OBJ cmd='progress ; keepcols "ra_s_91 de_s_91 main_id" ' out=${CAT_SIMBAD_MIN_0}
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=${CAT_SIMBAD_MIN_0} > ${CAT_SIMBAD_MIN_0}.stats.log

    # identify inner groups in SIMBAD:
    stilts ${STILTS_JAVA_OPTIONS} tmatch1 matcher='sky' params='3.0' values='ra_s_91 de_s_91' action='identify' in=${CAT_SIMBAD_MIN_0} out=$CAT_SIMBAD_MIN
    
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_SIMBAD_MIN cmd='progress ; colmeta -name SIMBAD_GROUP_SIZE GroupSize ; delcols "GroupID" ' out=$CAT_SIMBAD_MIN
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_SIMBAD_MIN > $CAT_SIMBAD_MIN.stats.log
fi
echo "- STATS on ${CAT_SIMBAD_MIN_0} :"
cat ${CAT_SIMBAD_MIN_0}.stats.log

echo "- STATS on $CAT_SIMBAD_MIN :"
cat $CAT_SIMBAD_MIN.stats.log


# SIMBAD: column subset for candidates
CAT_SIMBAD_SUB="1_sub_$CAT_SIMBAD_OBJ"
CAT_SIMBAD_SUB_SPTYPE="1_sub_sptype_$CAT_SIMBAD_OBJ"

if [ $CAT_SIMBAD_OBJ -nt $CAT_SIMBAD_SUB ] || [ $CAT_SIMBAD_OBJ -nt $CAT_SIMBAD_SUB_SPTYPE ]; then
    # sort V first to ensure brightest source in groups ...
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_SIMBAD_OBJ cmd='progress ; colmeta -name ra_s ra ; colmeta -name de_s dec ; keepcols "ra_s de_s ra_s_91 de_s_91 main_id sp_type main_type other_types V"; sort V ' out=$CAT_SIMBAD_SUB
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_SIMBAD_SUB > $CAT_SIMBAD_SUB.stats.log

    # filter on SPTYPE
    SPTYPE_FILTER="!NULL_SP_TYPE"

    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_SIMBAD_OBJ cmd="progress; colmeta -name ra_s ra ; colmeta -name de_s dec ; keepcols \"ra_s de_s pmra pmdec ra_s_91 de_s_91 main_id sp_type main_type other_types V\"; sort V ; select $SPTYPE_FILTER " out=$CAT_SIMBAD_SUB_SPTYPE
    
    # get groups from SIMBAD objects for all interesting source with sptype:
    stilts ${STILTS_JAVA_OPTIONS} tmatch2 matcher='sky' params='0.01' values1='ra_s_91 de_s_91' values2='ra_s_91 de_s_91' find='best' join='all1' in1=$CAT_SIMBAD_SUB_SPTYPE in2=$CAT_SIMBAD_MIN out=$CAT_SIMBAD_SUB_SPTYPE
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_SIMBAD_SUB_SPTYPE cmd='progress; delcols "ra_s_91_2 de_s_91_2 main_id_2 Separation"; colmeta -name main_id main_id_1 ; colmeta -name ra_s_91 ra_s_91_1 ; colmeta -name de_s_91 de_s_91_1 ' out=$CAT_SIMBAD_SUB_SPTYPE
    
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_SIMBAD_SUB_SPTYPE > $CAT_SIMBAD_SUB_SPTYPE.stats.log
fi
echo "- STATS on $CAT_SIMBAD_SUB :"
cat $CAT_SIMBAD_SUB.stats.log

echo "- STATS on $CAT_SIMBAD_SUB_SPTYPE :"
cat $CAT_SIMBAD_SUB_SPTYPE.stats.log


# MDFC: filter columns for candidates
CAT_MDFC_MIN="1_min_$CAT_MDFC"
if [ $CAT_MDFC -nt $CAT_MDFC_MIN ]; then
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_MDFC cmd='progress ; keepcols "Name RAJ2000 DEJ2000 IRflag med_Lflux disp_Lflux med_Mflux disp_Mflux med_Nflux disp_Nflux" ; colmeta -ucd ID_MDFC -name MDFC_ID Name; colmeta -ucd IR_FLAG IRflag ; colmeta -name Lflux_med -ucd PHOT_FLUX_L med_Lflux ; addcol -after Lflux_med -ucd PHOT_FLUX_L_ERROR e_Lflux_med 1.4826*disp_Lflux ; colmeta -name Mflux_med -ucd PHOT_FLUX_M med_Mflux ; addcol -after Mflux_med -ucd PHOT_FLUX_M_ERROR e_Mflux_med 1.4826*disp_Mflux ; colmeta -name Nflux_med -ucd PHOT_FLUX_N med_Nflux ; addcol -after Nflux_med -ucd PHOT_FLUX_N_ERROR e_Nflux_med 1.4826*disp_Nflux; delcols "disp_Lflux disp_Mflux disp_Nflux" ' out=$CAT_MDFC_MIN
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_MDFC_MIN > $CAT_MDFC_MIN.stats.log
fi
echo "- STATS on $CAT_MDFC_MIN :"
cat $CAT_MDFC_MIN.stats.log


# 2. Crossmatch ASCC / SIMBAD within 3 as to identify both groups
CAT_ASCC_SIMBAD_GRP="2_Grp_xmatch_ASCC_SIMBAD_full.fits"
CAT_ASCC_SIMBAD_GRP_0="2_Grp_xmatch_ASCC_SIMBAD_full_0.fits"
if [ $CAT_ASCC_MIN_GRP -nt $CAT_ASCC_SIMBAD_GRP ] || [ $CAT_SIMBAD_MIN -nt $CAT_ASCC_SIMBAD_GRP ]; then
    # use best1 to get groups in SIMBAD for every ASCC source at epoch 1991.25

    stilts ${STILTS_JAVA_OPTIONS} tmatch2 matcher='sky' params='3.0' values1='ra_a_91 de_a_91' values2='ra_s_91 de_s_91' find='best1' join='all1' in1=$CAT_ASCC_MIN_GRP in2=$CAT_SIMBAD_MIN out=$CAT_ASCC_SIMBAD_GRP_0
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_ASCC_SIMBAD_GRP_0 > ${CAT_ASCC_SIMBAD_GRP_0}.stats.log

    # Only consider groups between ASCC x SIMBAD within 3as:
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_ASCC_SIMBAD_GRP_0 cmd='progress ; delcols "ra_s_91 de_s_91 main_id SIMBAD_GROUP_SIZE GroupID Separation" ; colmeta -name SIMBAD_GROUP_SIZE GroupSize ' out=$CAT_ASCC_SIMBAD_GRP
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_ASCC_SIMBAD_GRP > $CAT_ASCC_SIMBAD_GRP.stats.log
fi
echo "- STATS on $CAT_ASCC_SIMBAD_GRP_0 :"
cat ${CAT_ASCC_SIMBAD_GRP_0}.stats.log

echo "- STATS on $CAT_ASCC_SIMBAD_GRP :"
cat $CAT_ASCC_SIMBAD_GRP.stats.log


# 2. Crossmatch ASCC / SIMBAD within 1.5 as to get both DATA
CAT_ASCC_SIMBAD_XMATCH="2_xmatch_ASCC_SIMBAD_full.fits"
CAT_NO_ASCC_SIMBAD_XMATCH="2_xmatch_NoASCC_SIMBAD_full.fits"
if [ $CAT_ASCC_SIMBAD_GRP -nt $CAT_ASCC_SIMBAD_XMATCH ] || [ $CAT_SIMBAD_SUB -nt $CAT_ASCC_SIMBAD_XMATCH ] || [ $CAT_ASCC_SIMBAD_GRP -nt $CAT_NO_ASCC_SIMBAD_XMATCH ]; then
    # use best to have the best ASCC x SIMBAD object: 
    # can not rely on SIMBAD V that may be missing or wrong ... (could help if missing value could be considered too)
    
    # xmatch on COO(1991.25):
    # note: simbad may have multiple entries (children) for X, X A, X B so main_id & sp_type may differ.
    stilts ${STILTS_JAVA_OPTIONS} tmatch2 matcher='sky' params='1.5' values1='ra_a_91 de_a_91' values2='ra_s_91 de_s_91' find='best' join='all1' in1=$CAT_ASCC_SIMBAD_GRP in2=$CAT_SIMBAD_SUB out=$CAT_ASCC_SIMBAD_XMATCH 
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_ASCC_SIMBAD_XMATCH cmd='progress ; addcol XM_SIMBAD_SEP_2000 "skyDistanceDegrees(_RAJ2000,_DEJ2000,ra_s,de_s) * 3600.0" ; addcol XM_SIMBAD_SEP_91 "skyDistanceDegrees(ra_a_91,de_a_91,ra_s_91,de_s_91) * 3600.0" ; delcols "ra_s de_s ra_s_91 de_s_91" ; colmeta -name XM_SIMBAD_SEP -ucd XM_SIMBAD_SEP Separation ' out=$CAT_ASCC_SIMBAD_XMATCH
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_ASCC_SIMBAD_XMATCH > $CAT_ASCC_SIMBAD_XMATCH.stats.log
    
    # keep complementary (missing in ASCC at 3 as but SPTYPE):
    stilts ${STILTS_JAVA_OPTIONS} tmatch2 matcher='sky' params='3.0' values1='ra_a_91 de_a_91' values2='ra_s_91 de_s_91' find='best' join='2not1' in1=$CAT_ASCC_SIMBAD_GRP in2=$CAT_SIMBAD_SUB_SPTYPE out=$CAT_NO_ASCC_SIMBAD_XMATCH 
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_NO_ASCC_SIMBAD_XMATCH cmd='progress ; colmeta -name _RAJ2000 ra_s; colmeta -name _DEJ2000 de_s; colmeta -ucd PHOT_JHN_V -name V_SIMBAD V; delcols "ra_s_91 de_s_91" ' out=$CAT_NO_ASCC_SIMBAD_XMATCH
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_NO_ASCC_SIMBAD_XMATCH > $CAT_NO_ASCC_SIMBAD_XMATCH.stats.log
fi
echo "- STATS on $CAT_ASCC_SIMBAD_XMATCH :"
cat $CAT_ASCC_SIMBAD_XMATCH.stats.log

echo "- STATS on $CAT_NO_ASCC_SIMBAD_XMATCH :"
cat $CAT_NO_ASCC_SIMBAD_XMATCH.stats.log


# ~2500 rows where XM_SIMBAD_SEP_2000 > 1.5 as (mostly because of pmRa/De difference or ASCC mistake = wrong association)

# Compare crossmatch on 1991 vs 2000:
CAT_ASCC_SIMBAD_XMATCH_J2K="2_xmatch_ASCC_SIMBAD_full_J2000.fits"
if [ $CAT_ASCC_SIMBAD_XMATCH -nt $CAT_ASCC_SIMBAD_XMATCH_J2K ] || [ $CAT_SIMBAD_SUB -nt $CAT_ASCC_SIMBAD_XMATCH_J2K ]; then
    # use best to have the best ASCC x SIMBAD object: 
    # can not rely on SIMBAD V that may be missing or wrong ... (could help if missing value could be considered too)
    
    # xmatch on COO(J2000):
    stilts ${STILTS_JAVA_OPTIONS} tmatch2 matcher='sky' params='1.5' values1='_RAJ2000 _DEJ2000' values2='ra_s de_s' find='best' join='all1' in1=$CAT_ASCC_SIMBAD_XMATCH in2=$CAT_SIMBAD_SUB out=$CAT_ASCC_SIMBAD_XMATCH_J2K 
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_ASCC_SIMBAD_XMATCH_J2K cmd='progress ; addcol XM_SIMBAD_FLAG "!main_Id_1.equals(main_Id_2)?1:NULL" ; delcols "XM_SIMBAD_SEP_91 XM_SIMBAD_SEP_2000 ra_s de_s ra_s_91 de_s_91 main_id_2 sp_type_2 main_type_2 other_types_2 Separation" ; colmeta -name main_id main_id_1 ; colmeta -name sp_type sp_type_1 ; colmeta -name main_type main_type_1 ; colmeta -name other_types other_types_1 ; colmeta -name V V_1 ; colmeta -ucd PHOT_JHN_V -name V_SIMBAD V_2' out=$CAT_ASCC_SIMBAD_XMATCH_J2K
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_ASCC_SIMBAD_XMATCH_J2K > $CAT_ASCC_SIMBAD_XMATCH_J2K.stats.log
fi
echo "- STATS on $CAT_ASCC_SIMBAD_XMATCH_J2K :"
cat $CAT_ASCC_SIMBAD_XMATCH_J2K.stats.log


# 2. Crossmatch ASCC / SIMBAD / MDFC (SIMBAD main_id):
CAT_ASCC_SIMBAD_MDFC_XMATCH="2_xmatch_ASCC_SIMBAD_MDFC_full.fits"
if [ $CAT_ASCC_SIMBAD_XMATCH_J2K -nt $CAT_ASCC_SIMBAD_MDFC_XMATCH ] || [ $CAT_MDFC_MIN -nt $CAT_ASCC_SIMBAD_MDFC_XMATCH ]; then
    stilts ${STILTS_JAVA_OPTIONS} tmatch2 matcher='exact' values1='main_id' values2='MDFC_ID' join='all1' in1=$CAT_ASCC_SIMBAD_XMATCH_J2K in2=$CAT_MDFC_MIN out=$CAT_ASCC_SIMBAD_MDFC_XMATCH
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_ASCC_SIMBAD_MDFC_XMATCH cmd='progress ; delcols "RAJ2000 DEJ2000" ' out=$CAT_ASCC_SIMBAD_MDFC_XMATCH
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_ASCC_SIMBAD_MDFC_XMATCH > $CAT_ASCC_SIMBAD_MDFC_XMATCH.stats.log
fi
echo "- STATS on $CAT_ASCC_SIMBAD_MDFC_XMATCH :"
cat $CAT_ASCC_SIMBAD_MDFC_XMATCH.stats.log


# xmatch on RA/DE (J2000) within 1.5as (as mdfc coords are different):

CAT_ASCC_SIMBAD_MDFC_XMATCH_DIST="2_xmatch_ASCC_SIMBAD_MDFC_full_dist.fits"
if [ $CAT_ASCC_SIMBAD_XMATCH_J2K -nt $CAT_ASCC_SIMBAD_MDFC_XMATCH_DIST ] || [ $CAT_MDFC_MIN -nt $CAT_ASCC_SIMBAD_MDFC_XMATCH_DIST ]; then
    stilts ${STILTS_JAVA_OPTIONS} tmatch2 matcher='sky' params='1.5' values1='_RAJ2000 _DEJ2000' values2='hmsToDegrees(RAJ2000) dmsToDegrees(DEJ2000)' find='best' join='all1' in1=$CAT_ASCC_SIMBAD_XMATCH_J2K in2=$CAT_MDFC_MIN out=$CAT_ASCC_SIMBAD_MDFC_XMATCH_DIST
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_ASCC_SIMBAD_MDFC_XMATCH_DIST cmd='progress ; delcols "RAJ2000 DEJ2000 Separation" ' out=$CAT_ASCC_SIMBAD_MDFC_XMATCH_DIST
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CAT_ASCC_SIMBAD_MDFC_XMATCH_DIST > $CAT_ASCC_SIMBAD_MDFC_XMATCH_DIST.stats.log
fi
echo "- STATS on $CAT_ASCC_SIMBAD_MDFC_XMATCH_DIST :"
cat $CAT_ASCC_SIMBAD_MDFC_XMATCH_DIST.stats.log


# Use xmatch(COO) for MDFC:
CAT_ASCC_SIMBAD_MDFC_XMATCH=$CAT_ASCC_SIMBAD_MDFC_XMATCH_DIST


# 3. Prepare candidates: columns, sort, then filters:
FULL_CANDIDATES="3_xmatch_ASCC_SIMBAD_MDFC_full_cols.fits"
if [ $CAT_ASCC_SIMBAD_MDFC_XMATCH -nt $FULL_CANDIDATES ]; then
    # note: merge (main_type,other_types) as other_types can be NULL:
    # convert XM_SIMBAD_FLAG (bad xm on 91 vs 2000?) into GROUP_SIZE_3:
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_ASCC_SIMBAD_MDFC_XMATCH cmd='progress ; addcol -after sp_type -ucd OBJ_TYPES OTYPES NULL_main_type?\"\":replaceAll(concat(\",\",main_type,\",\",replaceAll(other_types,\"[|]\",\",\"),\",\"),\"\ \",\"\") ; delcols "main_type other_types Vmag V" ; addcol -before _RAJ2000 -ucd POS_EQ_RA_MAIN RAJ2000 degreesToHms(_RAJ2000,6) ; addcol -after RAJ2000 -ucd POS_EQ_DEC_MAIN DEJ2000 degreesToDms(_DEJ2000,6) ; colmeta -ucd POS_EQ_PMRA pmRA ; colmeta -ucd POS_EQ_PMDEC pmDE ; colmeta -name MAIN_ID -ucd ID_MAIN main_id ; colmeta -name SP_TYPE -ucd SPECT_TYPE_MK sp_type ; addcol -after XM_SIMBAD_SEP -ucd GROUP_SIZE GROUP_SIZE_3 max((!NULL_ASCC_GROUP_SIZE?ASCC_GROUP_SIZE:0),(!NULL_SIMBAD_GROUP_SIZE?SIMBAD_GROUP_SIZE:0))+(!NULL_XM_SIMBAD_FLAG?1:0) ; sort _DEJ2000 ; delcols "_RAJ2000 _DEJ2000 ASCC_GROUP_SIZE SIMBAD_GROUP_SIZE XM_SIMBAD_FLAG" ' out=$FULL_CANDIDATES
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$FULL_CANDIDATES > $FULL_CANDIDATES.stats.log
fi
echo "- STATS on $FULL_CANDIDATES :"
cat $FULL_CANDIDATES.stats.log


# Complementary catalog (NoASCC but SIMBAD SPTYPE)
ALT_CANDIDATES="3_xmatch_NoASCC_SIMBAD_full_cols.fits"
if [ $CAT_NO_ASCC_SIMBAD_XMATCH -nt $ALT_CANDIDATES ]; then
    # note: merge (main_type,other_types) as other_types can be NULL:
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CAT_NO_ASCC_SIMBAD_XMATCH cmd='progress ; addcol -after sp_type -ucd OBJ_TYPES OTYPES NULL_main_type?\"\":replaceAll(concat(\",\",main_type,\",\",replaceAll(other_types,\"[|]\",\",\"),\",\"),\"\ \",\"\") ; delcols "main_type other_types" ; addcol -before _RAJ2000 -ucd POS_EQ_RA_MAIN RAJ2000 degreesToHms(_RAJ2000,6) ; addcol -after RAJ2000 -ucd POS_EQ_DEC_MAIN DEJ2000 degreesToDms(_DEJ2000,6) ; colmeta -ucd POS_EQ_PMRA -name pmRA pmra ; colmeta -ucd POS_EQ_PMDEC -name pmDE pmdec ; colmeta -name MAIN_ID -ucd ID_MAIN main_id ; colmeta -name SP_TYPE -ucd SPECT_TYPE_MK sp_type ; colmeta -name GROUP_SIZE_3 -ucd GROUP_SIZE SIMBAD_GROUP_SIZE; sort _DEJ2000 ; delcols "_RAJ2000 _DEJ2000" ' out=$ALT_CANDIDATES
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$ALT_CANDIDATES > $ALT_CANDIDATES.stats.log
fi
echo "- STATS on $ALT_CANDIDATES :"
cat $ALT_CANDIDATES.stats.log


# Prepare candidate catalogs using the SearchCal cfg format:

# Convert to VOTables:
# - bright: select stars WITH SP_TYPE + neighbours
CATALOG_BRIGHT="3_xmatch_ASCC_SIMBAD_MDFC_full_sptype.vot"
CATALOG_BRIGHT_0="3_xmatch_ASCC_SIMBAD_MDFC_full_sptype_0.fits"
if [ $FULL_CANDIDATES -nt $CATALOG_BRIGHT ]; then

    # Get all objects with SP_TYPE:
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$FULL_CANDIDATES cmd='progress; select !NULL_SP_TYPE ' out=$CATALOG_BRIGHT_0
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CATALOG_BRIGHT_0 > ${CATALOG_BRIGHT_0}.stats.log

    # Get all neightbours within 3 as:
    # use all to get all objects within 3 as in ASCC FULL for every ASCC BRIGHT source at epoch 1991.25
    stilts ${STILTS_JAVA_OPTIONS} tmatch2 matcher='sky' params='3.0' values1='ra_a_91 de_a_91' values2='ra_a_91 de_a_91' find='all' join='all1' fixcols='all' suffix2='' in1=$CATALOG_BRIGHT_0 in2=$FULL_CANDIDATES out=$CATALOG_BRIGHT
    # remove duplicates (on de_a_91)
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$CATALOG_BRIGHT cmd='progress ; sort de_a_91 ; uniq de_a_91 ; delcols "*_1 ra_a_91 de_a_91 GroupID GroupSize Separation" ' out=$CATALOG_BRIGHT
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CATALOG_BRIGHT > $CATALOG_BRIGHT.stats.log
fi
echo "- STATS on $CATALOG_BRIGHT_0 :"
cat ${CATALOG_BRIGHT_0}.stats.log
echo "- STATS on $CATALOG_BRIGHT :"
cat $CATALOG_BRIGHT.stats.log


CATALOG_BRIGHT_COMPL="3_xmatch_NoASCC_SIMBAD_full_sptype.vot"
if [ $ALT_CANDIDATES -nt $CATALOG_BRIGHT_COMPL ]; then

    # remove duplicates (on de_a_91)
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$ALT_CANDIDATES cmd='progress ' out=$CATALOG_BRIGHT_COMPL
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CATALOG_BRIGHT_COMPL > $CATALOG_BRIGHT_COMPL.stats.log
fi
echo "- STATS on $CATALOG_BRIGHT_COMPL :"
cat $CATALOG_BRIGHT_COMPL.stats.log


# - faint: select stars WITHOUT SP_TYPE nor neighbours (not in CATALOG_BRIGHT)
CATALOG_FAINT="3_xmatch_ASCC_SIMBAD_MDFC_full_NO_sptype.vot"
CATALOG_FAINT_0="3_xmatch_ASCC_SIMBAD_MDFC_full_NO_sptype_0.fits"
if [ $FULL_CANDIDATES -nt $CATALOG_FAINT ]; then

    # delete any MDFC columns as it should correspond to JSDC BRIGHT
    stilts ${STILTS_JAVA_OPTIONS} tpipe in=$FULL_CANDIDATES cmd='progress; delcols "ra_a_91 de_a_91 SP_TYPE MDFC_ID IRflag Lflux_med e_Lflux_med Mflux_med e_Mflux_med Nflux_med e_Nflux_med" ' out=$CATALOG_FAINT_0
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CATALOG_FAINT_0 > ${CATALOG_FAINT_0}.stats.log

    # use DEJ2000 (unique)
    stilts ${STILTS_JAVA_OPTIONS} tmatch2 matcher='exact' values1='DEJ2000' values2='DEJ2000' join='1not2' in1=$CATALOG_FAINT_0 in2=$CATALOG_BRIGHT out=$CATALOG_FAINT
    stilts ${STILTS_JAVA_OPTIONS} tcat omode="stats" in=$CATALOG_FAINT > $CATALOG_FAINT.stats.log
fi
echo "- STATS on $CATALOG_FAINT_0 :"
cat ${CATALOG_FAINT_0}.stats.log
echo "- STATS on $CATALOG_FAINT :"
cat $CATALOG_FAINT.stats.log


# 4. Generate SearchCal config files:
CONFIG="vobsascc_simbad_sptype.cfg"
if [ $CATALOG_BRIGHT -nt $CONFIG ]; then
    xsltproc -o $CONFIG sclguiVOTableToTSV.xsl $CATALOG_BRIGHT
fi

CONFIG="vobsNoAscc_simbad_no_sptype.cfg"
if [ $CATALOG_BRIGHT_COMPL -nt $CONFIG ]; then
    xsltproc -o $CONFIG sclguiVOTableToTSV.xsl $CATALOG_BRIGHT_COMPL
fi

CONFIG="vobsascc_simbad_no_sptype.cfg"
if [ $CATALOG_FAINT -nt $CONFIG ]; then
    xsltproc -o $CONFIG sclguiVOTableToTSV.xsl $CATALOG_FAINT
fi


echo "That's all folks ! @ date: `date -u`"


