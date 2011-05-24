/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsCATALOG_LIST.cpp,v 1.10.2.2 2011-04-15 22:19:32 duvert Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.10.2.1  2011/04/08 19:43:03  duvert
 * Support for AKARI-related Photometries
 *
 * Revision 1.10  2010/01/26 14:15:38  lafrasse
 * Added vobsCATALOG_WDS.
 *
 * Revision 1.9  2009/05/04 15:20:26  lafrasse
 * Removed all references to unused CHARM catalog.
 *
 * Revision 1.8  2009/02/09 16:26:10  lafrasse
 * Added Spectral Binary catalog (SB9).
 *
 * Revision 1.7  2006/03/03 15:03:27  scetre
 * Changed rcsId to rcsId __attribute__ ((unused))
 *
 * Revision 1.6  2005/11/29 13:52:41  gzins
 * Use local iterator instead of class member which has been removed
 *
 * Revision 1.5  2005/11/29 10:32:32  gzins
 * Added error handling
 *
 * Revision 1.4  2005/11/24 14:54:25  scetre
 * UNSO catalog added
 *
 * Revision 1.3  2005/11/16 10:47:55  scetre
 * Updated documentation
 *
 * Revision 1.2  2005/11/16 10:47:54  scetre
 * Updated documentation
 *
 * Revision 1.1  2005/11/15 14:56:43  scetre
 * Create catalog list
 *
 ******************************************************************************/

/**
 * @file
 *  Definition of vobsCATALOG_LIST class.
 */

static char *rcsId __attribute__ ((unused)) ="@(#) $Id: vobsCATALOG_LIST.cpp,v 1.10.2.2 2011-04-15 22:19:32 duvert Exp $"; 

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
    _catalogList[vobsCATALOG_ASCC_ID]     = &_ascc;
    _catalogList[vobsCATALOG_ASCC_LOCAL_ID]     = &_ascc_local;
    _catalogList[vobsCATALOG_BSC_ID]      = &_bsc;
    _catalogList[vobsCATALOG_CHARM2_ID]   = &_charm2;
    _catalogList[vobsCATALOG_CIO_ID]      = &_cio;
    _catalogList[vobsCATALOG_PHOTO_ID]    = &_photo;
    _catalogList[vobsCATALOG_DENIS_ID]    = &_denis;
    _catalogList[vobsCATALOG_DENIS_JK_ID] = &_denisJK;
    _catalogList[vobsCATALOG_HIC_ID]      = &_hic;
    _catalogList[vobsCATALOG_LBSI_ID]     = &_lbsi;
    _catalogList[vobsCATALOG_MASS_ID]     = &_mass;
    _catalogList[vobsCATALOG_MERAND_ID]   = &_merand;
    _catalogList[vobsCATALOG_MIDI_ID]     = &_midi;
    _catalogList[vobsCATALOG_SBSC_ID]     = &_sbsc;
    _catalogList[vobsCATALOG_UNSO_ID]     = &_unso;
    _catalogList[vobsCATALOG_SB9_ID]      = &_sb9;
    _catalogList[vobsCATALOG_WDS_ID]      = &_wds;
    _catalogList[vobsCATALOG_AKARI_ID]    = &_akari;
}

/**
 * Class destructor
 */
vobsCATALOG_LIST::~vobsCATALOG_LIST()
{
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
vobsCATALOG *vobsCATALOG_LIST::Get(string catalogName)
{
    logTrace("vobsCATALOG_LIST::Get()");
    
    map<string, vobsCATALOG *>::iterator iter;
    iter = _catalogList.find(catalogName);
    
    // Check if catalog is present in the list
    if (iter == _catalogList.end())
    {
        // Hanlde error
        errAdd (vobsERR_UNKNOWN_CATALOG_NAME, catalogName.c_str());
        return NULL;
    }
    
    return (iter->second);
}

/*___oOo___*/
