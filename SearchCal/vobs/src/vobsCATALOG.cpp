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
        meta->AddColumnMeta("RAJ2000",      "POS_EQ_RA_MAIN",           vobsSTAR_POS_EQ_RA_MAIN);
        meta->AddColumnMeta("DEJ2000",      "POS_EQ_DEC_MAIN",          vobsSTAR_POS_EQ_DEC_MAIN);
        // ASCC Plx/e_Plx are not as good as HIP2 => discarded
//      meta->AddColumnMeta("Plx",          "POS_PARLX_TRIG",           vobsSTAR_POS_PARLX_TRIG);
//      meta->AddColumnMeta("e_Plx",        "POS_PARLX_TRIG_ERROR",     vobsSTAR_POS_PARLX_TRIG_ERROR);
        meta->AddColumnMeta("pmRA",         "POS_EQ_PMRA",              vobsSTAR_POS_EQ_PMRA);
        meta->AddColumnMeta("pmDE",         "POS_EQ_PMDEC",             vobsSTAR_POS_EQ_PMDEC);
        meta->AddColumnMeta("Bmag",         "PHOT_JHN_B",               vobsSTAR_PHOT_JHN_B);
        meta->AddColumnMeta("Vmag",         "PHOT_JHN_V",               vobsSTAR_PHOT_JHN_V);
        // ASCC J/H/K magnitudes are not as good as 2MASS => discarded
