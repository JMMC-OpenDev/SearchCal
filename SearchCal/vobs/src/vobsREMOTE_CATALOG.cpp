/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Definition vobsREMOTE_CATALOG class.
 */

/*
 * System Headers
 */
#include <iostream>
#include <stdlib.h>
using namespace std;
#include "pthread.h"


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
#include "vobsREMOTE_CATALOG.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"
#include "vobsSTAR.h"
#include "vobsPARSER.h"

/* max query size */
#define vobsMAX_QUERY_SIZE "1000"

/* size of chunks (1024 as WISE queries may be very slow) */
#define vobsCHUNK_QUERY_SIZE 1024

/* list size threshold to use chunks */
#define vobsTHRESHOLD_SIZE (2 * vobsCHUNK_QUERY_SIZE)

/*
 * Local Variables
 */
/** vizier URI environment variable */
static const mcsENVNAME vobsVizierUriEnvVarName = "VOBS_VIZIER_URI";
/** vizier URI CGI suffix */
static const char* vobsVizierUriSuffix = "/viz-bin/asu-xml?";
/** vizier URI in use */
static char* vobsVizierURI = NULL;

/** DEV Flag environment variable */
static const mcsENVNAME vobsDevFlagEnvVarName = "VOBS_DEV_FLAG";
/** development flag */
static mcsLOGICAL vobsDevFlag = mcsFALSE;
/** development flag initialization flag */
static mcsLOGICAL vobsDevFlagInitialized = mcsFALSE;


/** thread local storage key for cancel flag */
static pthread_key_t tlsKey_cancelFlag;
/** flag to indicate that the thread local storage is initialized */
static bool vobsCancelInitialized = false;

/* cancellation flag stored in thread local storage */
bool vobsIsCancelled(void)
{
    if (vobsCancelInitialized)
    {
        void* global = pthread_getspecific(tlsKey_cancelFlag);

        if (IS_NOT_NULL(global))
        {
            bool* cancelFlag = (bool*)global;

            // dirty read:
            bool cancelled = *cancelFlag;

            /*
             * Valgrind report:
            ==12272== Possible data race during read of size 1 at 0x9681920 by thread #5
            ==12272==    at 0x55A521C: vobsIsCancelled() (vobsREMOTE_CATALOG.cpp:76)
            ==12272== Possible data race during write of size 1 at 0x9681920 by thread #8
            ==12272==    at 0x51362C5: ns__GetCalCancelSession(soap*, char*, bool*) (sclwsWS.cpp:720)
             */

            logDebug("Reading cancel flag(%p): %s", cancelFlag, (cancelled) ? "true" : "false");

            if (cancelled)
            {
                logInfo("Reading cancel flag(%p): %s", cancelFlag, (cancelled) ? "true" : "false");
            }

            return cancelled;
        }
    }
    return false;
}

void vobsSetCancelFlag(bool* cancelFlag)
{
    if (vobsCancelInitialized && IS_NOT_NULL(cancelFlag))
    {
        void* global = pthread_getspecific(tlsKey_cancelFlag);

        if (IS_NULL(global))
        {
            logDebug("Setting cancel flag(%p)", cancelFlag);

            pthread_setspecific(tlsKey_cancelFlag, cancelFlag);
        }
    }
}

/* Thread Cancel Flag handling */
mcsCOMPL_STAT vobsCancelInit(void)
{
    logDebug("vobsCancelInit:  enable thread cancel flag support");

    const int rc = pthread_key_create(&tlsKey_cancelFlag, NULL); // no destructor
    if (rc != 0)
    {
        return mcsFAILURE;
    }

    vobsCancelInitialized = true;

    return mcsSUCCESS;
}

mcsCOMPL_STAT vobsCancelExit(void)
{
    logDebug("vobsCancelExit: disable thread cancel flag support");

    pthread_key_delete(tlsKey_cancelFlag);

    vobsCancelInitialized = false;

    return mcsSUCCESS;
}

/** Free the vizier URI */
void vobsFreeVizierURI()
{
    if (IS_NOT_NULL(vobsVizierURI))
    {
        free(vobsVizierURI);
        vobsVizierURI = NULL;
    }
}

/**
 * Get the vizier URI in use
 */
char* vobsGetVizierURI()
{
    if (IS_NOT_NULL(vobsVizierURI))
    {
        return vobsVizierURI;
    }
    // compute it once:

    mcsSTRING1024 uri;

    // 2022.10.12: new CDS domain = [cds.unistra.fr]:
    const char* uriVizier = "http://vizier.cds.unistra.fr"; // For production purpose
    // const char* uriVizier =  "http://viz-beta.cds.unistra.fr"; // For beta testing
    // const char* uriVizier = "http://vizier.cfa.harvard.edu";

    strcpy(uri, uriVizier);

    // Try to read ENV. VAR. to get port number to bind on
    mcsSTRING1024 envVizierUri = "";
    if (miscGetEnvVarValue2(vobsVizierUriEnvVarName, envVizierUri, sizeof (envVizierUri), mcsTRUE) == mcsSUCCESS)
    {
        // Check the env. var. is not empty
        if (strlen(envVizierUri) != 0)
        {
            logDebug("Found '%s' environment variable content for VIZIER URI.", vobsVizierUriEnvVarName);

            strncpy(uri, envVizierUri, sizeof (envVizierUri) - 1);
        }
        else
        {
            logWarning("'%s' environment variable does not contain a valid VIZIER URI (is empty), will use internal URI instead.", vobsVizierUriEnvVarName);
        }
    }
    else // else if the ENV. VAR. is not defined, do nothing (the default value is used instead).
    {
        logDebug("Could not read '%s' environment variable content for VIZIER URI, will use internal URI instead.", vobsVizierUriEnvVarName);
    }

    // Add VIZIER CGI suffix
    strncat(uri, vobsVizierUriSuffix, sizeof (envVizierUri) - 1);

    vobsVizierURI = miscDuplicateString(uri);

    logQuiet("Catalogs will get VIZIER data from '%s'", vobsVizierURI);

    return vobsVizierURI;
}

/* Return mcsTRUE if the development flag is enabled (env var ); mcsFALSE otherwise */
mcsLOGICAL vobsGetDevFlag()
{
    if (IS_TRUE(vobsDevFlagInitialized))
    {
        return vobsDevFlag;
    }
    // compute it once:
    vobsDevFlagInitialized = mcsTRUE;

    // Try to read ENV. VAR. to get port number to bind on
    mcsSTRING1024 envDevFlag = "";
    if (miscGetEnvVarValue2(vobsDevFlagEnvVarName, envDevFlag, sizeof (envDevFlag), mcsTRUE) == mcsSUCCESS)
    {
        // Check the env. var. is not empty
        if (strlen(envDevFlag) != 0)
        {
            logDebug("Found '%s' environment variable content for DEV_FLAG.", vobsDevFlagEnvVarName);

            if ((strcmp("1", envDevFlag) == 0) || (strcmp("true", envDevFlag) == 0))
            {
                vobsDevFlag = mcsTRUE;
            }
            else
            {
                logInfo("'%s' environment variable does not contain a valid DEV_FLAG: %s", vobsDevFlagEnvVarName, envDevFlag);
            }
        }
        else
        {
            logInfo("'%s' environment variable does not contain a valid DEV_FLAG (empty).", vobsDevFlagEnvVarName);
        }
    }

    logQuiet("vobsDevFlag: %s", IS_TRUE(vobsDevFlag) ? "true" : "false");

    return vobsDevFlag;
}

