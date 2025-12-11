/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 *  Definition of vobsCATALOG_BADCAL_LOCAL class.
 */


/*
 * System Headers
 */
#include <iostream>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/stat.h>
using namespace std;

/*
 * MCS Headers
 */
#include "mcs.h"
#include "log.h"
#include "err.h"
#include "math.h"

/*
 * Local Headers
 */
#include "vobsCATALOG_BADCAL_LOCAL.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"

/*
 * To prevent concurrent access to shared ressources in multi-threaded context.
 */
static mcsMUTEX loadMutex = MCS_MUTEX_STATIC_INITIALIZER;

/**
 * Class constructor
 */
vobsCATALOG_BADCAL_LOCAL::vobsCATALOG_BADCAL_LOCAL() : vobsLOCAL_CATALOG(vobsCATALOG_BADCAL_LOCAL_ID,
                                                                         "$MCSDATA/tmp/badcal/badcal_list.dat"
                                                                         )
{
    // Initialize loaded time:
    _loaded_time = 0L;

    // Resolve real file path once:
    strcpy(_catalogFileName, _filename);

    // Resolve path
    char* resolvedPath = miscResolvePath(_catalogFileName);
    if (IS_NOT_NULL(resolvedPath))
    {
        strcpy(_catalogFileName, resolvedPath);
        free(resolvedPath);
    }
    else
    {
        _catalogFileName[0] = '\0';
    }

    // Preload at startup:
    Load(NULL);
}

/**
 * Class destructor
 */
vobsCATALOG_BADCAL_LOCAL::~vobsCATALOG_BADCAL_LOCAL()
{
}

/*
 * Public methods
 */

/*
 * Private methods
 */
mcsCOMPL_STAT vobsCATALOG_BADCAL_LOCAL::Search(vobsSCENARIO_RUNTIME &ctx,
                                               vobsREQUEST &request,
                                               vobsSTAR_LIST &list,
                                               const char* option,
                                               vobsCATALOG_STAR_PROPERTY_CATALOG_MAPPING* propertyCatalogMap,
                                               mcsLOGICAL logResult)
{
    return GetAll(list);
}

mcsCOMPL_STAT vobsCATALOG_BADCAL_LOCAL::GetAll(vobsSTAR_LIST &list)
{
    // purge given list to be able to add stars:
    list.Clear();

    mcsMutexLock(&loadMutex);

    // Load catalog in star list (to check timestamp)
    if (Load(NULL) == mcsSUCCESS)
    {
        // copy stars as targetId will be modified by Merge():
        list.Copy(_starList);
    }
    else
    {
        // Ignore failures
        logInfo("Badcal list is unavailable");
    }

    mcsMutexUnlock(&loadMutex);

    return mcsSUCCESS;
}

/**
 * Load BADCAL Local catalog.
 *
 * Build star list from BADCAL Local catalog stars.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsCATALOG_BADCAL_LOCAL::Load(vobsCATALOG_STAR_PROPERTY_CATALOG_MAPPING* propertyCatalogMap)
{
    struct stat stats;
    struct tm fileMod;
    time_t file_time;

    /* Test if file exists */
    if ((stat(_catalogFileName, &stats) == 0) && (stats.st_size > 0))
    {
        /* Use GMT date (up to second precision) */
        gmtime_r(&stats.st_mtime, &fileMod);
        file_time = mktime(&fileMod);
        logDebug("file_time: %ld", file_time);

        mcsDOUBLE elapsed = (mcsDOUBLE) difftime(file_time, _loaded_time);
        logDebug("File age: %.1lf seconds", elapsed);

        if (elapsed > 0.0)
        {
            // more recent
            Clear();
            // update file time:
            _loaded_time = file_time;
        }
    }
    else
    {
        logWarning("Badcal file not found at '%s'", _catalogFileName);
        return mcsFAILURE;
    }

    if (IS_FALSE(_loaded))
    {
        _starList.Clear();

        // Load catalog file
        FAIL(_starList.Load(_catalogFileName, GetCatalogMeta(), propertyCatalogMap, mcsFALSE, GetCatalogId()));

        // Set flag indicating a correct catalog load
        _loaded = mcsTRUE;

        // define catalog id / meta in star list:
        _starList.SetCatalogMeta(GetCatalogId(), GetCatalogMeta());

        // Fix coordinates RA/DEC if needed:
        const mcsUINT32 nbStars = _starList.Size();

        logTest("Fix RA/DEC format: [%d stars]", nbStars);

        vobsSTAR* star;
        mcsDOUBLE ra, dec;
        mcsSTRING16 raDeg, decDeg;
        mcsSTRING32 targetId;

        // For each calibrator of the list
        for (mcsUINT32 el = 0; el < nbStars; el++)
        {
            star = _starList.GetNextStar((mcsLOGICAL) (el == 0));

            if (IS_NOT_NULL(star))
            {
                FAIL(star->GetRaDec(ra, dec));

                vobsSTAR::raToDeg(ra, raDeg);
                vobsSTAR::decToDeg(dec, decDeg);

                // build targetId to make Merge() work
                snprintf(targetId, mcsLEN32, "%s%s", raDeg, decDeg);

                // Set queried identifier in the Target_ID column (= 'RaDec'):
                star->GetTargetIdProperty()->SetValue(targetId, vobsCATALOG_BADCAL_LOCAL_ID, vobsCONFIDENCE_HIGH, mcsTRUE);
            }
        }
        // Sort by declination to optimize CDS queries because spatial index(dec) is probably in use
        _starList.Sort(vobsSTAR_ID_TARGET);

        logInfo("CATALOG_BADCAL_LOCAL correctly loaded: %d stars", _starList.Size());
    }
    return mcsSUCCESS;
}

/*___oOo___*/
