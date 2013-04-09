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
#include "vobsCATALOG_AKARI.h"
#include "vobsCATALOG_DENIS.h"
#include "vobsCATALOG_LBSI.h"
#include "vobsCATALOG_SB9.h"
#include "vobsCATALOG_SBSC.h"


/*
 * Convenience macros
 */

/**
 * Add common Column meta datas:
 * - RA/DEC coordinates precessed by CDS in J200 epoch 2000.0
 * - targetId
 */
#define AddCommonColumnMetas(meta) \
        meta->AddColumnMeta(vobsCATALOG_RAJ2000,    "POS_EQ_RA_MAIN",   vobsSTAR_POS_EQ_RA_MAIN);       /* RA   coordinate */ \
        meta->AddColumnMeta(vobsCATALOG_DEJ2000,    "POS_EQ_DEC_MAIN",  vobsSTAR_POS_EQ_DEC_MAIN);      /* DEC  coordinate */ \
        meta->AddColumnMeta(vobsCATALOG_TARGET_ID,  "ID_TARGET",        vobsSTAR_ID_TARGET);            /* star coordinates sent to CDS (RA+DEC) */


/** Initialize static members */
vobsCATALOG_META_PTR_MAP vobsCATALOG::vobsCATALOG_catalogMetaMap;

bool vobsCATALOG::vobsCATALOG_catalogMetaInitialized = false;

/*
 * Class constructor
 */

/**
 * Build a catalog object.
 */
