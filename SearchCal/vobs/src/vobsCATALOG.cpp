/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Definition vobsCATALOG class .
 */

/*
 * System Headers
 */
#include <iostream>
#include <stdlib.h>
using namespace std;


/*
 * MCS Headers
 */
#include "mcs.h"
#include "log.h"
#include "err.h"
#include "misc.h"

/*
 * Local Headers
 */
#include "vobsCATALOG.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"
#include "vobsSTAR.h"
#include "vobsSCENARIO_RUNTIME.h"

/*
 * Convenience macros
 */

/**
 * Add common Column meta datas:
 * - RA/DEC coordinates precessed by CDS in J200 epoch 2000.0
 * - targetId
 */
#define AddCommonColumnMetas(meta) \
        meta->AddColumnMeta(vobsCATALOG___RAJ2000,    "POS_EQ_RA_MAIN",   vobsSTAR_POS_EQ_RA_MAIN);       /* RA   coordinate */ \
        meta->AddColumnMeta(vobsCATALOG___DEJ2000,    "POS_EQ_DEC_MAIN",  vobsSTAR_POS_EQ_DEC_MAIN);      /* DEC  coordinate */ \
        meta->AddColumnMeta(vobsCATALOG___TARGET_ID,  "ID_TARGET",        vobsSTAR_ID_TARGET);            /* star coordinates sent to CDS (RA+DEC) */


/** Initialize static members */
vobsCATALOG_META_PTR_MAP vobsCATALOG::vobsCATALOG_catalogMetaMap;

bool vobsCATALOG::vobsCATALOG_catalogMetaInitialized = false;

/*
 * Class constructor
 */

/**
 * Build a catalog object.
 */
vobsCATALOG::vobsCATALOG(vobsORIGIN_INDEX catalogId)
{
    if (vobsCATALOG::vobsCATALOG_catalogMetaInitialized == false)
    {
        AddCatalogMetas();
    }

    // Define meta data:
    _meta = vobsCATALOG::GetCatalogMeta(catalogId);
}


/*
 * Class destructor
 */

/**
 * Delete a catalog object.
 */
vobsCATALOG::~vobsCATALOG()
{
}

