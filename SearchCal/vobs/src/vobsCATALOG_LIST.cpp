/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsCATALOG_LIST.cpp,v 1.2 2005-11-16 10:47:54 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2005/11/15 14:56:43  scetre
 * Create catalog list
 *
 ******************************************************************************/

/**
 * @file
 *  Definition of vobsCATALOG_LIST class.
 */

static char *rcsId="@(#) $Id: vobsCATALOG_LIST.cpp,v 1.2 2005-11-16 10:47:54 scetre Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

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

/**
 * Class constructor
 */
vobsCATALOG_LIST::vobsCATALOG_LIST()
{
    // The constructor add all catalog in the map
    _catalogList[vobsCATALOG_ASCC_ID]=&_ascc;
    _catalogList[vobsCATALOG_BSC_ID]=&_bsc;
    _catalogList[vobsCATALOG_CHARM2_ID]=&_charm2;
    _catalogList[vobsCATALOG_CHARM_ID]=&_charm;
    _catalogList[vobsCATALOG_CIO_ID]=&_cio;
    _catalogList[vobsCATALOG_PHOTO_ID]=&_photo;
    _catalogList[vobsCATALOG_DENIS_ID]=&_denis;
    _catalogList[vobsCATALOG_DENIS_JK_ID]=&_denisJK;
    _catalogList[vobsCATALOG_HIC_ID]=&_hic;
    _catalogList[vobsCATALOG_LBSI_ID]=&_lbsi;
    _catalogList[vobsCATALOG_MASS_ID]=&_mass;
    _catalogList[vobsCATALOG_MERAND_ID]=&_merand;
    _catalogList[vobsCATALOG_MIDI_ID]=&_midi;
    _catalogList[vobsCATALOG_SBSC_ID]=&_sbsc;
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
    
    _catalogListIterator = _catalogList.find(catalogName);
    
    // Check if catalog is present in the list
    if (_catalogList.find(catalogName) == _catalogList.end())
    {
        // errAdd
        return NULL;
    }
    
    return (_catalogListIterator->second);
}

/*
 * Protected methods
 */


/*
 * Private methods
 */


/*___oOo___*/