//      meta->AddColumnMeta("Jmag",         "PHOT_JHN_J",               vobsSTAR_PHOT_JHN_J);
//      meta->AddColumnMeta("Hmag",         "PHOT_JHN_H",               vobsSTAR_PHOT_JHN_H);
//      meta->AddColumnMeta("Kmag",         "PHOT_JHN_K",               vobsSTAR_PHOT_JHN_K);
        meta->AddColumnMeta("v1",           "CODE_VARIAB",              vobsSTAR_CODE_VARIAB_V1);
        meta->AddColumnMeta("v2",           "CODE_VARIAB",              vobsSTAR_CODE_VARIAB_V2);
        meta->AddColumnMeta("v3",           "VAR_CLASS",                vobsSTAR_CODE_VARIAB_V3);
        meta->AddColumnMeta("d5",           "CODE_MULT_FLAG",           vobsSTAR_CODE_MULT_FLAG);
        meta->AddColumnMeta("SpType",       "SPECT_TYPE_MK",            vobsSTAR_SPECT_TYPE_MK);
        meta->AddColumnMeta("TYC1",         "ID_TYC1",                  vobsSTAR_ID_TYC1);
        meta->AddColumnMeta("TYC2",         "ID_TYC2",                  vobsSTAR_ID_TYC2);
        meta->AddColumnMeta("TYC3",         "ID_TYC3",                  vobsSTAR_ID_TYC3);
        meta->AddColumnMeta("HIP",          "ID_HIP",                   vobsSTAR_ID_HIP);
        meta->AddColumnMeta("HD",           "ID_HD",                    vobsSTAR_ID_HD);
        meta->AddColumnMeta("DM",           "ID_DM",                    vobsSTAR_ID_DM);
        AddCatalogMeta(meta);

        
        // MIDI LOCAL:
        meta = new vobsCATALOG_META("MIDI", vobsCATALOG_MIDI_ID);
        meta->AddColumnMeta("HD",           "ID_HD",                    vobsSTAR_ID_HD);
        meta->AddColumnMeta("RAJ2000",      "POS_EQ_RA_MAIN",           vobsSTAR_POS_EQ_RA_MAIN);
        meta->AddColumnMeta("DEJ2000",      "POS_EQ_DEC_MAIN",          vobsSTAR_POS_EQ_DEC_MAIN);
        meta->AddColumnMeta("orig",         "IR_FLUX_ORIGIN",           vobsSTAR_IR_FLUX_ORIGIN);
        meta->AddColumnMeta("SpType",       "SPECT_TYPE_MK",            vobsSTAR_SPECT_TYPE_MK);
        meta->AddColumnMeta("Plx",          "POS_PARLX_TRIG",           vobsSTAR_POS_PARLX_TRIG);
        meta->AddColumnMeta("e_Plx",        "POS_PARLX_TRIG_ERROR",     vobsSTAR_POS_PARLX_TRIG_ERROR);
        meta->AddColumnMeta("pmRA",         "POS_EQ_PMRA",              vobsSTAR_POS_EQ_PMRA);
        meta->AddColumnMeta("pmDE",         "POS_EQ_PMDEC",             vobsSTAR_POS_EQ_PMDEC);
        meta->AddColumnMeta("Vmag",         "PHOT_JHN_V",               vobsSTAR_PHOT_JHN_V);
        meta->AddColumnMeta("Hmag",         "PHOT_JHN_H",               vobsSTAR_PHOT_JHN_H);
        meta->AddColumnMeta("Fnu_12",       "PHOT_FLUX_IR_12",          vobsSTAR_PHOT_FLUX_IR_12); // conflict with F12
        meta->AddColumnMeta("Fqual_12",     "PHOT_FLUX_IR_12_ERROR",    vobsSTAR_PHOT_FLUX_IR_12_ERROR); // conflict with e_F12
        meta->AddColumnMeta("F12",          "PHOT_FLUX_IR_12",          vobsSTAR_PHOT_FLUX_IR_12); // conflict with Fnu_12
        meta->AddColumnMeta("e_F12",        "PHOT_FLUX_IR_12_ERROR",    vobsSTAR_PHOT_FLUX_IR_12_ERROR); // conflict with Fqual_12
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
        AddCatalogMeta(new vobsCATALOG_META("2MASS", vobsCATALOG_MASS_ID, 1.0, 1997.43, 2001.13));

        /*
         * The AKARI ["II/297/irc"] Infrared Astronomical Satellite observed the whole sky in
         * the far infrared (50-180µm) and the mid-infrared (9 and 18µm)
         * between May 2006 and August 2007 (Murakami et al. 2007PASJ...59S.369M)
         * Note: Does not have Julian date !
         */
        AddCatalogMeta(new vobsCATALOG_META("AKARI", vobsCATALOG_AKARI_ID, 1.0, 2006.333, 2007.667));

        // ASCC catalog ["I/280"] gives coordinates in epoch 1991.25 (hip) and has proper motions:
        meta = new vobsCATALOG_META("ASCC", vobsCATALOG_ASCC_ID, 1.0, 1991.25, 1991.25, mcsTRUE);
        meta->AddColumnMeta(vobsCATALOG_RAJ2000,    "POS_EQ_RA_MAIN",   vobsSTAR_POS_EQ_RA_MAIN);
        meta->AddColumnMeta(vobsCATALOG_DEJ2000,    "POS_EQ_DEC_MAIN",  vobsSTAR_POS_EQ_DEC_MAIN);
        meta->AddColumnMeta("e_RAJ2000",    "ERROR",                    vobsSTAR_POS_EQ_RA_ERROR);
        meta->AddColumnMeta("e_DEJ2000",    "ERROR",                    vobsSTAR_POS_EQ_DEC_ERROR);
        meta->AddColumnMeta("HIP",          "ID_ALTERNATIVE",           vobsSTAR_ID_HIP);
        meta->AddColumnMeta("HD",           "ID_ALTERNATIVE",           vobsSTAR_ID_HD);
        meta->AddColumnMeta("DM",           "ID_ALTERNATIVE",           vobsSTAR_ID_DM);
        meta->AddColumnMeta("TYC1",         "ID_ALTERNATIVE",           vobsSTAR_ID_TYC1);
        meta->AddColumnMeta("TYC2",         "ID_ALTERNATIVE",           vobsSTAR_ID_TYC2);
        meta->AddColumnMeta("TYC3",         "ID_ALTERNATIVE",           vobsSTAR_ID_TYC3);
        meta->AddColumnMeta("pmRA",         "POS_EQ_PMRA",              vobsSTAR_POS_EQ_PMRA);
        meta->AddColumnMeta("pmDE",         "POS_EQ_PMDEC",             vobsSTAR_POS_EQ_PMDEC);
        meta->AddColumnMeta("e_pmRA",       "ERROR",                    vobsSTAR_POS_EQ_PMRA_ERROR);
        meta->AddColumnMeta("e_pmDE",       "ERROR",                    vobsSTAR_POS_EQ_PMDEC_ERROR);
        // ASCC Plx/e_Plx are not as good as HIP2 => discarded
