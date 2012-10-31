/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 *  Definition of vobsCATALOG_LIST class.
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

/*
 * Local Headers 
 */
#include "vobsCATALOG_LIST.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"

/* initialize vobs module (vobsCATALOG meta data) */
void vobsInit()
{
    vobsGetVizierURI();

    // allocate property masks:
    vobsCATALOG_HIP2 _hip2;
}

/* clean vobs module on exit */
void vobsExit()
{
    vobsFreeVizierURI();
}

/**
 * Class constructor
 */
vobsCATALOG_LIST::vobsCATALOG_LIST()
{
    // The constructor add all catalogs in the map
    _catalogMap[vobsCATALOG_AKARI_ID] = &_akari;
    _catalogMap[vobsCATALOG_ASCC_ID] = &_ascc;
    _catalogMap[vobsCATALOG_ASCC_LOCAL_ID] = &_ascc_local;
    _catalogMap[vobsCATALOG_BSC_ID] = &_bsc;
    _catalogMap[vobsCATALOG_CIO_ID] = &_cio;
    _catalogMap[vobsCATALOG_PHOTO_ID] = &_photo;
    _catalogMap[vobsCATALOG_DENIS_ID] = &_denis;
    _catalogMap[vobsCATALOG_DENIS_JK_ID] = &_denisJK;
    _catalogMap[vobsCATALOG_HIC_ID] = &_hic;
    _catalogMap[vobsCATALOG_HIP2_ID] = &_hip2;
    _catalogMap[vobsCATALOG_LBSI_ID] = &_lbsi;
    _catalogMap[vobsCATALOG_MASS_ID] = &_mass;
    _catalogMap[vobsCATALOG_MERAND_ID] = &_merand;
    _catalogMap[vobsCATALOG_MIDI_ID] = &_midi;
    _catalogMap[vobsCATALOG_SBSC_ID] = &_sbsc;
    _catalogMap[vobsCATALOG_SB9_ID] = &_sb9;
    _catalogMap[vobsCATALOG_USNO_ID] = &_usno;
    _catalogMap[vobsCATALOG_WDS_ID] = &_wds;
}

/**
 * Class destructor
 */
vobsCATALOG_LIST::~vobsCATALOG_LIST()
{
    // Note: catalogList contains catalog pointers that are freed implicitely
}

/**
 * Get a catalog from its name
 *
 * @param catalogName the name of the catalog to get
 *
 * @return the corresponding catalog, NULL if not found.
 */
vobsCATALOG* vobsCATALOG_LIST::Get(const char* catalogName) const
{
    vobsCATALOG_PTR_MAP::const_iterator iter = _catalogMap.find(catalogName);

    // Check if catalog is present in the list
    if (iter == _catalogMap.end())
    {
        // Handle error
        errAdd(vobsERR_UNKNOWN_CATALOG_NAME, catalogName);
        return NULL;
    }

    return iter->second;
}

/*___oOo___*/