vobsCATALOG::vobsCATALOG(const char* name)
{
    if (vobsCATALOG::vobsCATALOG_catalogMetaInitialized == false)
    {
        AddCatalogMetas();
    }

    // Define meta data:
    _meta = vobsCATALOG::GetCatalogMeta(name);
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
        
        // ASCC LOCAL:
        meta = new vobsCATALOG_META("ASCC_LOCAL", vobsCATALOG_ASCC_LOCAL_ID);
        meta->AddColumnMeta("RAJ2000",      "POS_EQ_RA_MAIN",           vobsSTAR_POS_EQ_RA_MAIN);       // RA   coordinate
        meta->AddColumnMeta("DEJ2000",      "POS_EQ_DEC_MAIN",          vobsSTAR_POS_EQ_DEC_MAIN);      // DEC  coordinate
        // ASCC Plx/e_Plx are not as good as HIP2 => discarded
//      meta->AddColumnMeta("Plx",          "POS_PARLX_TRIG",           vobsSTAR_POS_PARLX_TRIG);       // parallax
//      meta->AddColumnMeta("e_Plx",        "POS_PARLX_TRIG_ERROR",     vobsSTAR_POS_PARLX_TRIG_ERROR); // parallax error
        meta->AddColumnMeta("pmRA",         "POS_EQ_PMRA",              vobsSTAR_POS_EQ_PMRA);          // RA   proper motion 
        meta->AddColumnMeta("pmDE",         "POS_EQ_PMDEC",             vobsSTAR_POS_EQ_PMDEC);         // DEC  proper motion 
        meta->AddColumnMeta("Bmag",         "PHOT_JHN_B",               vobsSTAR_PHOT_JHN_B);           // johnson magnitude B
        meta->AddColumnMeta("Vmag",         "PHOT_JHN_V",               vobsSTAR_PHOT_JHN_V);           // johnson magnitude V
        // ASCC J/H/K magnitudes are not as good as 2MASS => discarded
//      meta->AddColumnMeta("Jmag",         "PHOT_JHN_J",               vobsSTAR_PHOT_JHN_J);           // johnson magnitude J
//      meta->AddColumnMeta("Hmag",         "PHOT_JHN_H",               vobsSTAR_PHOT_JHN_H);           // johnson magnitude H
//      meta->AddColumnMeta("Kmag",         "PHOT_JHN_K",               vobsSTAR_PHOT_JHN_K);           // johnson magnitude K
        meta->AddColumnMeta("v1",           "CODE_VARIAB",              vobsSTAR_CODE_VARIAB_V1);       // variability v1
        meta->AddColumnMeta("v2",           "CODE_VARIAB",              vobsSTAR_CODE_VARIAB_V2);       // variability v2
        meta->AddColumnMeta("v3",           "VAR_CLASS",                vobsSTAR_CODE_VARIAB_V3);       // variability v3
        meta->AddColumnMeta("d5",           "CODE_MULT_FLAG",           vobsSTAR_CODE_MULT_FLAG);       // multiplicty flag d5
        meta->AddColumnMeta("SpType",       "SPECT_TYPE_MK",            vobsSTAR_SPECT_TYPE_MK);        // spectral type
        meta->AddColumnMeta("TYC1",         "ID_TYC1",                  vobsSTAR_ID_TYC1);              // TYC1 identifier
        meta->AddColumnMeta("TYC2",         "ID_TYC2",                  vobsSTAR_ID_TYC2);              // TYC2 identifier
        meta->AddColumnMeta("TYC3",         "ID_TYC3",                  vobsSTAR_ID_TYC3);              // TYC3 identifier
        meta->AddColumnMeta("HIP",          "ID_HIP",                   vobsSTAR_ID_HIP);               // HIP  identifier
        meta->AddColumnMeta("HD",           "ID_HD",                    vobsSTAR_ID_HD);                // HD   identifier
        meta->AddColumnMeta("DM",           "ID_DM",                    vobsSTAR_ID_DM);                // DM   identifier
        AddCatalogMeta(meta);
        
        
        // MIDI LOCAL:
        meta = new vobsCATALOG_META("MIDI", vobsCATALOG_MIDI_ID);
        meta->AddColumnMeta("HD",           "ID_HD",                    vobsSTAR_ID_HD);                // HD   identifier
        meta->AddColumnMeta("RAJ2000",      "POS_EQ_RA_MAIN",           vobsSTAR_POS_EQ_RA_MAIN);       // RA   coordinate
        meta->AddColumnMeta("DEJ2000",      "POS_EQ_DEC_MAIN",          vobsSTAR_POS_EQ_DEC_MAIN);      // DEC  coordinate
        meta->AddColumnMeta("orig",         "IR_FLUX_ORIGIN",           vobsSTAR_IR_FLUX_ORIGIN);
        meta->AddColumnMeta("SpType",       "SPECT_TYPE_MK",            vobsSTAR_SPECT_TYPE_MK);        // spectral type
        meta->AddColumnMeta("Plx",          "POS_PARLX_TRIG",           vobsSTAR_POS_PARLX_TRIG);       // parallax
        meta->AddColumnMeta("e_Plx",        "POS_PARLX_TRIG_ERROR",     vobsSTAR_POS_PARLX_TRIG_ERROR); // parallax error
        meta->AddColumnMeta("pmRA",         "POS_EQ_PMRA",              vobsSTAR_POS_EQ_PMRA);          // RA   proper motion 
        meta->AddColumnMeta("pmDE",         "POS_EQ_PMDEC",             vobsSTAR_POS_EQ_PMDEC);         // DEC  proper motion 
        meta->AddColumnMeta("Vmag",         "PHOT_JHN_V",               vobsSTAR_PHOT_JHN_V);           // johnson magnitude V
        meta->AddColumnMeta("Hmag",         "PHOT_JHN_H",               vobsSTAR_PHOT_JHN_H);           // johnson magnitude H
        meta->AddColumnMeta("Fnu_12",       "PHOT_FLUX_IR_12",          vobsSTAR_PHOT_FLUX_IR_12);      // conflict with F12
        meta->AddColumnMeta("Fqual_12",     "PHOT_FLUX_IR_12_ERROR",    vobsSTAR_PHOT_FLUX_IR_12_ERROR);// conflict with e_F12
        meta->AddColumnMeta("F12",          "PHOT_FLUX_IR_12",          vobsSTAR_PHOT_FLUX_IR_12);      // conflict with Fnu_12
        meta->AddColumnMeta("e_F12",        "PHOT_FLUX_IR_12_ERROR",    vobsSTAR_PHOT_FLUX_IR_12_ERROR);// conflict with Fqual_12
        meta->AddColumnMeta("Bin_HIP",      "CODE_MULT_FLAG",           vobsSTAR_CODE_MULT_FLAG);
        meta->AddColumnMeta("Var",          "CODE_VARIAB",              vobsSTAR_CODE_BIN_FLAG);
        meta->AddColumnMeta("Calib",        "REF_STAR",                 vobsSTAR_REF_STAR);
        meta->AddColumnMeta("Teff",         "PHYS_TEMP_EFFEC",          vobsSTAR_PHYS_TEMP_EFFEC);
        meta->AddColumnMeta("e_Tef",        "PHYS_TEMP_EFFEC_ERROR",    vobsSTAR_PHYS_TEMP_EFFEC_ERROR);
        meta->AddColumnMeta("Diam12",       "DIAM12",                   vobsSTAR_DIAM12);
        meta->AddColumnMeta("e_Diam12",     "DIAM12_ERROR",             vobsSTAR_DIAM12_ERROR);
        meta->AddColumnMeta("A_V",          "PHOT_EXTINCTION_TOTAL",    vobsSTAR_PHOT_EXTINCTION_TOTAL);
        meta->AddColumnMeta("Chi2",         "CHI2_QUALITY",             vobsSTAR_CHI2_QUALITY);
        meta->AddColumnMeta("SpTyp_Teff",   "SP_TYP_PHYS_TEMP_EFFEC",   vobsSTAR_SP_TYP_PHYS_TEMP_EFFEC);
        AddCatalogMeta(meta);

        
        // Remote catalogs:
        /*
         * 2MASS catalog ["II/246/out"] gives coordinates in various epochs (1997 - 2001) but has Julian date:
         * North: 1997 June 7 - 2000 December 1 UT (mjd = 50606)
         * South: 1998 March 18 - 2001 February 15 UT (mjd = 51955)
         */
        meta = new vobsCATALOG_META("2MASS", vobsCATALOG_MASS_ID, 1.0, 1997.43, 2001.13, mcsFALSE, mcsFALSE, NULL, "&opt=%5bTU%5d");
        AddCommonColumnMetas(meta);
        meta->AddColumnMeta("2MASS",        "ID_MAIN",                  vobsSTAR_ID_2MASS);             // 2MASS identifier
        meta->AddColumnMeta("JD",           "TIME_DATE",                vobsSTAR_JD_DATE);              // Julian date of source measurement
        meta->AddColumnMeta("opt",          "ID_CATALOG",               vobsSTAR_2MASS_OPT_ID_CATALOG); // associated optical source
        meta->AddColumnMeta("Qflg",         "CODE_QUALITY",             vobsSTAR_CODE_QUALITY);         // quality flag
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
        meta->AddColumnMeta("e_S09",        "ERROR",                    vobsSTAR_PHOT_FLUX_IR_09_ERROR);// flux density error at 9 µm
        meta->AddColumnMeta("S18",          "PHOT_FLUX_IR_25",          vobsSTAR_PHOT_FLUX_IR_18);      // flux density at 18 µm
        meta->AddColumnMeta("e_S18",        "ERROR",                    vobsSTAR_PHOT_FLUX_IR_18_ERROR);// flux density error at 18 µm
        AddCatalogMeta(meta);
        

        // ASCC catalog ["I/280"] gives coordinates in epoch 1991.25 (hip) and has proper motions:
        meta = new vobsCATALOG_META("ASCC", vobsCATALOG_ASCC_ID, 1.0, 1991.25, 1991.25, mcsTRUE);
        AddCommonColumnMetas(meta);
        meta->AddColumnMeta("e_RAJ2000",    "ERROR",                    vobsSTAR_POS_EQ_RA_ERROR);      // RA   error
        meta->AddColumnMeta("e_DEJ2000",    "ERROR",                    vobsSTAR_POS_EQ_DEC_ERROR);     // DEC  error
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
        // ASCC Plx/e_Plx are not as good as HIP2 => discarded
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
        // note: following properties are decoded using specific code in vobsCDATA.h
        meta->AddColumnMeta("HD",           "ID_ALTERNATIVE",           vobsSTAR_ID_HD);                // HD   identifier
        // magnitudes for given bands (J, H, K, L, M, N) stored in the 'vobsSTAR_PHOT_JHN_?' property
        meta->AddColumnMeta("lambda",       "INST_WAVELENGTH_VALUE",    NULL);                          // magnitude
        meta->AddColumnMeta("F(IR)",        "PHOT_FLUX_IR_MISC",        NULL);                          // IR magnitude
        AddCatalogMeta(meta);

        
        // DENIS catalog ["B/denis"] gives coordinates in various epochs (1995 - 2002) but has Julian date:
        meta = new vobsCATALOG_META("DENIS", vobsCATALOG_DENIS_ID, 1.0, 1995.5, 2002.5);
        AddCommonColumnMetas(meta);
        meta->AddColumnMeta("DENIS",        "ID_MAIN",                  vobsSTAR_ID_DENIS);             // Denis identifier
        meta->AddColumnMeta("ObsJD",        "TIME_DATE",                vobsSTAR_JD_DATE);              // Julian date of source measurement
        // A2RAdeg / A2DEdeg = USNOA2.0 nearest match: TODO what use = query USNO catalog ?
        meta->AddColumnMeta("A2RAdeg",      "POS_EQ_RA_OTHER",          vobsSTAR_POS_EQ_RA_OTHER);      // RA  USNOA2.0 nearest match
        meta->AddColumnMeta("A2DEdeg",      "POS_EQ_DEC_OTHER",         vobsSTAR_POS_EQ_DEC_OTHER);     // DEC USNOA2.0 nearest match

        meta->AddColumnMeta("Imag",         "PHOT_COUS_I",              vobsSTAR_PHOT_COUS_I);          // cousin magnitude Imag at 0.82 µm
        meta->AddColumnMeta("e_Imag",       "ERROR",                    vobsSTAR_PHOT_COUS_I_ERROR);    // error cousin magnitude I
        meta->AddColumnMeta("Iflg",         "CODE_MISC",                vobsSTAR_CODE_MISC_I);          // quality flag
        // TODO: decide if photographic blue and red magnitudes should be removed definitely (Denis, USNO, 2MASS ...)
        // because useless for computations only used by GUI (display)
        meta->AddColumnMeta("Bmag",         "PHOT_PHG_B",               vobsSTAR_PHOT_PHG_B);           // photometric magnitude B
        meta->AddColumnMeta("Rmag",         "PHOT_PHG_R",               vobsSTAR_PHOT_PHG_R);           // photometric magnitude B
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
        AddCatalogMeta(meta);

        // HIP2 catalog ["I/311/hip2"] gives precise coordinates and parallax in epoch 1991.25 (hip) and has proper motions:
        const char* overwriteIds [] = {vobsSTAR_POS_EQ_RA_MAIN, vobsSTAR_POS_EQ_DEC_MAIN,
                                       vobsSTAR_POS_EQ_RA_ERROR, vobsSTAR_POS_EQ_DEC_ERROR,
                                       vobsSTAR_POS_EQ_PMRA, vobsSTAR_POS_EQ_PMDEC,
                                       vobsSTAR_POS_EQ_PMRA_ERROR, vobsSTAR_POS_EQ_PMDEC_ERROR,
                                       vobsSTAR_POS_PARLX_TRIG, vobsSTAR_POS_PARLX_TRIG_ERROR};

        meta = new vobsCATALOG_META("HIP2", vobsCATALOG_HIP2_ID, 1.0, 1991.25, 1991.25, mcsTRUE, mcsFALSE,
                                            vobsSTAR::GetPropertyMask(sizeof (overwriteIds) / sizeof (overwriteIds[0]), overwriteIds));
        AddCommonColumnMetas(meta);
        AddCatalogMeta(meta);

        // LBSI catalog ["J/A+A/393/183/catalog"]
        meta = new vobsCATALOG_META("LBSI", vobsCATALOG_LBSI_ID);
        AddCommonColumnMetas(meta);
        AddCatalogMeta(meta);

        // MERAND catalog ["J/A+A/433/1155"]
        meta = new vobsCATALOG_META("MERAND", vobsCATALOG_MERAND_ID);
        AddCommonColumnMetas(meta);
        AddCatalogMeta(meta);

        // PHOTO catalog ["II/7A/catalog"] has multiple rows per target:
        meta = new vobsCATALOG_META("PHOTO", vobsCATALOG_PHOTO_ID, 1.0, EPOCH_2000, EPOCH_2000, mcsFALSE, mcsTRUE);
        AddCommonColumnMetas(meta);
        AddCatalogMeta(meta);

        // SB9 catalog ["B/sb9/main"]
        meta = new vobsCATALOG_META("SB9", vobsCATALOG_SB9_ID);
        AddCommonColumnMetas(meta);
        AddCatalogMeta(meta);

        // SBSC catalog ["V/36B/bsc4s"]
        meta = new vobsCATALOG_META("SBSC", vobsCATALOG_SBSC_ID);
        AddCommonColumnMetas(meta);
        AddCatalogMeta(meta);

        // USNO catalog ["I/284"] has proper motion and is used by crossmatch with 2MASS (FAINT):
        meta = new vobsCATALOG_META("USNO", vobsCATALOG_USNO_ID, 1.0, EPOCH_2000, EPOCH_2000, mcsTRUE);
        AddCommonColumnMetas(meta);
        AddCatalogMeta(meta);

        // WDS catalog ["B/wds/wds"]
        meta = new vobsCATALOG_META("WDS", vobsCATALOG_WDS_ID);
        AddCommonColumnMetas(meta);
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
    miscoDYN_BUF buffer;

    // Allocate buffer
    FAIL(buffer.Alloc(20 * 1024));

    buffer.AppendLine("<?xml version=\"1.0\"?>\n\n");

    FAIL(buffer.AppendString("<catalogList>\n"));

    for (vobsCATALOG_META_PTR_MAP::iterator iter = vobsCATALOG::vobsCATALOG_catalogMetaMap.begin();
         iter != vobsCATALOG::vobsCATALOG_catalogMetaMap.end(); iter++)
    {
        iter->second->DumpCatalogMetaAsXML(buffer);
    }

    FAIL(buffer.AppendString("</catalogList>\n\n"));

    const char* fileName = "CatalogMeta.xml";

    logInfo("Saving catalog meta XML description: %s", fileName);

    // Try to save the generated VOTable in the specified file as ASCII
    return buffer.SaveInASCIIFile(fileName);
}


/*
 * Public methods
 */

/*___oOo___*/