void vobsCATALOG::AddCatalogMetas(void)
{
    if (vobsCATALOG::vobsCATALOG_catalogMetaInitialized == false)
    {
        vobsCATALOG_META* meta;

        // Local Catalogs:

        // JSDC BRIGHT LOCAL (primary catalog) = ASCC stars merged with SIMBAD (RA/DE J2000, pmRA/DE, SpType, objType)
        // only candidates = [RA/DEC positions (J2000 - epoch 2000) + pmRA/DEC] from ASCC
        meta = new vobsCATALOG_META("JSDC_LOCAL", vobsCATALOG_JSDC_LOCAL_ID);
        meta->AddColumnMeta("RAJ2000",      "POS_EQ_RA_MAIN",           vobsSTAR_POS_EQ_RA_MAIN);       // RA   coordinate (ASCC)
        meta->AddColumnMeta("DEJ2000",      "POS_EQ_DEC_MAIN",          vobsSTAR_POS_EQ_DEC_MAIN);      // DEC  coordinate (ASCC)
        meta->AddColumnMeta("pmRA",         "POS_EQ_PMRA",              vobsSTAR_POS_EQ_PMRA);          // RA   proper motion (ASCC)
        meta->AddColumnMeta("pmDE",         "POS_EQ_PMDEC",             vobsSTAR_POS_EQ_PMDEC);         // DEC  proper motion (ASCC)
        /* Simbad id, SpType & ObjTypes */
        meta->AddColumnMeta("MAIN_ID",      "ID_MAIN",                  vobsSTAR_ID_SIMBAD);            // SIMBAD main identifier
        meta->AddColumnMeta("SP_TYPE",      "SPECT_TYPE_MK",            vobsSTAR_SPECT_TYPE_MK);        // spectral type (Simbad)
        meta->AddColumnMeta("OTYPES",       "OBJ_TYPES",                vobsSTAR_OBJ_TYPES);            // object types (Simbad)
        /* Group size within 5 arcsecs */
        meta->AddColumnMeta("GROUP_SIZE_5", "GROUP_SIZE",               vobsSTAR_GROUP_SIZE);           // ASCC / SIMBAD Group size
        AddCatalogMeta(meta);


        // JSDC FAINT LOCAL (primary catalog) = ASCC stars merged with SIMBAD (RA/DE J2000, pmRA/DE, NO SpType, objType)
        // only candidates = [RA/DEC positions (J2000 - epoch 2000) + pmRA/DEC] from ASCC
        meta = new vobsCATALOG_META("JSDC_FAINT_LOCAL", vobsCATALOG_JSDC_FAINT_LOCAL_ID);
        meta->AddColumnMeta("RAJ2000",      "POS_EQ_RA_MAIN",           vobsSTAR_POS_EQ_RA_MAIN);       // RA   coordinate (ASCC)
        meta->AddColumnMeta("DEJ2000",      "POS_EQ_DEC_MAIN",          vobsSTAR_POS_EQ_DEC_MAIN);      // DEC  coordinate (ASCC)
        meta->AddColumnMeta("pmRA",         "POS_EQ_PMRA",              vobsSTAR_POS_EQ_PMRA);          // RA   proper motion (ASCC)
        meta->AddColumnMeta("pmDE",         "POS_EQ_PMDEC",             vobsSTAR_POS_EQ_PMDEC);         // DEC  proper motion (ASCC)
        /* Simbad id, SpType & ObjTypes */
        meta->AddColumnMeta("MAIN_ID",      "ID_MAIN",                  vobsSTAR_ID_SIMBAD);            // SIMBAD main identifier
        meta->AddColumnMeta("OTYPES",       "OBJ_TYPES",                vobsSTAR_OBJ_TYPES);            // object types (Simbad)
        /* Group size within 5 arcsecs */
        meta->AddColumnMeta("GROUP_SIZE_5", "GROUP_SIZE",               vobsSTAR_GROUP_SIZE);           // ASCC / SIMBAD Group size
        AddCatalogMeta(meta);


        // Remote catalogs:
        /*
         * 2MASS catalog ["II/246/out"] gives coordinates in various epochs (1997 - 2001) but has Julian date:
         * North: 1997 June 7 - 2000 December 1 UT (mjd = 50606)
         * South: 1998 March 18 - 2001 February 15 UT (mjd = 51955)
         */
        // 2015.05.28: discard query filter Opt=[TU] (bad 2MASS match with TYCHO / USNO)
        meta = new vobsCATALOG_META("2MASS", vobsCATALOG_MASS_ID, 1.0, 1997.43, 2001.13);
        AddCommonColumnMetas(meta);
        meta->AddColumnMeta("2MASS",        "ID_MAIN",                  vobsSTAR_ID_2MASS);             // 2MASS identifier
        meta->AddColumnMeta("JD",           "TIME_DATE",                vobsSTAR_JD_DATE);              // Julian date of source measurement
        meta->AddColumnMeta("opt",          "ID_CATALOG",               vobsSTAR_2MASS_OPT_ID_CATALOG); // associated optical source
        meta->AddColumnMeta("Qflg",         "CODE_QUALITY",             vobsSTAR_CODE_QUALITY_2MASS);   // quality flag [ABCDEFUX]
        meta->AddColumnMeta("Jmag",         "PHOT_JHN_J",               vobsSTAR_PHOT_JHN_J);           // johnson magnitude J
        meta->AddColumnMeta("e_Jmag",       "ERROR",                    vobsSTAR_PHOT_JHN_J_ERROR);     // error johnson magnitude J
        meta->AddColumnMeta("Hmag",         "PHOT_JHN_H",               vobsSTAR_PHOT_JHN_H);           // johnson magnitude H
        meta->AddColumnMeta("e_Hmag",       "ERROR",                    vobsSTAR_PHOT_JHN_H_ERROR);     // error johnson magnitude H
        meta->AddColumnMeta("Kmag",         "PHOT_JHN_K",               vobsSTAR_PHOT_JHN_K);           // johnson magnitude K
        meta->AddColumnMeta("e_Kmag",       "ERROR",                    vobsSTAR_PHOT_JHN_K_ERROR);     // error johnson magnitude K
        AddCatalogMeta(meta);


        /*
         * The AKARI ["II/297/irc"] Infrared Astronomical Satellite observed the whole sky in
         * the far infrared (50-180µm) and the mid-infrared (9 and 18µm)
         * between May 2006 and August 2007 (Murakami et al. 2007PASJ...59S.369M)
         * Note: Does not have Julian date !
         */
        meta = new vobsCATALOG_META("AKARI", vobsCATALOG_AKARI_ID, 1.0, 2006.333, 2007.667);
        AddCommonColumnMetas(meta);
        meta->AddColumnMeta("objID",        "ID_NUMBER",                vobsSTAR_ID_AKARI);             // AKARI sequential index
        meta->AddColumnMeta("S09",          "PHOT_FLUX_IR_9",           vobsSTAR_PHOT_FLUX_IR_09);      // flux density at 9 µm
        meta->AddColumnMeta("e_S09",        "ERROR",                    vobsSTAR_PHOT_FLUX_IR_09_ERROR); // flux density error at 9 µm
        meta->AddColumnMeta("S18",          "PHOT_FLUX_IR_25",          vobsSTAR_PHOT_FLUX_IR_18);      // flux density at 18 µm
        meta->AddColumnMeta("e_S18",        "ERROR",                    vobsSTAR_PHOT_FLUX_IR_18_ERROR); // flux density error at 18 µm
        AddCatalogMeta(meta);


        /*
         * The Wide-field Infrared Survey Explorer (WISE; see Wright et al.
         * 2010AJ....140.1868W) is a NASA Medium Class Explorer mission that
         * conducted a digital imaging survey of the entire sky in the 3.4, 4.6,
         * 12 and 22um mid-infrared bandpasses (hereafter W1, W2, W3 and W4)
         * between 7th Jan 2010 and 1st Feb 2011
         * Note: Does not have Julian date !
         * note: RA/DEC (J2000 are given at mean epoch 2010.58) and ignores PM corrections
         * PSF ~ 6 arcsec
         */
        meta = new vobsCATALOG_META("WISE", vobsCATALOG_WISE_ID, 1.0, 2010.02, 2011.085);
        AddCommonColumnMetas(meta);
        meta->AddColumnMeta("AllWISE",      "ID_MAIN",                  vobsSTAR_ID_WISE);              // WISE All-Sky Release Catalog name
        meta->AddColumnMeta("qph",          "CODE_QUALITY",             vobsSTAR_CODE_QUALITY_WISE);    // Photometric quality flag [ABCUXZ]
        meta->AddColumnMeta("W1mag",        "PHOT_IR_3.4",              vobsSTAR_PHOT_JHN_L);           // W1 magnitude (3.35um)
        meta->AddColumnMeta("e_W1mag",      "ERROR",                    vobsSTAR_PHOT_JHN_L_ERROR);     // Mean error on W1 magnitude
        meta->AddColumnMeta("W2mag",        "PHOT_IR_4.2",              vobsSTAR_PHOT_JHN_M);           // W2 magnitude (4.6um)
        meta->AddColumnMeta("e_W2mag",      "ERROR",                    vobsSTAR_PHOT_JHN_M_ERROR);     // Mean error on W2 magnitude
        meta->AddColumnMeta("W3mag",        "PHOT_FLUX_IR_12",          vobsSTAR_PHOT_JHN_N);           // W3 magnitude (11.6um)
        meta->AddColumnMeta("e_W3mag",      "ERROR",                    vobsSTAR_PHOT_JHN_N_ERROR);     // Mean error on W3 magnitude
        AddCatalogMeta(meta);


        // Note: if overwritePropertyMask is used, only properties(and errors) defined by the property mask will be updated / overwritten !

        // 201608: disabled overwrite mask on RA/DEC PM RA/DEC:
        /*
        // ASCC catalog ["I/280"] gives coordinates in epoch 1991.25 (hip) and has proper motions:
        // Overwrite RA/DEC and pmRA/DEC to update their values AND errors (JSDC, GetStar and Faint scenario)
        const char* ascc_overwriteIds [] = {vobsSTAR_POS_EQ_RA_MAIN,  vobsSTAR_POS_EQ_DEC_MAIN,
                                            vobsSTAR_POS_EQ_PMRA,     vobsSTAR_POS_EQ_PMDEC};
         */
        meta = new vobsCATALOG_META("ASCC", vobsCATALOG_ASCC_ID, 1.0, 1991.25, 1991.25, mcsTRUE, mcsFALSE
                                    // ,vobsSTAR::GetPropertyMask(sizeof (ascc_overwriteIds) / sizeof (ascc_overwriteIds[0]), ascc_overwriteIds));
                                    );
        AddCommonColumnMetas(meta);
        meta->AddColumnMeta("e_RAJ2000",    "ERROR",                    vobsSTAR_POS_EQ_RA_ERROR);      // Error on RA*cos(DEdeg) (mas)
        meta->AddColumnMeta("e_DEJ2000",    "ERROR",                    vobsSTAR_POS_EQ_DEC_ERROR);     // DEdeg error (mas)
        meta->AddColumnMeta("HIP",          "ID_ALTERNATIVE",           vobsSTAR_ID_HIP);               // HIP  identifier
        meta->AddColumnMeta("HD",           "ID_ALTERNATIVE",           vobsSTAR_ID_HD);                // HD   identifier
        meta->AddColumnMeta("DM",           "ID_ALTERNATIVE",           vobsSTAR_ID_DM);                // DM   identifier
        meta->AddColumnMeta("TYC1",         "ID_ALTERNATIVE",           vobsSTAR_ID_TYC1);              // TYC1 identifier
        meta->AddColumnMeta("TYC2",         "ID_ALTERNATIVE",           vobsSTAR_ID_TYC2);              // TYC2 identifier
        meta->AddColumnMeta("TYC3",         "ID_ALTERNATIVE",           vobsSTAR_ID_TYC3);              // TYC3 identifier
        meta->AddColumnMeta("pmRA",         "POS_EQ_PMRA",              vobsSTAR_POS_EQ_PMRA);          // RA   proper motion
        meta->AddColumnMeta("e_pmRA",       "ERROR",                    vobsSTAR_POS_EQ_PMRA_ERROR);    // RA   error on proper motion
        meta->AddColumnMeta("pmDE",         "POS_EQ_PMDEC",             vobsSTAR_POS_EQ_PMDEC);         // DEC  proper motion
        meta->AddColumnMeta("e_pmDE",       "ERROR",                    vobsSTAR_POS_EQ_PMDEC_ERROR);   // DEC  error on proper motion
        // ASCC Plx/e_Plx are not as good as HIP2 so use carefully for non-HIP2 stars
        //      meta->AddColumnMeta("Plx",          "POS_PARLX_TRIG",           vobsSTAR_POS_PARLX_TRIG);       // parallax
        //      meta->AddColumnMeta("e_Plx",        "POS_PARLX_TRIG_ERROR",     vobsSTAR_POS_PARLX_TRIG_ERROR); // parallax error
        meta->AddColumnMeta("SpType",       "SPECT_TYPE_MK",            vobsSTAR_SPECT_TYPE_MK);        // spectral type
        meta->AddColumnMeta("Bmag",         "PHOT_JHN_B",               vobsSTAR_PHOT_JHN_B);           // johnson magnitude B
        meta->AddColumnMeta("e_Bmag",       "ERROR",                    vobsSTAR_PHOT_JHN_B_ERROR);     // error johnson magnitude B
        meta->AddColumnMeta("Vmag",         "PHOT_JHN_V",               vobsSTAR_PHOT_JHN_V);           // johnson magnitude V
        meta->AddColumnMeta("e_Vmag",       "ERROR",                    vobsSTAR_PHOT_JHN_V_ERROR);     // error johnson magnitude V
        meta->AddColumnMeta("v1",           "CODE_VARIAB",              vobsSTAR_CODE_VARIAB_V1);       // variability v1
        meta->AddColumnMeta("v2",           "CODE_VARIAB",              vobsSTAR_CODE_VARIAB_V2);       // variability v2
        meta->AddColumnMeta("v3",           "VAR_CLASS",                vobsSTAR_CODE_VARIAB_V3);       // variability v3
        meta->AddColumnMeta("d5",           "CODE_MULT_FLAG",           vobsSTAR_CODE_MULT_FLAG);       // multiplicty flag d5
        AddCatalogMeta(meta);


        // BSC catalog ["V/50/catalog"]
        meta = new vobsCATALOG_META("BSC", vobsCATALOG_BSC_ID);
        AddCommonColumnMetas(meta);
        meta->AddColumnMeta("HD",           "ID_ALTERNATIVE",           vobsSTAR_ID_HD);                // HD   identifier
        meta->AddColumnMeta("RotVel",       "VELOC_ROTAT",              vobsSTAR_VELOC_ROTAT);          // rotational velocity
        AddCatalogMeta(meta);


        // CIO catalog ["II/225/catalog"] has multiple rows per target:
        meta = new vobsCATALOG_META("CIO", vobsCATALOG_CIO_ID, 1.0, EPOCH_2000, EPOCH_2000, mcsFALSE, mcsTRUE, NULL,
                                    "&x_F(IR)=M&lambda=1.25,1.65,2.20,3.5,5.0,10.0"); // IR magnitudes for (J, H, K, L, M, N)
        AddCommonColumnMetas(meta);
        meta->AddColumnMeta("HD",           "ID_ALTERNATIVE",           vobsSTAR_ID_HD);                // HD   identifier
        // note: following properties are decoded using specific code in vobsCDATA.h
        // magnitudes for given bands (J, H, K, L, M, N) stored in the 'vobsSTAR_PHOT_JHN_?' property
        meta->AddColumnMeta("lambda",       "INST_WAVELENGTH_VALUE",    NULL);                          // magnitude
        meta->AddColumnMeta("F(IR)",        "PHOT_FLUX_IR_MISC",        NULL);                          // IR magnitude
        AddCatalogMeta(meta);


        // B/denis - so far not able to used it correctly
        // DENIS catalog ["B/denis"] gives coordinates in various epochs (1995 - 2002) but has Julian date:
        meta = new vobsCATALOG_META("DENIS", vobsCATALOG_DENIS_ID, 1.0, 1995.5, 2002.5);
        // keep empty catalog meta data when disabled
        if (vobsCATALOG_DENIS_ID_ENABLE)
        {
            // Note: denis is an IR survey (I gun, J, Ks) => R/B are coming from USNO-A match !

            AddCommonColumnMetas(meta);
            meta->AddColumnMeta("DENIS",        "ID_MAIN",                  vobsSTAR_ID_DENIS);             // Denis identifier
            meta->AddColumnMeta("ObsJD",        "TIME_DATE",                vobsSTAR_JD_DATE);              // Julian date of source measurement

            meta->AddColumnMeta("Imag",         "PHOT_COUS_I",              vobsSTAR_PHOT_COUS_I);          // cousin magnitude Imag at 0.82 µm
            meta->AddColumnMeta("e_Imag",       "ERROR",                    vobsSTAR_PHOT_COUS_I_ERROR);    // error cousin magnitude I
            meta->AddColumnMeta("Iflg",         "CODE_MISC",                vobsSTAR_CODE_MISC_I);          // quality flag
            // TODO: decide if photographic B/R magnitudes should be removed definitely (Denis, USNO, 2MASS ...)
            // because useless for computations only used by GUI (display)
            meta->AddColumnMeta("Bmag",         "PHOT_PHG_B",               vobsSTAR_PHOT_PHG_B);           // photometric magnitude B
            meta->AddColumnMeta("Rmag",         "PHOT_PHG_R",               vobsSTAR_PHOT_PHG_R);           // photometric magnitude B
        }
        AddCatalogMeta(meta);


        // DENIS_JK catalog ["J/A+A/413/1037/table1"] MAY give coordinates in various epochs (1995 - 2002) and has Julian date:
        // TODO: use epochs 1995.5, 2002.5
        meta = new vobsCATALOG_META("DENIS-JK", vobsCATALOG_DENIS_JK_ID, 1.0, EPOCH_2000, EPOCH_2000, mcsFALSE, mcsFALSE, NULL,
                                    "&Var=%3C4"); // variability constraint: Var < 4
        AddCommonColumnMetas(meta);
        // Get the Julian date of source measurement (TIME_DATE) stored in the 'vobsSTAR_JD_DATE' property
        // miscDynBufAppendString(&_query, "&-out=ObsJD");
        // TODO: fix it
        // JD-2400000 	d 	Mean JD (= JD-2400000) of observation (time.epoch)
        meta->AddColumnMeta("Jmag",         "PHOT_JHN_J",               vobsSTAR_PHOT_JHN_J);           // johnson magnitude J
        meta->AddColumnMeta("e_Jmag",       "ERROR",                    vobsSTAR_PHOT_JHN_J_ERROR);     // error johnson magnitude J
        meta->AddColumnMeta("Ksmag",        "PHOT_JHN_K",               vobsSTAR_PHOT_JHN_K);           // johnson magnitude Ks
        meta->AddColumnMeta("e_Ksmag",      "ERROR",                    vobsSTAR_PHOT_JHN_K_ERROR);     // error johnson magnitude Ks
        AddCatalogMeta(meta);


        // HIC catalog ["I/196/main"]
        meta = new vobsCATALOG_META("HIC", vobsCATALOG_HIC_ID);
        AddCommonColumnMetas(meta);
        meta->AddColumnMeta("HD",           "ID_ALTERNATIVE",           vobsSTAR_ID_HD);                // HD   identifier
        meta->AddColumnMeta("RV",           "VELOC_HC",                 vobsSTAR_VELOC_HC);             // radial velocity
        AddCatalogMeta(meta);


        // HIP1 catalog ["I/239/hip_main"] gives coordinates in epoch 1991.25 (hip):
        // note: B and I magnitudes (+ errors) are computed in post processing
        const char* hip1_overwriteIds [] = {vobsSTAR_PHOT_JHN_V, vobsSTAR_PHOT_JHN_B_V,
                                            vobsSTAR_PHOT_COUS_V_I, vobsSTAR_PHOT_COUS_V_I_REFER_CODE,
                                            vobsSTAR_PHOT_JHN_B, vobsSTAR_PHOT_COUS_I};

        meta = new vobsCATALOG_META("HIP1", vobsCATALOG_HIP1_ID, 1.0, 1991.25, 1991.25, mcsTRUE, mcsFALSE,
                                    vobsSTAR::GetPropertyMask(sizeof (hip1_overwriteIds) / sizeof (hip1_overwriteIds[0]), hip1_overwriteIds));
        AddCommonColumnMetas(meta);
        meta->AddColumnMeta("HIP",          "ID_MAIN",                  vobsSTAR_ID_HIP);               // HIP  identifier
        meta->AddColumnMeta("Vmag",         "PHOT_JHN_V",               vobsSTAR_PHOT_JHN_V);           // johnson magnitude V
        meta->AddColumnMeta("e_VTmag",      "ERROR",                    vobsSTAR_PHOT_JHN_V_ERROR);     // error TYCHO magnitude V
        meta->AddColumnMeta("B-V",          "PHOT_JHN_B-V",             vobsSTAR_PHOT_JHN_B_V);         // johnson B-V colour
        meta->AddColumnMeta("e_B-V",        "ERROR",                    vobsSTAR_PHOT_JHN_B_V_ERROR);   // error johnson B-V colour
        meta->AddColumnMeta("V-I",          "PHOT_COUS_V-I",            vobsSTAR_PHOT_COUS_V_I);        // cousin V-I colour
        meta->AddColumnMeta("e_V-I",        "ERROR",                    vobsSTAR_PHOT_COUS_V_I_ERROR);   // error cousin V-I colour
        meta->AddColumnMeta("r_V-I",        "REFER_CODE",               vobsSTAR_PHOT_COUS_V_I_REFER_CODE); // [A-T] Source of V-I
        AddCatalogMeta(meta);


        // HIP2 catalog ["I/311/hip2"] gives precise coordinates and parallax in epoch 1991.25 (hip) and has proper motions:
        // 201608: disabled overwrite mask on RA/DEC PM RA/DEC:
        const char* hip2_overwriteIds [] = {
                                            // vobsSTAR_POS_EQ_RA_MAIN,  vobsSTAR_POS_EQ_DEC_MAIN,
                                            // vobsSTAR_POS_EQ_PMRA,     vobsSTAR_POS_EQ_PMDEC,
                                            vobsSTAR_POS_PARLX_TRIG
        };

        meta = new vobsCATALOG_META("HIP2", vobsCATALOG_HIP2_ID, 1.0, 1991.25, 1991.25, mcsTRUE, mcsFALSE,
                                    vobsSTAR::GetPropertyMask(sizeof (hip2_overwriteIds) / sizeof (hip2_overwriteIds[0]), hip2_overwriteIds));
        AddCommonColumnMetas(meta);
        meta->AddColumnMeta("HIP",          "ID_MAIN",                  vobsSTAR_ID_HIP);               // HIP  identifier
        meta->AddColumnMeta("e_RArad",      "ERROR",                    vobsSTAR_POS_EQ_RA_ERROR);      // Formal error on RArad (mas)
        meta->AddColumnMeta("e_DErad",      "ERROR",                    vobsSTAR_POS_EQ_DEC_ERROR);     // Formal error on DErad (mas)
        meta->AddColumnMeta("pmRA",         "POS_EQ_PMRA",              vobsSTAR_POS_EQ_PMRA);          // RA   proper motion
        meta->AddColumnMeta("e_pmRA",       "ERROR",                    vobsSTAR_POS_EQ_PMRA_ERROR);    // RA   error on proper motion
        meta->AddColumnMeta("pmDE",         "POS_EQ_PMDEC",             vobsSTAR_POS_EQ_PMDEC);         // DEC  proper motion
        meta->AddColumnMeta("e_pmDE",       "ERROR",                    vobsSTAR_POS_EQ_PMDEC_ERROR);   // DEC  error on proper motion
        meta->AddColumnMeta("Plx",          "POS_PARLX_TRIG",           vobsSTAR_POS_PARLX_TRIG);       // parallax
        meta->AddColumnMeta("e_Plx",        "ERROR",                    vobsSTAR_POS_PARLX_TRIG_ERROR); // parallax error
        AddCatalogMeta(meta);


        // PHOTO catalog ["II/7A/catalog"] has multiple rows per target:
        meta = new vobsCATALOG_META("PHOTO", vobsCATALOG_PHOTO_ID, 1.0, EPOCH_2000, EPOCH_2000, mcsFALSE, mcsTRUE);
        AddCommonColumnMetas(meta);
        meta->AddColumnMeta("B",            "PHOT_JHN_B",               vobsSTAR_PHOT_JHN_B);           // johnson magnitude B
        meta->AddColumnMeta("V",            "PHOT_JHN_V",               vobsSTAR_PHOT_JHN_V);           // johnson magnitude V
        meta->AddColumnMeta("R",            "PHOT_JHN_R",               vobsSTAR_PHOT_JHN_R);           // johnson magnitude R
        meta->AddColumnMeta("I",            "PHOT_JHN_I",               vobsSTAR_PHOT_JHN_I);           // johnson magnitude I
        meta->AddColumnMeta("J",            "PHOT_JHN_J",               vobsSTAR_PHOT_JHN_J);           // johnson magnitude J
        meta->AddColumnMeta("H",            "PHOT_JHN_H",               vobsSTAR_PHOT_JHN_H);           // johnson magnitude H
        meta->AddColumnMeta("K",            "PHOT_JHN_K",               vobsSTAR_PHOT_JHN_K);           // johnson magnitude K
        meta->AddColumnMeta("L",            "PHOT_JHN_L",               vobsSTAR_PHOT_JHN_L);           // johnson magnitude L
        meta->AddColumnMeta("M",            "PHOT_JHN_M",               vobsSTAR_PHOT_JHN_M);           // johnson magnitude M
        meta->AddColumnMeta("N",            "PHOT_IR_N:10.4",           vobsSTAR_PHOT_JHN_N);           // johnson magnitude N
        AddCatalogMeta(meta);


        // SB9 catalog ["B/sb9/main"]
        meta = new vobsCATALOG_META("SB9", vobsCATALOG_SB9_ID);
        AddCommonColumnMetas(meta);
        meta->AddColumnMeta("Seq",          "ID_MAIN",                  vobsSTAR_ID_SB9);               // SB9 sequential index
        AddCatalogMeta(meta);


        // SBSC catalog ["V/36B/bsc4s"]
        meta = new vobsCATALOG_META("SBSC", vobsCATALOG_SBSC_ID);
        AddCommonColumnMetas(meta);
        meta->AddColumnMeta("HD",           "ID_MAIN",                  vobsSTAR_ID_HD);                // HD   identifier
        meta->AddColumnMeta("vsini",        "VELOC_ROTAT",              vobsSTAR_VELOC_ROTAT);          // rotational velocity
        AddCatalogMeta(meta);


        // USNO-B1.0 catalog ["I/284"] has proper motion and is used by crossmatch with 2MASS (FAINT):
        meta = new vobsCATALOG_META("USNO", vobsCATALOG_USNO_ID, 1.0, EPOCH_2000, EPOCH_2000, mcsTRUE);
        // keep empty catalog meta data when disabled
        if (vobsCATALOG_USNO_ID_ENABLE)
        {
            AddCommonColumnMetas(meta);
            meta->AddColumnMeta("e_RAJ2000",    "ERROR",                    vobsSTAR_POS_EQ_RA_ERROR);      // Error on RA*cos(DE) (mas)
            meta->AddColumnMeta("e_DEJ2000",    "ERROR",                    vobsSTAR_POS_EQ_DEC_ERROR);     // DE error (mas)
            meta->AddColumnMeta("pmRA",         "POS_EQ_PMRA",              vobsSTAR_POS_EQ_PMRA);          // RA   proper motion
            meta->AddColumnMeta("e_pmRA",       "ERROR",                    vobsSTAR_POS_EQ_PMRA_ERROR);    // RA   error on proper motion
            meta->AddColumnMeta("pmDE",         "POS_EQ_PMDEC",             vobsSTAR_POS_EQ_PMDEC);         // DEC  proper motion
            meta->AddColumnMeta("e_pmDE",       "ERROR",                    vobsSTAR_POS_EQ_PMDEC_ERROR);   // DEC  error on proper motion
            // TODO: decide if photographic B/R/I magnitudes should be removed definitely
            // because useless for computations only used by GUI (display)
            meta->AddColumnMeta("Imag",         "PHOT_PHG_I",               vobsSTAR_PHOT_PHG_I);           // photometric magnitude I
            // 2 photometric Rmag/Bmag are available, TODO: decide/validate which magnitudes use
            meta->AddColumnMeta("B1mag",        "PHOT_PHG_B",               vobsSTAR_PHOT_PHG_B);           // photometric magnitude B
            meta->AddColumnMeta("B2mag",        "PHOT_PHG_B",               vobsSTAR_PHOT_PHG_B);           // photometric magnitude B
            meta->AddColumnMeta("R1mag",        "PHOT_PHG_R",               vobsSTAR_PHOT_PHG_R);           // photometric magnitude R
            meta->AddColumnMeta("R2mag",        "PHOT_PHG_R",               vobsSTAR_PHOT_PHG_R);           // photometric magnitude R
        }
        AddCatalogMeta(meta);


        // WDS catalog ["B/wds/wds"]
        meta = new vobsCATALOG_META("WDS", vobsCATALOG_WDS_ID, 1.0, EPOCH_2000, EPOCH_2000, mcsFALSE, mcsFALSE, NULL, NULL,
                                    mcsFALSE); // Do not sort results because WDS has multiple records for the same RA/DEC coordinates
        AddCommonColumnMetas(meta);
        meta->AddColumnMeta("WDS",          "ID_MAIN",                  vobsSTAR_ID_WDS);               // WDS sequential index
        meta->AddColumnMeta("sep1",         "ORBIT_SEPARATION",         vobsSTAR_ORBIT_SEPARATION_SEP1); // first mesured separation
        meta->AddColumnMeta("sep2",         "ORBIT_SEPARATION",         vobsSTAR_ORBIT_SEPARATION_SEP2); // last mesured separation
        AddCatalogMeta(meta);


        // Dump properties into XML file:
        DumpCatalogMetaAsXML();

        vobsCATALOG::vobsCATALOG_catalogMetaInitialized = true;
    }
}