/*
 * Class constructor
 * @param name catalog identifier / name
 */
vobsREMOTE_CATALOG::vobsREMOTE_CATALOG(vobsORIGIN_INDEX catalogId) : vobsCATALOG(catalogId)
{
}

/*
 * Class destructor
 */
vobsREMOTE_CATALOG::~vobsREMOTE_CATALOG()
{
}


/*
 * Public methods
 */

/**
 * Search in the catalog a list of star according to a vobsREQUEST
 *
 * @param request a vobsREQUEST which have all the constraints for the search
 * @param list a vobsSTAR_LIST as the result of the search
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 *
 * \sa vobsREQUEST
 *
 * \b Errors codes:\n
 * The possible errors are:
 */
mcsCOMPL_STAT vobsREMOTE_CATALOG::Search(vobsSCENARIO_RUNTIME &ctx,
                                         vobsREQUEST &request,
                                         vobsSTAR_LIST &list,
                                         const char* option,
                                         vobsCATALOG_STAR_PROPERTY_CATALOG_MAPPING* propertyCatalogMap,
                                         mcsLOGICAL logResult)
{
    // Check cancellation:
    FAIL_COND(vobsIsCancelled());

    mcsUINT32 listSize = list.Size();

    // Prepare file name to log result of the catalog request
    mcsSTRING512 logFileName;
    // if the log level is higher or equal to the debug level
    if (IS_TRUE(logResult) || doLog(logDEBUG))
    {
        // Get band used for search
        const char* band = request.GetSearchBand();

        // build the first part of the file name in the MCSDATA directory
        sprintf(logFileName, "$MCSDATA/tmp/Cat_%s", band);

        // Get catalog name, and replace '/' by '_'
        mcsSTRING32 catalog;
        strcpy(catalog, GetName());

        miscReplaceChrByChr(catalog, '/', '_');
        strcat(logFileName, "_");
        strcat(logFileName, catalog);

        // Add request type (primary or not)
        strcat(logFileName, (listSize == 0) ? "_1.log" : "_2.log");

        // Resolve path
        char *resolvedPath;
        resolvedPath = miscResolvePath(logFileName);
        if (IS_NOT_NULL(resolvedPath))
        {
            strcpy(logFileName, resolvedPath);
            free(resolvedPath);
        }
    }
    else
    {
        memset((char *) logFileName, '\0', sizeof (logFileName));
    }

    // Prepare arguments:
    char* vizierURI = vobsGetVizierURI();
    const vobsCATALOG_META* catalogMeta = GetCatalogMeta();
    vobsORIGIN_INDEX catalogId = catalogMeta->GetCatalogId();

    // Reset and get the query buffer:
    miscoDYN_BUF* query = ctx.GetQueryBuffer();

    // Check if the list is empty
    // if ok, the asking is writing according to only the request
    if (listSize == 0)
    {
        FAIL(PrepareQuery(query, request, option));

        // The parser get the query result through Internet, and analyse it
        vobsPARSER parser;
        FAIL(parser.Parse(ctx, vizierURI, query->GetBuffer(), catalogId, catalogMeta, list, propertyCatalogMap, logFileName));

        // Check cancellation:
        FAIL_COND(vobsIsCancelled());

        // Perform post processing on catalog results (targetId mapping ...):
        if (list.Size() > 0)
        {
            FAIL(ProcessList(ctx, list));
        }
    }
    else
    {
        // else, the asking is writing according to the request and the star list
        if (listSize < vobsTHRESHOLD_SIZE)
        {
            FAIL(PrepareQuery(ctx, query, request, list, option));

            // The parser get the query result through Internet, and analyse it
            vobsPARSER parser;
            FAIL(parser.Parse(ctx, vizierURI, query->GetBuffer(), catalogId, catalogMeta, list, propertyCatalogMap, logFileName));

            // Check cancellation:
            FAIL_COND(vobsIsCancelled());

            // Perform post processing on catalog results (targetId mapping ...):
            if (list.Size() > 0)
            {
                FAIL(ProcessList(ctx, list));
            }
        }
        else
        {
            logTest("Search: list Size=%d, cutting in chunks of %d", listSize, vobsCHUNK_QUERY_SIZE);

            // shadow is a local copy of the input list:
            vobsSTAR_LIST shadow("Shadow");

            // just move stars into given list:
            shadow.CopyRefs(list);

            // purge given list to be able to add stars using CopyRefs(subset):
            list.Clear();

            // subset contains only star pointers (no copy):
            vobsSTAR_LIST subset("Subset");

            // define the free pointer flag to avoid double frees (shadow and subset are storing same star pointers):
            subset.SetFreeStarPointers(false);

            /*
             * Note: vobsPARSER::parse calls subset.Clear() that restore the free pointer flag to avoid memory leaks
             */

            mcsINT32 count = 0, total = 0, i = 0;

            vobsSTAR* currentStar = shadow.GetNextStar(mcsTRUE);

            while (IS_NOT_NULL(currentStar))
            {
                subset.AddRefAtTail(currentStar);

                count++;
                total++;

                if (count > vobsCHUNK_QUERY_SIZE)
                {
                    // define the free pointer flag to avoid double frees (shadow and subset are storing same star pointers):
                    subset.SetFreeStarPointers(false);

                    i++;

                    logTest("Search: Iteration %d = %d", i, total);

                    FAIL(PrepareQuery(ctx, query, request, subset, option));

                    // The parser get the query result through Internet, and analyse it
                    vobsPARSER parser;
                    FAIL(parser.Parse(ctx, vizierURI, query->GetBuffer(), catalogId, catalogMeta, subset, propertyCatalogMap, logFileName));

                    // Check cancellation:
                    FAIL_COND(vobsIsCancelled());

                    // Perform post processing on catalog results (targetId mapping ...):
                    if (subset.Size() > 0)
                    {
                        FAIL(ProcessList(ctx, subset));
                    }

                    // move stars into list:
                    // note: subset list was cleared by vobsPARSER.parse() so it manages star pointers now:
                    list.CopyRefs(subset);

                    // clear subset:
                    subset.Clear();

                    count = 0;
                }
                currentStar = shadow.GetNextStar();
            }

            // finish the list
            if (subset.Size() > 0)
            {
                // define the free pointer flag to avoid double frees (shadow and subset are storing same star pointers):
                subset.SetFreeStarPointers(false);

                FAIL(PrepareQuery(ctx, query, request, subset, option));

                // The parser get the query result through Internet, and analyse it
                vobsPARSER parser;
                FAIL(parser.Parse(ctx, vizierURI, query->GetBuffer(), catalogId, catalogMeta, subset, propertyCatalogMap, logFileName));

                // Check cancellation:
                FAIL_COND(vobsIsCancelled());

                // Perform post processing on catalog results (targetId mapping ...):
                if (subset.Size() > 0)
                {
                    FAIL(ProcessList(ctx, subset));
                }

                // move stars into list:
                // note: subset list was cleared by vobsPARSER.parse() so it manages star pointers now:
                list.CopyRefs(subset);

                // clear subset:
                subset.Clear();
            }

            // clear shadow list (explicit):
            shadow.Clear();
        }
    }

    return mcsSUCCESS;
}