//      meta->AddColumnMeta("Plx",          "POS_PARLX_TRIG",           vobsSTAR_POS_PARLX_TRIG);
//      meta->AddColumnMeta("e_Plx",        "POS_PARLX_TRIG_ERROR",     vobsSTAR_POS_PARLX_TRIG_ERROR);
        meta->AddColumnMeta("SpType",       "SPECT_TYPE_MK",            vobsSTAR_SPECT_TYPE_MK);
        meta->AddColumnMeta("Bmag",         "PHOT_JHN_B",               vobsSTAR_PHOT_JHN_B);
        meta->AddColumnMeta("Vmag",         "PHOT_JHN_V",               vobsSTAR_PHOT_JHN_V);
        meta->AddColumnMeta("v1",           "CODE_VARIAB",              vobsSTAR_CODE_VARIAB_V1);
        meta->AddColumnMeta("v2",           "CODE_VARIAB",              vobsSTAR_CODE_VARIAB_V2);
        meta->AddColumnMeta("v3",           "VAR_CLASS",                vobsSTAR_CODE_VARIAB_V3);
        meta->AddColumnMeta("d5",           "CODE_MULT_FLAG",           vobsSTAR_CODE_MULT_FLAG);
        AddCatalogMeta(meta);
        
        
        // BSC catalog ["V/50/catalog"]
        AddCatalogMeta(new vobsCATALOG_META("BSC", vobsCATALOG_BSC_ID));

        // CIO catalog ["II/225/catalog"] has multiple rows per target:
        AddCatalogMeta(new vobsCATALOG_META("CIO", vobsCATALOG_CIO_ID, 1.0, EPOCH_2000, EPOCH_2000, mcsFALSE, mcsTRUE));

        // DENIS catalog ["B/denis"] gives coordinates in various epochs (1995 - 2002) but has Julian date:
        AddCatalogMeta(new vobsCATALOG_META("DENIS", vobsCATALOG_DENIS_ID, 1.0, 1995.5, 2002.5));

        // DENIS_JK catalog ["J/A+A/413/1037/table1"] gives coordinates in various epochs (1995 - 2002) but has Julian date:
        AddCatalogMeta(new vobsCATALOG_META("DENIS-JK", vobsCATALOG_DENIS_JK_ID)); //, 1.0, 1995.5, 2002.5)); TODO ?

        // HIC catalog ["I/196/main"]
        AddCatalogMeta(new vobsCATALOG_META("HIC", vobsCATALOG_HIC_ID));

        // HIP2 catalog ["I/311/hip2"] gives precise coordinates and parallax in epoch 1991.25 (hip) and has proper motions:
        const char* overwriteIds [] = {vobsSTAR_POS_EQ_RA_MAIN, vobsSTAR_POS_EQ_DEC_MAIN,
                                       vobsSTAR_POS_EQ_RA_ERROR, vobsSTAR_POS_EQ_DEC_ERROR,
                                       vobsSTAR_POS_EQ_PMRA, vobsSTAR_POS_EQ_PMDEC,
                                       vobsSTAR_POS_EQ_PMRA_ERROR, vobsSTAR_POS_EQ_PMDEC_ERROR,
                                       vobsSTAR_POS_PARLX_TRIG, vobsSTAR_POS_PARLX_TRIG_ERROR};

        AddCatalogMeta(new vobsCATALOG_META("HIP2", vobsCATALOG_HIP2_ID, 1.0, 1991.25, 1991.25, mcsTRUE, mcsFALSE,
                                            vobsSTAR::GetPropertyMask(sizeof (overwriteIds) / sizeof (overwriteIds[0]), overwriteIds)));

        // LBSI catalog ["J/A+A/393/183/catalog"]
        AddCatalogMeta(new vobsCATALOG_META("LBSI", vobsCATALOG_LBSI_ID));

        // MERAND catalog ["J/A+A/433/1155"]
        AddCatalogMeta(new vobsCATALOG_META("MERAND", vobsCATALOG_MERAND_ID));

        // PHOTO catalog ["II/7A/catalog"] has multiple rows per target:
        AddCatalogMeta(new vobsCATALOG_META("PHOTO", vobsCATALOG_PHOTO_ID, 1.0, EPOCH_2000, EPOCH_2000, mcsFALSE, mcsTRUE));

        // SB9 catalog ["B/sb9/main"]
        AddCatalogMeta(new vobsCATALOG_META("SB9", vobsCATALOG_SB9_ID));

        // SBSC catalog ["V/36B/bsc4s"]
        AddCatalogMeta(new vobsCATALOG_META("SBSC", vobsCATALOG_SBSC_ID));

        // USNO catalog ["I/284"] has proper motion and is used by crossmatch with 2MASS (FAINT):
        AddCatalogMeta(new vobsCATALOG_META("USNO", vobsCATALOG_USNO_ID, 1.0, EPOCH_2000, EPOCH_2000, mcsTRUE));

        // WDS catalog ["B/wds/wds"]
        AddCatalogMeta(new vobsCATALOG_META("WDS", vobsCATALOG_WDS_ID));


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
