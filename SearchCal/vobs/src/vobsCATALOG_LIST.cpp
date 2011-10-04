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


/**
 * Class constructor
 */
vobsCATALOG_LIST::vobsCATALOG_LIST()
{
    // The constructor add all catalogs in the map
    _catalogList[vobsCATALOG_AKARI_ID]      = &_akari;
    _catalogList[vobsCATALOG_ASCC_ID]       = &_ascc;
    _catalogList[vobsCATALOG_ASCC_LOCAL_ID] = &_ascc_local;
    _catalogList[vobsCATALOG_BSC_ID]        = &_bsc;
    _catalogList[vobsCATALOG_CHARM2_ID]     = &_charm2;
    _catalogList[vobsCATALOG_CIO_ID]        = &_cio;
    _catalogList[vobsCATALOG_PHOTO_ID]      = &_photo;
    _catalogList[vobsCATALOG_DENIS_ID]      = &_denis;
    _catalogList[vobsCATALOG_DENIS_JK_ID]   = &_denisJK;
    _catalogList[vobsCATALOG_HIC_ID]        = &_hic;
    _catalogList[vobsCATALOG_LBSI_ID]       = &_lbsi;
    _catalogList[vobsCATALOG_MASS_ID]       = &_mass;
    _catalogList[vobsCATALOG_MERAND_ID]     = &_merand;
    _catalogList[vobsCATALOG_MIDI_ID]       = &_midi;
    _catalogList[vobsCATALOG_SBSC_ID]       = &_sbsc;
    _catalogList[vobsCATALOG_SB9_ID]        = &_sb9;
    _catalogList[vobsCATALOG_UNSO_ID]       = &_unso;
    _catalogList[vobsCATALOG_WDS_ID]        = &_wds;
}

/**
 * Class destructor
 */
vobsCATALOG_LIST::~vobsCATALOG_LIST()
{
    // Note: catalogList contains catalog pointers that are freed implicitely
}

/*
 * Public methods
 */

/**
 * Get a catalog from its name
 *
 * @param catalogName the name of the catalog to get
 *
 * @return the corresponding catalog, NULL if not found.
 */
vobsCATALOG* vobsCATALOG_LIST::Get(const char* catalogName)
{
    logTrace("vobsCATALOG_LIST::Get()");
    
    CatalogList::iterator iter = _catalogList.find(catalogName);
    
    // Check if catalog is present in the list
    if (iter == _catalogList.end())
    {
        // Handle error
        errAdd (vobsERR_UNKNOWN_CATALOG_NAME, catalogName);
        return NULL;
    }
    
    return (iter->second);
}

/*___oOo___*/
