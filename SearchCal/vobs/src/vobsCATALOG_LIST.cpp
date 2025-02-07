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

/* pre-initialize vobs module (dev flag) */
void vobsPreInit()
{
    // Define the alx dev flag from env:
    alxSetDevFlag(vobsGetDevFlag());
    // Define the alx low memory flag from env:
    alxSetLowMemFlag(vobsGetLowMemFlag());
    // Define the alx deprecated flag from env:
    alxSetDeprecatedFlag(vobsGetDeprecatedFlag());
}

/* initialize vobs module (vobsCATALOG meta data) */
void vobsInit()
{
    vobsCancelInit();

    vobsGetVizierURI();

    // prepare catalog meta data:
    vobsCATALOG_LIST _catalogList;
}

/* clean vobs module on exit */
void vobsExit()
{
    // Free catalog meta data:
    vobsCATALOG::FreeCatalogMetaMap();

    vobsFreeVizierURI();

    vobsCancelExit();
}

/**
 * Class constructor
 */
vobsCATALOG_LIST::vobsCATALOG_LIST() :
_akari(vobsCATALOG_AKARI_ID),
_ascc(vobsCATALOG_ASCC_ID),
_jsdc_local(),
_jsdc_faint_local(),
_bsc(vobsCATALOG_BSC_ID),
_cio(vobsCATALOG_CIO_ID),
_denis(vobsCATALOG_DENIS_ID),
_denisJK(vobsCATALOG_DENIS_JK_ID),
_hic(vobsCATALOG_HIC_ID),
_hip1(vobsCATALOG_HIP1_ID),
_hip2(vobsCATALOG_HIP2_ID),
_mass(vobsCATALOG_MASS_ID),
_photo(vobsCATALOG_PHOTO_ID),
_sbsc(vobsCATALOG_SBSC_ID),
_sb9(vobsCATALOG_SB9_ID),
_usno(vobsCATALOG_USNO_ID),
_wds(vobsCATALOG_WDS_ID),
_wise(vobsCATALOG_WISE_ID),
_gaiaDR3(vobsCATALOG_GAIA_ID),
_gaiaDR3ap(vobsCATALOG_GAIA_AP_ID),
_mdfc(vobsCATALOG_MDFC_ID)
{
    // The constructor add all catalogs in the map
    _catalogMap[vobsCATALOG_AKARI_ID] = &_akari;
    _catalogMap[vobsCATALOG_ASCC_ID] = &_ascc;
    _catalogMap[vobsCATALOG_JSDC_LOCAL_ID] = &_jsdc_local;
    _catalogMap[vobsCATALOG_JSDC_FAINT_LOCAL_ID] = &_jsdc_faint_local;
    _catalogMap[vobsCATALOG_BSC_ID] = &_bsc;
    _catalogMap[vobsCATALOG_CIO_ID] = &_cio;
    _catalogMap[vobsCATALOG_DENIS_ID] = &_denis;
    _catalogMap[vobsCATALOG_DENIS_JK_ID] = &_denisJK;
    _catalogMap[vobsCATALOG_HIC_ID] = &_hic;
    _catalogMap[vobsCATALOG_HIP1_ID] = &_hip1;
    _catalogMap[vobsCATALOG_HIP2_ID] = &_hip2;
    _catalogMap[vobsCATALOG_MASS_ID] = &_mass;
    _catalogMap[vobsCATALOG_PHOTO_ID] = &_photo;
    _catalogMap[vobsCATALOG_SBSC_ID] = &_sbsc;
    _catalogMap[vobsCATALOG_SB9_ID] = &_sb9;
    _catalogMap[vobsCATALOG_USNO_ID] = &_usno;
    _catalogMap[vobsCATALOG_WDS_ID] = &_wds;
    _catalogMap[vobsCATALOG_WISE_ID] = &_wise;
    _catalogMap[vobsCATALOG_GAIA_ID] = &_gaiaDR3;
    _catalogMap[vobsCATALOG_GAIA_AP_ID] = &_gaiaDR3ap;
    _catalogMap[vobsCATALOG_MDFC_ID] = &_mdfc;
}

/**
 * Class destructor
 */
vobsCATALOG_LIST::~vobsCATALOG_LIST()
{
    // Note: catalogList contains catalog pointers that are freed implicitely
}

/**
 * Get a catalog from its identifier
 *
 * @param catalogId the identifier of the catalog to get
 *
 * @return the corresponding catalog, NULL if not found.
 */
vobsCATALOG* vobsCATALOG_LIST::Get(vobsORIGIN_INDEX catalogId) const
{
    vobsCATALOG_PTR_MAP::const_iterator iter = _catalogMap.find(catalogId);

    // Check if catalog is present in the list
    if (iter == _catalogMap.end())
    {
        // Handle error
        mcsSTRING16 tmp;
        sprintf(tmp, "id='%d'", catalogId);
        errAdd(vobsERR_UNKNOWN_CATALOG_NAME, tmp);
        return NULL;
    }

    return iter->second;
}

/*___oOo___*/