/**
 * Dump the property index
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned
 */
mcsCOMPL_STAT vobsCATALOG::DumpCatalogMetaAsXML()
{
    miscoDYN_BUF xmlBuf;
    // Prepare buffer:
    FAIL(xmlBuf.Reserve(60 * 1024));

    xmlBuf.AppendLine("<?xml version=\"1.0\"?>\n\n");

    FAIL(xmlBuf.AppendString("<catalogList>\n"));

    for (vobsCATALOG_META_PTR_MAP::iterator iter = vobsCATALOG::vobsCATALOG_catalogMetaMap.begin();
            iter != vobsCATALOG::vobsCATALOG_catalogMetaMap.end(); iter++)
    {
        iter->second->DumpCatalogMetaAsXML(xmlBuf);
    }

    FAIL(xmlBuf.AppendString("</catalogList>\n\n"));

    mcsCOMPL_STAT result = mcsSUCCESS;

    // This file will be stored in the $MCSDATA/tmp repository
    const char* fileName = "$MCSDATA/tmp/CatalogMeta.xml";

    // Resolve path
    char* resolvedPath = miscResolvePath(fileName);
    if (IS_NOT_NULL(resolvedPath))
    {
        logInfo("Saving catalog meta XML description: %s", resolvedPath);

        result = xmlBuf.SaveInASCIIFile(resolvedPath);
        free(resolvedPath);
    }
    return result;
}


/*
 * Public methods
 */

/*___oOo___*/