/*
 * Private methods
 */

/**
 * Prepare the asking.
 *
 * Prepare the asking according to the request (constraints) for a first ask
 * to the CDS, that's mean that the use of this asking will help to have a
 * list of possible star
 *
 * @param request vobsREQUEST which have all the constraints for the search
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsREMOTE_CATALOG::PrepareQuery(miscoDYN_BUF* query, vobsREQUEST &request, const char* option)
{
    // reset query buffer:
    FAIL(query->Reset());

    // in this case of request, there are three parts to write :
    // the location
    // the position of the reference star
    // the specific part of the query
    FAIL(WriteQueryURIPart(query));

    FAIL(WriteReferenceStarPosition(query, request));

    FAIL(WriteQuerySpecificPart(query, request));

    // options:
    FAIL(WriteOption(query, option));

    // properties to retrieve
    FAIL(WriteQuerySpecificPart(query));

    return mcsSUCCESS;
}

/**
 * Prepare the asking.
 *
 * Prepare the asking according to the request (constraints). The knowledge of
 * another list of star help to create the asking for a final ask to the CDS.
 *
 * @param request vobsREQUEST which have all the constraints for the search
 * @param tmpList vobsSTAR_LIST which come from an older ask to the CDS.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsREMOTE_CATALOG::PrepareQuery(vobsSCENARIO_RUNTIME &ctx,
                                               miscoDYN_BUF* query,
                                               vobsREQUEST &request,
                                               vobsSTAR_LIST &tmpList,
                                               const char* option)
{
    // reset query buffer:
    FAIL(query->Reset());

    // in this case of request, there are four parts to write :
    // the location
    // the constant part of the query
    // the specific part of the query
    // the list to complete
    FAIL(WriteQueryURIPart(query));

    FAIL(WriteQueryConstantPart(query, request, tmpList));

    // options:
    FAIL(WriteOption(query, option));

    // properties to retrieve
    FAIL(WriteQuerySpecificPart(query));

    FAIL(WriteQueryStarListPart(ctx, query, tmpList));

    return mcsSUCCESS;
}

/**
 * Build the destination part of the query->
 *
 * Build the destination part of the query-> All catalog files are located on
 * web server. It is possible to find them on the URL :
 * http://vizier.u-strasbg.fr/viz-bin/asu-xml?-source= ...
 * * &-out.meta=hudU1&-oc.form=sexa has been added to get previous UCD1 instead
 * of UCD1+ with the
 *  * rest of information
 *   * more info found here http://cdsweb.u-strasbg.fr/doc/asu-summary.htx
 *  -oc.form forces rigth coordinates h:m:s (dispite given param -oc=hms)
 *  -out.meta=
 *    h -> add column names into cdata header (required by our parser)
 *    u -> retrieve column units as viz1bin used to do by default
 *    d -> retrieve column descriptions as viz1bin used to do by default
 *    U1 -> request ucd1 instead of ucd1+
 *
 * Adds common part = MAX=... and compute _RAJ2000 and _DEJ2000 (HMS)
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsREMOTE_CATALOG::WriteQueryURIPart(miscoDYN_BUF* query)
{
    query->AppendString("-source=");
    query->AppendString(GetName());
    query->AppendString("&-out.meta=hudU1&-oc.form=sexa");

    // add common part: MAX=... and compute _RAJ2000 and _DEJ2000 (HMS)
    query->AppendString("&-c.eq=J2000");

    // Get the computed right ascension (J2000 / epoch 2000 in HMS) _RAJ2000 (POS_EQ_RA_MAIN) stored in the 'vobsSTAR_POS_EQ_RA_MAIN' property
    query->AppendString("&-out.add=");
    query->AppendString(vobsCATALOG___RAJ2000);
    // Get the computed declination (J2000 / epoch 2000 in DMS)     _DEJ2000 (POS_EQ_DEC_MAIN) stored in the 'vobsSTAR_POS_EQ_DEC_MAIN' property
    query->AppendString("&-out.add=");
    query->AppendString(vobsCATALOG___DEJ2000);

    query->AppendString("&-oc=hms");
    query->AppendString("&-out.max=");
    query->AppendString(vobsMAX_QUERY_SIZE);

    if (IS_TRUE(GetCatalogMeta()->DoSortByDistance()))
    {
        // order results by distance
        query->AppendString("&-sort=_r");
    }

    return mcsSUCCESS;
}

/**
 * Build the constant part of the asking
 *
 * Build the constant part of the asking. For each catalog, a part of the
 * asking is the same.
 *
 * @param request vobsREQUEST which help to get the search radius
 * @param tmpList vobsSTAR_LIST which come from an older ask to the CDS.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsREMOTE_CATALOG::WriteQueryConstantPart(miscoDYN_BUF* query, vobsREQUEST &request, vobsSTAR_LIST &tmpList)
{
    bool useBox = false;
    mcsSTRING16 separation;

    // Get cone search radius:
    mcsDOUBLE radius = request.GetConeSearchRadius();
    if (radius > 0.0)
    {
        logTest("Search: input list [%s] catalog id: '%s'", tmpList.GetName(), tmpList.GetCatalogName());
        logTest("Search: catalog id: '%s'", GetCatalogMeta()->GetName());

        if (GetCatalogMeta()->IsSingleEpoch())
        {
            if (IS_NOT_NULL(tmpList.GetCatalogMeta()) && IS_TRUE(tmpList.GetCatalogMeta()->DoPrecessEpoch()))
            {
                // Need to expand radius to get enough candidates from 2MASS to (ASCC, HIP):
                mcsDOUBLE avgRadius = 0.0;

                FAIL(GetAverageEpochSearchRadius(tmpList, avgRadius))

                radius += avgRadius * 1.05 + 0.05; // 5% higher
            }
            else
            {
                // use radius in arcsec useful to keep only wanted stars (close to the reference star)
                radius += 0.05; // for rounding purposes
            }

            sprintf(separation, "%.1lf", radius);
        }
        else
        {
            // Need to expand radius to get enough candidates (2MASS):
            useBox = true;

            // Adapt search area according to star's proper motion (epoch):
            mcsDOUBLE deltaRa = 0.0;
            mcsDOUBLE deltaDec = 0.0;

            FAIL(GetEpochSearchArea(tmpList, deltaRa, deltaDec))

            // length = maxMove (5% higher) + radius (margin):
            deltaRa = deltaRa * 1.05 + radius + 0.05; // for rounding purposes
            deltaDec = deltaDec * 1.05 + radius + 0.05; // for rounding purposes

            // use box area:
            sprintf(separation, "%.1lf/%.1lf", deltaRa, deltaDec);
        }
    }
    else
    {
        // cone search with radius = 5 arcsec by default
        strcpy(separation, "5");
    }

    // note: internal crossmatch are performed using RA/DEC range up to few arcsec:
    if (useBox)
    {
        logTest("Search: Box search area=%s arcsec", separation);

        query->AppendString("&-c.geom=b&-c.bs="); // -c.bs means box in arcsec
    }
    else
    {
        logTest("Search: Cone search area=%s arcsec", separation);

        query->AppendString("&-c.rs="); // -c.rs means radius in arcsec
    }
    query->AppendString(separation);

    // Get the given star coordinates to CDS (RA+DEC) _1 (ID_TARGET) stored in the 'vobsSTAR_ID_TARGET' property
    // for example: '016.417537-41.369444'
    query->AppendString("&-out.add=");
    query->AppendString(vobsCATALOG___TARGET_ID);

    query->AppendString("&-file=-c");

    return mcsSUCCESS;
}

/**
 * Build the specific part of the asking.
 *
 * Build the specific part of the asking. This is the part of the asking
 * which is write specifically for each catalog. The constraints of the request
 * which help to build an asking in order to restrict the research.
 *
 * @param request vobsREQUEST which help to restrict the search
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsREMOTE_CATALOG::WriteQuerySpecificPart(miscoDYN_BUF* query, vobsREQUEST &request)
{
    bool useBox = false;
    mcsSTRING32 separation;

    // Add search geometry constraints:
    if (request.GetSearchAreaGeometry() == vobsBOX)
    {
        useBox = true;

        // Get search box size
        mcsDOUBLE deltaRa, deltaDec;
        FAIL(request.GetSearchArea(deltaRa, deltaDec));

        deltaRa  += 0.5; // for rounding purposes
        deltaDec += 0.5; // for rounding purposes

        // use box area:
        sprintf(separation, "%.0lf/%.0lf", deltaRa, deltaDec);
    }
    else
    {
        // Get search radius
        mcsDOUBLE radius;
        FAIL(request.GetSearchArea(radius));

        radius += 0.5; // for rounding purposes
        sprintf(separation, "%.0lf", radius);
    }

    // Add query constraints:
    if (useBox)
    {
        logTest("Search: Box search area=%s arcmin", separation);

        query->AppendString("&-c.geom=b&-c.bm="); // -c.bm means box in arcmin
    }
    else
    {
        logTest("Search: Cone search area=%s arcmin", separation);

        query->AppendString("&-c.rm="); // -c.rm means radius in arcmin
    }
    query->AppendString(separation);

    // Add the magnitude range constraint on the requested band:
    const char* band = request.GetSearchBand();

    mcsSTRING32 rangeMag;
    sprintf(rangeMag, "%.2lf..%.2lf", request.GetMinMagRange(), request.GetMaxMagRange());

    logTest("Search: Magnitude %s range=%s", band, rangeMag);

    FAIL(WriteQueryBandPart(query, band, rangeMag));

    return mcsSUCCESS;
}

/**
 * Build the band constraint part of the asking.
 *
 * @param band requested band
 * @param rangeMag magnitude range constraint ("%.2lf..%.2lf")
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsREMOTE_CATALOG::WriteQueryBandPart(miscoDYN_BUF* query, const char* band, mcsSTRING32 &rangeMag)
{
    // Add the magnitude range constraint on the requested band:
    query->AppendString("&");
    query->AppendString(band);
    query->AppendString("mag=");
    query->AppendString(rangeMag);

    return mcsSUCCESS;
}

/**
 * Build the specific part of the asking.
 *
 * Build the specific part of the asking. This is the part of the asking
 * which is write specifically for each catalog.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsREMOTE_CATALOG::WriteQuerySpecificPart(miscoDYN_BUF* query)
{
    // Write query to get catalog columns:
    const vobsCATALOG_COLUMN_PTR_LIST columnList = GetCatalogMeta()->GetColumnList();

    const char* id;
    for (vobsCATALOG_COLUMN_PTR_LIST::const_iterator iter = columnList.begin(); iter != columnList.end(); iter++)
    {
        id = (*iter)->GetId();

        // skip columns already added in query by WriteQueryURIPart() and WriteQueryConstantPart()
        if ((strcmp(id, vobsCATALOG___RAJ2000) != 0) && (strcmp(id, vobsCATALOG___DEJ2000) != 0)
                && (strcmp(id, vobsCATALOG___TARGET_ID) != 0))
        {
            query->AppendString("&-out=");
            query->AppendString(id);
        }
    }

    return mcsSUCCESS;
}

/**
 * Build the position box part of the asking.
 *
 * Build the position box part of the asking. This method is used in case of
 * restrictive search.
 *
 * @param request vobsREQUEST which help to restrict the search
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsREMOTE_CATALOG::WriteReferenceStarPosition(miscoDYN_BUF* query, vobsREQUEST &request)
{
    mcsDOUBLE ra, dec;
    mcsDOUBLE pmRa, pmDec;
    mcsSTRING16 raDeg, decDeg;

    ra = request.GetObjectRaInDeg();
    dec = request.GetObjectDecInDeg();

    if (IS_FALSE(GetCatalogMeta()->IsEpoch2000()))
    {
        // proper motion (mas/yr):
        // TODO: let sclgui provide pmRA / pmDec using star resolver (SIMBAD) info:
        pmRa = request.GetPmRa();
        pmDec = request.GetPmDec();

        // ra/dec coordinates are corrected to the catalog's epoch:

        // TODO: test that case (not getstar but primary requests ??)
        const mcsDOUBLE epochMed = GetCatalogMeta()->GetEpochMedian();

        ra = vobsSTAR::GetPrecessedRA(ra, pmRa, EPOCH_2000, epochMed);
        dec = vobsSTAR::GetPrecessedDEC(dec, pmDec, EPOCH_2000, epochMed);
    }

    vobsSTAR::raToDeg(ra, raDeg);
    vobsSTAR::decToDeg(dec, decDeg);

    // Add encoded RA/Dec (decimal degrees) in query -c=005.940325+12.582441
    query->AppendString("&-c=");
    query->AppendString(raDeg);

    if (decDeg[0] == '+')
    {
        query->AppendString("%2b");
        query->AppendString(&decDeg[1]);
    }
    else
    {
        query->AppendString(decDeg);
    }

    return mcsSUCCESS;
}

/**
 * Build the end part of the asking.
 *
 * The end part of the asking for a search from a star list.
 *
 * @param list vobsSTAR_LIST which help to build the end part
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsREMOTE_CATALOG::WriteQueryStarListPart(vobsSCENARIO_RUNTIME &ctx, miscoDYN_BUF* query, vobsSTAR_LIST &list)
{
    query->AppendString("&-out.form=List");

    // write a star list object as a dynamic buffer in order to write it in a
    // string format in the query
    FAIL_DO(StarList2String(ctx, query, list),
            logError("An Error occurred when converting the input star list to string (RA/DEC coordinates) !"));

    return mcsSUCCESS;
}

/**
 * Write option
 *
 * @return always mcsSUCCESS
 */
mcsCOMPL_STAT vobsREMOTE_CATALOG::WriteOption(miscoDYN_BUF* query, const char* option)
{
    // Write optional catalog meta's query option:
    const char* queryOption = GetCatalogMeta()->GetQueryOption();
    if (IS_NOT_NULL(queryOption))
    {
        query->AppendString(queryOption);
    }

    // Write optional scenario's query option:
    if (IS_NOT_NULL(option))
    {
        query->AppendString(option);
    }

    return mcsSUCCESS;
}

/**
 * Convert a star list to a string list.
 *
 * The research of specific star knowing their coordinates need to write in the
 * asking the list of coordinate as a string. This method convert the position
 * of all star present in a star list in a string.
 *
 * @param strList string list as a string
 * @param list star list to convert
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsREMOTE_CATALOG::StarList2String(vobsSCENARIO_RUNTIME &ctx,
                                                  miscoDYN_BUF* query,
                                                  const vobsSTAR_LIST &list)
{
    const mcsUINT32 nbStars = list.Size();

    // if the list is not empty
    if (nbStars != 0)
    {
        const bool isLogDebug = doLog(logDEBUG);

        // Compute the number of bytes by which the Dynamic Buffer should be expanded and allocate them
        /* buffer capacity = fixed (50) + dynamic (nbStars x 24) */
        FAIL(query->Reserve(50 + 24 * nbStars));

        // Start the List argument -c=<<====LIST&
        query->AppendString("&-c=%3C%3C%3D%3D%3D%3DLIST&");

        mcsDOUBLE ra, dec;
        mcsSTRING16 raDeg, decDeg;

        const bool doPrecess = IS_FALSE(GetCatalogMeta()->IsEpoch2000());
        const mcsDOUBLE epochMed = GetCatalogMeta()->GetEpochMedian();

        vobsTARGET_ID_MAPPING* targetIdIndex = NULL;

        if (doPrecess)
        {
            // Prepare the targetId index:
            targetIdIndex = ctx.GetTargetIdIndex();
            // clear if needed:
            ctx.ClearTargetIdIndex();
        }

        // line buffer to avoid too many calls to dynamic buf:
        // Note: 48 bytes is large enough to contain one line
        // No buffer overflow checks !

        char *targetIdFrom, *targetTo;
        mcsSTRING48 value;
        char* valPtr;
        vobsSTAR* star;

        for (mcsUINT32 el = 0; el < nbStars; el++)
        {
            if (el == 0)
            {
                value[0] = '\0';
                // reset value pointer:
                valPtr = value;
            }
            else
            {
                strcpy(value, "&+");
                // reset value pointer:
                valPtr = value + 2;
            }

            // Get next star
            star = list.GetNextStar((mcsLOGICAL) (el == 0));

            FAIL(star->GetRaDec(ra, dec));

            vobsSTAR::raToDeg(ra, raDeg);
            vobsSTAR::decToDeg(dec, decDeg);

            if (doPrecess)
            {
                targetIdFrom = ctx.GetTargetId();
                strcpy(targetIdFrom, raDeg);
                strcat(targetIdFrom, decDeg);

                // ra/dec coordinates are corrected to the catalog's epoch:
                FAIL(star->PrecessRaDecJ2000ToEpoch(epochMed, ra, dec));

                vobsSTAR::raToDeg(ra, raDeg);
                vobsSTAR::decToDeg(dec, decDeg);

                targetTo = ctx.GetTargetId();
                strcpy(targetTo, raDeg);
                strcat(targetTo, decDeg);

                if (isLogDebug)
                {
                    logDebug("targetId      '%s'", targetTo);
                    logDebug("targetIdJ2000 '%s'", targetIdFrom);
                }

                targetIdIndex->insert(vobsTARGET_ID_PAIR(targetTo, targetIdFrom));
            }

            // Add encoded RA/Dec (decimal degrees) in query 005.940325+12.582441
            vobsStrcatFast(valPtr, raDeg);

            if (decDeg[0] == '+')
            {
                vobsStrcatFast(valPtr, "%2b");
                vobsStrcatFast(valPtr, &decDeg[1]);
            }
            else
            {
                vobsStrcatFast(valPtr, decDeg);
            }

            query->AppendString(value);
        }

        // Close the List argument &====LIST
        query->AppendString("&%3D%3D%3D%3DLIST");
    }

    return mcsSUCCESS;
}

mcsCOMPL_STAT vobsREMOTE_CATALOG::GetEpochSearchArea(const vobsSTAR_LIST &list, mcsDOUBLE &deltaRA, mcsDOUBLE &deltaDEC)
{
    const mcsUINT32 nbStars = list.Size();

    mcsDOUBLE deltaRa = 0.0;
    mcsDOUBLE deltaDec = 0.0;

    // if the list is not empty
    if (nbStars != 0)
    {
        vobsSTAR* star;
        mcsDOUBLE pmRa, pmDec; // mas/yr

        const mcsDOUBLE deltaEpoch = GetCatalogMeta()->GetEpochDelta();

        for (mcsUINT32 el = 0; el < nbStars; el++)
        {
            // Get next star
            star = list.GetNextStar((mcsLOGICAL) (el == 0));

            if (IS_NOT_NULL(star))
            {
                FAIL(star->GetPmRaDec(pmRa, pmDec));

                deltaRa = alxMax(deltaRa, fabs(vobsSTAR::GetDeltaRA(pmRa, deltaEpoch)));
                deltaDec = alxMax(deltaDec, fabs(vobsSTAR::GetDeltaDEC(pmDec, deltaEpoch)));
            }
        }

        deltaRa *= alxDEG_IN_ARCSEC;
        deltaDec *= alxDEG_IN_ARCSEC;

        logDebug("delta Ra/Dec: %lf %lf", deltaRa, deltaDec);
    }

    deltaRA = deltaRa;
    deltaDEC = deltaDec;

    return mcsSUCCESS;
}

mcsCOMPL_STAT vobsREMOTE_CATALOG::GetAverageEpochSearchRadius(const vobsSTAR_LIST &list, mcsDOUBLE &radius)
{
    // TODO: should deal with pmRA around poles ? ie take into account the declination of each star ?

    // Typical case: ASCC or USNO query (1991.25) with 2MASS stars (1997 - 2001) as input (FAINT)

    const static mcsDOUBLE maxProperMotion = 2.0; // 2 arcsec/yr

    const mcsUINT32 nbStars = list.Size();

    mcsDOUBLE deltaEpoch = 0.0;

    // if the list is not empty
    if (nbStars != 0)
    {
        mcsDOUBLE jdDate, epoch;
        vobsSTAR* star;

        const mcsDOUBLE epochMed = GetCatalogMeta()->GetEpochMedian();

        for (mcsUINT32 el = 0; el < nbStars; el++)
        {
            // Get next star
            star = list.GetNextStar((mcsLOGICAL) (el == 0));

            if (IS_NOT_NULL(star))
            {
                jdDate = star->GetJdDate();

                if (jdDate != -1.0)
                {
                    // 2MASS observation epoch:
                    epoch = EPOCH_2000 + (jdDate - JD_2000) / 365.25;

                    epoch -= epochMed; // minus ASCC epoch

                    if (epoch < 0.0)
                    {
                        epoch = -epoch;
                    }

                    if (deltaEpoch < epoch)
                    {
                        deltaEpoch = epoch;
                    }
                }
                else
                {
                    logWarning("GetAverageEpochSearchRadius: not implemented !");
                    return mcsFAILURE;
                }
            }
        }

        logDebug("deltaEpoch: %lf", deltaEpoch);
    }

    radius = deltaEpoch * maxProperMotion;

    return mcsSUCCESS;
}

/**
 * Method to process optionally the output star list from the catalog
 *
 * @param list a vobsSTAR_LIST as the result of the search
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsREMOTE_CATALOG::ProcessList(vobsSCENARIO_RUNTIME &ctx, vobsSTAR_LIST &list)
{
    const mcsUINT32 listSize = list.Size();
    if (listSize > 0)
    {
        logDebug("ProcessList: list Size = %d", listSize);

        if (IS_FALSE(GetCatalogMeta()->IsEpoch2000()))
        {
            const bool isLogDebug = doLog(logDEBUG);

            // fix target Id column by using a map<string, string> to fix epoch to J2000
            vobsTARGET_ID_MAPPING* targetIdIndex = ctx.GetTargetIdIndex();

            if (targetIdIndex->size() > 0)
            {
                // For each star of the given star list
                vobsSTAR* star = NULL;
                vobsSTAR_PROPERTY* targetIdProperty;
                const char *targetIdJ2000, *targetId;
                vobsTARGET_ID_MAPPING::iterator it;

                // For each star of the list
                for (star = list.GetNextStar(mcsTRUE); IS_NOT_NULL(star); star = list.GetNextStar(mcsFALSE))
                {
                    targetIdProperty = star->GetTargetIdProperty();

                    // test if property is set
                    if (IS_TRUE(targetIdProperty->IsSet()))
                    {
                        targetId = targetIdProperty->GetValue();

                        if (isLogDebug)
                        {
                            logDebug("targetId      '%s'", targetId);
                        }

                        // explicit cast to char*
                        it = targetIdIndex->find((char*) targetId);

                        if (it == targetIdIndex->end())
                        {
                            logInfo("targetId not found: '%s'", targetId);
                        }
                        else
                        {
                            targetIdJ2000 = it->second;

                            if (isLogDebug)
                            {
                                logDebug("targetIdJ2000 '%s'", targetIdJ2000);
                            }

                            FAIL(targetIdProperty->SetValue(targetIdJ2000, targetIdProperty->GetOriginIndex(), vobsCONFIDENCE_HIGH, mcsTRUE));
                        }
                    }
                }

                // clear targetId index:
                ctx.ClearTargetIdIndex();
            }
        }
    }
    return mcsSUCCESS;
}

/*
 * Catalog Post Processing (data)
 */
mcsCOMPL_STAT ProcessList_DENIS(vobsSTAR_LIST &list);
mcsCOMPL_STAT ProcessList_HIP1(vobsSTAR_LIST &list);
mcsCOMPL_STAT ProcessList_MASS(vobsSTAR_LIST &list);
mcsCOMPL_STAT ProcessList_WISE(vobsSTAR_LIST &list);
mcsCOMPL_STAT ProcessList_MDFC(vobsSTAR_LIST &list);

/**
 * Method to process optionally the output star list from the catalog
 *
 * @param list a vobsSTAR_LIST as the result of the search
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsREMOTE_CATALOG::PostProcessList(vobsSTAR_LIST &list)
{
    const mcsUINT32 listSize = list.Size();
    if (listSize > 0)
    {
        logDebug("ProcessList: list Size = %d", listSize);

        vobsORIGIN_INDEX originIndex = GetCatalogId();

        // Perform custom post processing:
        if (isCatalog2Mass(originIndex))
        {
            ProcessList_MASS(list);
        }
        else if (isCatalogWise(originIndex))
        {
            ProcessList_WISE(list);
        }
        else if (vobsCATALOG_DENIS_ID_ENABLE && isCatalogDenis(originIndex))
        {
            ProcessList_DENIS(list);
        }
        else if (isCatalogHip1(originIndex))
        {
            ProcessList_HIP1(list);
        } 
        else if (isCatalogMdfc(originIndex))
        {
            ProcessList_MDFC(list);
        }
        // TODO: DENIS_JK (JD) ??
    }
    return mcsSUCCESS;
}

/**
 * Method to process the output star list from the DENIS catalog
 *
 * @param list a vobsSTAR_LIST as the result of the search
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT ProcessList_DENIS(vobsSTAR_LIST &list)
{
    logInfo("ProcessList_DENIS: list Size=%d", list.Size());

    // Check flag related to I magnitude
    // Note (2):
    // This flag is the concatenation of image and source flags, in hexadecimal format.
    // For the image flag, the first two digits contain:
    // Bit 0 (0100) clouds during observation
    // Bit 1 (0200) electronic Read-Out problem
    // Bit 2 (0400) internal temperature problem
    // Bit 3 (0800) very bright star
    // Bit 4 (1000) bright star
    // Bit 5 (2000) stray light
    // Bit 6 (4000) unknown problem
    // For the source flag, the last two digits contain:
    // Bit 0 (0001) source might be a dust on mirror
    // Bit 1 (0002) source is a ghost detection of a bright star
    // Bit 2 (0004) source is saturated
    // Bit 3 (0008) source is multiple detect
    // Bit 4 (0010) reserved

    const mcsINT32 idIdx = vobsSTAR::GetPropertyIndex(vobsSTAR_ID_DENIS);
    const mcsINT32 iFlagIdx = vobsSTAR::GetPropertyIndex(vobsSTAR_CODE_MISC_I);
    const mcsINT32 magIcIdx = vobsSTAR::GetPropertyIndex(vobsSTAR_PHOT_COUS_I);

    vobsSTAR_PROPERTY *iFlagProperty, *magIcProperty;
    vobsSTAR* star = NULL;
    const char *starId, *code;
    mcsUINT32 iFlag;

    // For each star of the list
    for (star = list.GetNextStar(mcsTRUE); IS_NOT_NULL(star); star = list.GetNextStar(mcsFALSE))
    {
        // Get the star ID (logs)
        starId = star->GetProperty(idIdx)->GetValueOrBlank();

        // Get Imag property:
        magIcProperty = star->GetProperty(magIcIdx);

        // Get I Flag property:
        iFlagProperty = star->GetProperty(iFlagIdx);

        // test if property is set
        if (IS_TRUE(magIcProperty->IsSet()) && IS_TRUE(iFlagProperty->IsSet()))
        {
            // Check if it is saturated or there was clouds during observation

            // Get Iflg value as string
            code = iFlagProperty->GetValue();

            // Convert it into integer; hexadecimal conversion
            sscanf(code, "%x", &iFlag);

            // discard all flagged observation
            if (iFlag != 0)
            {
                logTest("Star 'DENIS %s' - discard I Cousin magnitude (saturated or clouds - Iflg='%s')", starId, code);

                // TODO: use confidence index instead of clearing values BUT allow overwriting of low confidence index values
                magIcProperty->ClearValue();
            }
        }
    }

    return mcsSUCCESS;
}

/**
 * Method to process the output star list from the catalog HIP1
 *
 * @param list a vobsSTAR_LIST as the result of the search
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT ProcessList_HIP1(vobsSTAR_LIST &list)
{
    logInfo("ProcessList_HIP1: list Size=%d", list.Size());

    const mcsINT32 idIdx = vobsSTAR::GetPropertyIndex(vobsSTAR_ID_HIP);
    const mcsINT32 mVIdx = vobsSTAR::GetPropertyIndex(vobsSTAR_PHOT_JHN_V);
    const mcsINT32 mB_VIdx = vobsSTAR::GetPropertyIndex(vobsSTAR_PHOT_JHN_B_V);
    const mcsINT32 mV_IcIdx = vobsSTAR::GetPropertyIndex(vobsSTAR_PHOT_COUS_V_I);
    const mcsINT32 rV_IcIdx = vobsSTAR::GetPropertyIndex(vobsSTAR_PHOT_COUS_V_I_REFER_CODE);

    vobsSTAR_PROPERTY *mVProperty, *mB_VProperty, *mV_IcProperty, *rV_IcProperty;
    vobsSTAR* star = NULL;
    const char *starId, *code;
    char ch;

    mcsDOUBLE mV, eV, mB_V, eB_V, mV_Ic, eV_Ic;
    mcsDOUBLE mB, eB, mIc, eIc;

    vobsCONFIDENCE_INDEX confidenceIc;

    // For each star of the list
    for (star = list.GetNextStar(mcsTRUE); IS_NOT_NULL(star); star = list.GetNextStar(mcsFALSE))
    {
        // Get the star ID (logs)
        starId = star->GetProperty(idIdx)->GetValueOrBlank();

        // Get V property:
        mVProperty = star->GetProperty(mVIdx);

        if (IS_TRUE(mVProperty->IsSet()))
        {
            FAIL(mVProperty->GetValue(&mV));
            // Use NaN to avoid using undefined error:
            FAIL(star->GetPropertyErrorOrDefault(mVProperty, &eV, NAN));

            // Get B-V property:
            mB_VProperty = star->GetProperty(mB_VIdx);

            // test if property is set
            if (IS_TRUE(mB_VProperty->IsSet()))
            {
                FAIL(mB_VProperty->GetValue(&mB_V));
                // Use NaN to avoid using undefined error:
                FAIL(star->GetPropertyErrorOrDefault(mB_VProperty, &eB_V, NAN));

                /*
                 * Compute B only when eB-V and eV are correct (< 0.1)
                 * because B (HIP1) overwrite B mag from ASCC catalog
                 */
                if ((!isnan(eB_V)) && (eB_V > DEF_MAG_ERROR))
                {
                    /* Set confidence to medium when eB-V is not correct (> 0.1) */
                    mB_VProperty->SetValue(mB_V, vobsCATALOG_HIP1_ID, vobsCONFIDENCE_MEDIUM, mcsTRUE);
                }
                else if (isnan(eV) || (eV <= DEF_MAG_ERROR))
                {
                    // B = V + (B-V)
                    mB = mV + mB_V;

                    // Check NaN to avoid useless computation:
                    // e_B = SQRT( (e_V)^2 + (e_B-V)^2 )
                    eB = (isnan(eV) || isnan(eB_V)) ? NAN : alxNorm(eV, eB_V);

                    logTest("Star 'HIP %s' V=%.3lf(%.3lf)  BV=%.3lf(%.3lf)  B=%.3lf(%.3lf)",
                            starId, mV, eV, mB_V, eB_V, mB, eB);

                    // set B / eB properties with HIP1 origin (conversion):
                    FAIL(star->SetPropertyValueAndError(vobsSTAR_PHOT_JHN_B, mB, eB, vobsCATALOG_HIP1_ID));
                }
            }

            // Get rVIc property:
            rV_IcProperty = star->GetProperty(rV_IcIdx);

            // test if property is set
            if (IS_TRUE(rV_IcProperty->IsSet()))
            {
                code = rV_IcProperty->GetValue();
                ch = code[0];

                /*
                 * Note on r_V-I  : the origin of the V-I colour, in summary:
                 * 'A' for an observation of V-I in Cousins' system;
                 * 'B' to 'K' when V-I derived from measurements in other bands/photoelectric systems
                 * 'L' to 'P' when V-I derived from Hipparcos and Star Mapper photometry
                 * 'Q' for long-period variables
                 * 'R' to 'T' when colours are unknown
                 */
                if ((ch >= 'A') && (ch <= 'P'))
                {
                    // Get VIc property:
                    mV_IcProperty = star->GetProperty(mV_IcIdx);

                    // test if property is set
                    if (IS_TRUE(mV_IcProperty->IsSet()))
                    {
                        FAIL(mV_IcProperty->GetValue(&mV_Ic));
                        // Use NaN to avoid using undefined error:
                        FAIL(star->GetPropertyErrorOrDefault(mV_IcProperty, &eV_Ic, NAN));

                        // I = V - (V-I)
                        mIc = mV - mV_Ic;

                        // Check NaN to avoid useless computation:
                        // e_I = SQRT( (e_V)^2 + (e_V-I)^2 )
                        eIc = (isnan(eV) || isnan(eV_Ic)) ? NAN : alxNorm(eV, eV_Ic);

                        // High confidence for [A,L:P], medium for [B:K]
                        confidenceIc = ((ch >= 'B') && (ch <= 'K')) ? vobsCONFIDENCE_MEDIUM : vobsCONFIDENCE_HIGH;

                        /* Set confidence to medium when eV-Ic is not correct (> 0.1) */
                        if (isnan(eV_Ic) || eV_Ic > DEF_MAG_ERROR)
                        {
                            /* Overwrite confidence index for (V-Ic) */
                            mV_IcProperty->SetValue(mV_Ic, vobsCATALOG_HIP1_ID, vobsCONFIDENCE_MEDIUM, mcsTRUE);

                            confidenceIc = vobsCONFIDENCE_MEDIUM;
                        }

                        logTest("Star 'HIP %s' V=%.3lf(%.3lf) VIc=%.3lf(%.3lf) Ic=%.3lf(%.3lf %s)",
                                starId, mV, eV, mV_Ic, eV_Ic, mIc, eIc,
                                vobsGetConfidenceIndex(confidenceIc));

                        // set Ic / eIc properties with HIP1 origin (conversion):
                        FAIL(star->SetPropertyValueAndError(vobsSTAR_PHOT_COUS_I, mIc, eIc, vobsCATALOG_HIP1_ID, confidenceIc));
                    }
                }
                else
                {
                    logTest("Star 'HIP %s' - unsupported r_V-I value '%s'", starId, code);
                }
            }
        }
    }

    return mcsSUCCESS;
}

/**
 * Method to process the output star list from the catalog 2MASS
 *
 * @param list a vobsSTAR_LIST as the result of the search
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT ProcessList_MASS(vobsSTAR_LIST &list)
{
    logInfo("ProcessList_MASS: list Size=%d", list.Size());

    // keep only flux whom quality is between (A and E) (vobsSTAR_CODE_QUALITY_2MASS property Qflg column)
    // ie ignore F, X or U flagged data
    static const char* fluxProperties[] = {vobsSTAR_PHOT_JHN_J, vobsSTAR_PHOT_JHN_H, vobsSTAR_PHOT_JHN_K};

    const mcsINT32 idIdx = vobsSTAR::GetPropertyIndex(vobsSTAR_ID_2MASS);
    const mcsINT32 qFlagIdx = vobsSTAR::GetPropertyIndex(vobsSTAR_CODE_QUALITY_2MASS);

    vobsSTAR_PROPERTY *qFlagProperty;
    vobsSTAR* star = NULL;
    const char *starId, *code;
    mcsUINT32 i;
    char ch;

    // For each star of the list
    for (star = list.GetNextStar(mcsTRUE); IS_NOT_NULL(star); star = list.GetNextStar(mcsFALSE))
    {
        // Get the star ID (logs)
        starId = star->GetProperty(idIdx)->GetValueOrBlank();

        // Get QFlg property:
        qFlagProperty = star->GetProperty(qFlagIdx);

        // test if property is set
        if (IS_TRUE(qFlagProperty->IsSet()))
        {
            code = qFlagProperty->GetValue();

            if (strlen(code) == 3)
            {
                for (i = 0; i < 3; i++)
                {
                    ch = code[i];

                    // check quality between (A and E)
                    if ((ch < 'A') || (ch > 'E'))
                    {
                        logTest("Star '2MASS %s' clear property %s (bad quality='%c')", starId, fluxProperties[i], ch);

                        // TODO: use confidence index instead of clearing values BUT allow overwriting of low confidence index values
                        star->ClearPropertyValue(fluxProperties[i]);
                    }
                }
            }
            else
            {
                logTest("Star '2MASS %s' - invalid Qflg value '%s'", starId, code);
            }
        }
    }

    return mcsSUCCESS;
}

/**
 * Method to process the output star list from the catalog WISE
 *
 * @param list a vobsSTAR_LIST as the result of the search
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT ProcessList_WISE(vobsSTAR_LIST &list)
{
    logInfo("ProcessList_WISE: list Size=%d", list.Size());

    // keep only flux whom quality is between (A and C) (vobsSTAR_CODE_QUALITY_WISE property Qph_wise column)
    // ie ignore U, X or Z flagged data
    static const char* fluxProperties[] = {vobsSTAR_PHOT_JHN_L, vobsSTAR_PHOT_JHN_M, vobsSTAR_PHOT_JHN_N, vobsSTAR_PHOT_FLUX_IR_25};

    const mcsINT32 idIdx = vobsSTAR::GetPropertyIndex(vobsSTAR_ID_WISE);
    const mcsINT32 qFlagIdx = vobsSTAR::GetPropertyIndex(vobsSTAR_CODE_QUALITY_WISE);

    vobsSTAR_PROPERTY *qFlagProperty;
    vobsSTAR* star = NULL;
    const char *starId, *code;
    mcsUINT32 i;
    char ch;

    // For each star of the list
    for (star = list.GetNextStar(mcsTRUE); IS_NOT_NULL(star); star = list.GetNextStar(mcsFALSE))
    {
        // Get the star ID (logs)
        starId = star->GetProperty(idIdx)->GetValueOrBlank();

        // Get Qph_wise property:
        qFlagProperty = star->GetProperty(qFlagIdx);

        // test if property is set
        if (IS_TRUE(qFlagProperty->IsSet()))
        {
            code = qFlagProperty->GetValue();

            if (strlen(code) == 4)
            {
                for (i = 0; i < 4; i++)
                {
                    ch = code[i];

                    // check quality between (A and C)
                    if ((ch < 'A') || (ch > 'C'))
                    {
                        logTest("Star 'WISE %s' clear property %s (bad quality='%c')", starId, fluxProperties[i], ch);

                        // TODO: use confidence index instead of clearing values BUT allow overwriting of low confidence index values
                        star->ClearPropertyValue(fluxProperties[i]);
                    }
                }
            }
            else
            {
                logTest("Star 'WISE %s' - invalid Qph_wise value '%s'", starId, code);
            }
        }
    }

    return mcsSUCCESS;
}

/**
 * Method to process the output star list from the catalog WISE
 *
 * @param list a vobsSTAR_LIST as the result of the search
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT ProcessList_MDFC(vobsSTAR_LIST &list)
{
    logInfo("ProcessList_MDFC: list Size=%d", list.Size());

    // convert MDFC Flux MAD to std error:
    static const mcsINT32 fluxPropertyIds[] = {
        vobsSTAR::GetPropertyIndex(vobsSTAR_PHOT_FLUX_L_MED), 
        vobsSTAR::GetPropertyIndex(vobsSTAR_PHOT_FLUX_M_MED), 
        vobsSTAR::GetPropertyIndex(vobsSTAR_PHOT_FLUX_N_MED)
    };

    vobsSTAR* star = NULL;
    mcsSTRING64 starId;
    mcsUINT32 i;
    vobsSTAR_PROPERTY *fluxProperty;
    mcsDOUBLE flux, err;

    // For each star of the list
    for (star = list.GetNextStar(mcsTRUE); IS_NOT_NULL(star); star = list.GetNextStar(mcsFALSE))
    {
        // Get Star ID
        FAIL(star->GetId(starId, sizeof (starId)));

        for (i = 0; i < 3; i++)
        {
            // Get Qph_wise property:
            fluxProperty = star->GetProperty(fluxPropertyIds[i]);

            // test if property is set
            if (IS_NOT_NULL(fluxProperty) && IS_TRUE(fluxProperty->IsErrorSet()))
            {
                FAIL(fluxProperty->GetValue(&flux));
                FAIL(fluxProperty->GetError(&err));
                
                // Convert dispersion (mad) to standard error:
                err *= 1.4826;

                logTest("Star '%s' - MDFC '%s' flux: %.4lf %.5lf (Jy)", starId, 
                        ((i == 0) ? "L" : ((i == 1) ? "M" : "N")),
                        flux, err);
                
                fluxProperty->SetError(err * 1.4826, mcsTRUE);
            }
        }
    }

    return mcsSUCCESS;
}

/*___oOo___*/
