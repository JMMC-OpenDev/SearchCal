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

/* list size threshold to use chunks */
#define vobsTHRESHOLD_SIZE 2048

/* size of chunks */
#define vobsMAX_QUERY_SIZE 512

#define TARGET_ID_LENGTH 21

/*
 * Local Variables
 */
/** vizier URI environment variable */
static const mcsENVNAME vobsVizierUriEnvVarName = "VOBS_VIZIER_URI";
/** vizier URI CGI suffix */
static const char* vobsVizierUriSuffix = "/viz-bin/asu-xml?";
/** vizier URI in use */
static char* vizierURI = NULL;

/** Free the vizier URI */
void vobsFreeVizierURI()
{
    if (vizierURI != NULL)
    {
        free(vizierURI);
        vizierURI = NULL;
    }
}

/**
 * Get the vizier URI in use
 */
char* vobsGetVizierURI()
{
    if (vizierURI != NULL)
    {
        return vizierURI;
    }
    // compute it once:

    mcsSTRING1024 uri;

    const char* uriVizier = "http://vizier.u-strasbg.fr"; // For production purpose
    //    const char* uriVizier =  "http://viz-beta.u-strasbg.fr"; // For beta testing

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

    vizierURI = miscDuplicateString(uri);

    logQuiet("Catalogs will get VIZIER data from '%s'", vizierURI);

    return vizierURI;
}

/*
 * Class constructor
 * @param name catalog identifier / name
 */
vobsREMOTE_CATALOG::vobsREMOTE_CATALOG(const char *name,
                                       const mcsDOUBLE precision,
                                       const mcsDOUBLE epochFrom,
                                       const mcsDOUBLE epochTo,
                                       const mcsLOGICAL hasProperMotion,
                                       const mcsLOGICAL multipleRows,
                                       const vobsSTAR_PROPERTY_MASK* overwritePropertyMask)
: vobsCATALOG(name, precision, epochFrom, epochTo, hasProperMotion, multipleRows, overwritePropertyMask)
{
    // Initialise dynamic buffer corresponding to query
    miscDynBufInit(&_query);

    /* Allocate some memory to store the complete query (4K) */
    miscDynBufAlloc(&_query, 4096);

    // define targetId index to NULL: 
    _targetIdIndex = NULL;

    // defaults to true:
    _alwaysSort = mcsTRUE;
}

/*
 * Class destructor
 */
vobsREMOTE_CATALOG::~vobsREMOTE_CATALOG()
{
    // free targetId index:
    if (_targetIdIndex != NULL)
    {
        ClearTargetIdIndex();

        delete(_targetIdIndex);
    }
    // Destroy dynamic buffer corresponding to query
    miscDynBufDestroy(&_query);
}


/*
 * Public methods
 */

/**
 * Search in the catalog a list of star.
 *
 * Search int the catalog a list of star according to a vobsREQUEST
 *
 * @param request a vobsREQUEST which have all the contraints for the search
 * @param list a vobsSTAR_LIST as the result of the search
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 * 
 * \sa vobsREQUEST
 *
 * \b Errors codes:\n 
 * The possible errors are:
 */
mcsCOMPL_STAT vobsREMOTE_CATALOG::Search(vobsREQUEST &request,
                                         vobsSTAR_LIST &list,
                                         vobsCATALOG_STAR_PROPERTY_CATALOG_MAPPING* propertyCatalogMap,
                                         mcsLOGICAL logResult)
{
    mcsUINT32 listSize = list.Size();

    // Prepare file name to log result of the catalog request
    mcsSTRING512 logFileName;
    // if the log level is higher or equal to the debug level
    if ((logResult == mcsTRUE) || doLog(logDEBUG))
    {
        // Get band used for search
        const char* band = request.GetSearchBand();

        // build the first part of the file name in the MCSDATA directory
        sprintf(logFileName, "$MCSDATA/tmp/list_%s", band);

        // Get catalog name, and replace '/' by '_'
        mcsSTRING32 catalog;
        strcpy(catalog, GetName());

        miscReplaceChrByChr(catalog, '/', '_');
        strcat(logFileName, "_");
        strcat(logFileName, catalog);

        // the list is mpty the data which will be write in the file will come
        // from a "primary" asking
        strcat(logFileName, (listSize == 0) ? "_1.log" : "_2.log");

        // Resolve path
        char *resolvedPath;
        resolvedPath = miscResolvePath(logFileName);
        if (resolvedPath != NULL)
        {
            strcpy(logFileName, resolvedPath);
            free(resolvedPath);
        }
    }
    else
    {
        memset((char *) logFileName, '\0', sizeof (logFileName));
    }

    // Check if the list is empty
    // if ok, the asking is writing according to only the request
    if (listSize == 0)
    {
        FAIL(PrepareQuery(request));

        // The parser get the query result through Internet, and analyse it
        vobsPARSER parser;
        FAIL(parser.Parse(vobsGetVizierURI(), miscDynBufGetBuffer(&_query), GetName(), list, propertyCatalogMap, logFileName));
    }
    else
    {
        // else, the asking is writing according to the request and the star list
        if (listSize < vobsTHRESHOLD_SIZE)
        {
            FAIL(PrepareQuery(request, list));

            // The parser get the query result through Internet, and analyse it
            vobsPARSER parser;
            FAIL(parser.Parse(vobsGetVizierURI(), miscDynBufGetBuffer(&_query), GetName(), list, propertyCatalogMap, logFileName));

            // Perform post processing on catalog results (targetId mapping ...):
            if (list.Size() > 0)
            {
                FAIL(ProcessList(list));
            }
        }
        else
        {
            logTest("Search: list Size = %d, cutting in chunks of %d", listSize, vobsMAX_QUERY_SIZE);

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

            int count = 0, total = 0, i = 0;

            vobsSTAR* currentStar = shadow.GetNextStar(mcsTRUE);

            while (currentStar != NULL)
            {
                subset.AddRefAtTail(currentStar);

                count++;
                total++;

                if (count > vobsMAX_QUERY_SIZE)
                {
                    // define the free pointer flag to avoid double frees (shadow and subset are storing same star pointers):
                    subset.SetFreeStarPointers(false);

                    i++;

                    logTest("Search: Iteration %d = %d", i, total);

                    FAIL(PrepareQuery(request, subset));

                    // The parser get the query result through Internet, and analyse it
                    vobsPARSER parser;
                    FAIL(parser.Parse(vobsGetVizierURI(), miscDynBufGetBuffer(&_query), GetName(), subset, propertyCatalogMap, logFileName));

                    // Perform post processing on catalog results (targetId mapping ...):
                    if (subset.Size() > 0)
                    {
                        FAIL(ProcessList(subset));
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

                FAIL(PrepareQuery(request, subset));

                // The parser get the query result through Internet, and analyse it
                vobsPARSER parser;
                FAIL(parser.Parse(vobsGetVizierURI(), miscDynBufGetBuffer(&_query), GetName(), subset, propertyCatalogMap, logFileName));

                // Perform post processing on catalog results (targetId mapping ...):
                if (subset.Size() > 0)
                {
                    FAIL(ProcessList(subset));
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
 * Protected methods
 */

/**
 * Method to process optionally the output star list from the catalog
 * 
 * @param list a vobsSTAR_LIST as the result of the search
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsREMOTE_CATALOG::ProcessList(vobsSTAR_LIST &list)
{
    const mcsUINT32 listSize = list.Size();

    if (listSize > 0)
    {
        logDebug("ProcessList: list Size = %d", listSize);

        // fix target Id column by using a map<string, string> to fix epoch to J2000

        if ((_targetIdIndex != NULL) && (_targetIdIndex->size() > 0))
        {
            // For each star of the given star list
            vobsSTAR* star = NULL;
            vobsSTAR_PROPERTY* targetIdProperty;
            const char *targetIdJ2000, *targetId;
            vobsTARGET_ID_MAPPING::iterator it;

            // For each star of the list
            for (star = list.GetNextStar(mcsTRUE); star != NULL; star = list.GetNextStar(mcsFALSE))
            {
                targetIdProperty = star->GetTargetIdProperty();

                // test if property is set
                if (targetIdProperty->IsSet() == mcsTRUE)
                {
                    targetId = targetIdProperty->GetValue();

                    if (doLog(logDEBUG))
                    {
                        logDebug("targetId      '%s'", targetId);
                    }

                    it = _targetIdIndex->find(targetId);

                    if (it == _targetIdIndex->end())
                    {
                        logInfo("targetId not found: '%s'", targetId);
                    }
                    else
                    {
                        targetIdJ2000 = it->second;

                        if (doLog(logDEBUG))
                        {
                            logDebug("targetIdJ2000 '%s'", targetIdJ2000);
                        }

                        FAIL(targetIdProperty->SetValue(targetIdJ2000, targetIdProperty->GetOrigin(), targetIdProperty->GetConfidenceIndex(), mcsTRUE));
                    }
                }
            }

            // clear targetId index:
            ClearTargetIdIndex();
        }
    }

    return mcsSUCCESS;
}

/**
 * Prepare the asking.
 *
 * Prepare the asking according to the request (constraints) for a first ask
 * to the CDS, that's mean that the use of this asking will help to have a
 * list of possible star
 *
 * @param request vobsREQUEST which have all the contraints for the search
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsREMOTE_CATALOG::PrepareQuery(vobsREQUEST &request)
{
    // Reset the dynamic buffer which contain the query    
    miscDynBufReset(&_query);

    // in this case of request, there are three parts to write :
    // the location
    // the position of the reference star
    // the specific part of the query
    FAIL(WriteQueryURIPart());

    FAIL(WriteReferenceStarPosition(request));

    FAIL(WriteQuerySpecificPart(request));

    FAIL(WriteOption());

    return mcsSUCCESS;
}

/**
 * Prepare the asking.
 *
 * Prepare the asking according to the request (constraints). The knowledge of
 * another list of star help to create the asking for a final ask to the CDS.
 *
 * @param request vobsREQUEST which have all the contraints for the search  
 * @param tmpList vobsSTAR_LIST which come from an older ask to the CDS. 
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsREMOTE_CATALOG::PrepareQuery(vobsREQUEST &request,
                                               vobsSTAR_LIST &tmpList)
{
    miscDynBufReset(&_query);

    // in this case of request, there are four parts to write :
    // the location
    // the constant part of the query
    // the specific part of the query
    // the list to complete
    FAIL(WriteQueryURIPart());

    FAIL(WriteQueryConstantPart(request, tmpList));

    FAIL(WriteQuerySpecificPart());

    FAIL(WriteOption());

    FAIL(WriteQueryStarListPart(tmpList));

    return mcsSUCCESS;
}

/**
 * Build the destination part of the query.
 *
 * Build the destination part of the query. All catalog files are located on
 * web server. It is possible to find them on the URL : 
 * http://vizier.u-strasbg.fr/viz-bin/asu-xml?-source= ...
 * * &-out.meta=hudU1&-oc.form=sexa has been added o get previous UCD1 instead
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
 * Adds common part = MAX=1000 and compute _RAJ2000 and _DEJ2000 (HMS)
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsREMOTE_CATALOG::WriteQueryURIPart(void)
{
    miscDynBufAppendString(&_query, "-source=");
    miscDynBufAppendString(&_query, GetName());
    miscDynBufAppendString(&_query, "&-out.meta=hudU1&-oc.form=sexa");

    // add common part: MAX=1000 and compute _RAJ2000 and _DEJ2000 (HMS)
    miscDynBufAppendString(&_query, "&-c.eq=J2000");

    // Get the computed right ascension (J2000 / epoch 2000 in HMS) _RAJ2000 (POS_EQ_RA_MAIN) stored in the 'vobsSTAR_POS_EQ_RA_MAIN' property
    miscDynBufAppendString(&_query, "&-out.add=_RAJ2000");
    // Get the computed declination (J2000 / epoch 2000 in DMS)     _DEJ2000 (POS_EQ_DEC_MAIN) stored in the 'vobsSTAR_POS_EQ_DEC_MAIN' property
    miscDynBufAppendString(&_query, "&-out.add=_DEJ2000");

    miscDynBufAppendString(&_query, "&-oc=hms");
    miscDynBufAppendString(&_query, "&-out.max=1000");

    if (_alwaysSort == mcsTRUE)
    {
        // order results by distance
        miscDynBufAppendString(&_query, "&-sort=_r");
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
mcsCOMPL_STAT vobsREMOTE_CATALOG::WriteQueryConstantPart(vobsREQUEST &request, vobsSTAR_LIST &tmpList)
{
    bool useBox = false;
    mcsSTRING16 separation;

    // Get cone search radius:
    mcsDOUBLE radius = request.GetConeSearchRadius();
    if (radius > 0.0)
    {
        logTest("Search: input list [%s] catalog id: '%s'", tmpList.GetName(), tmpList.GetCatalogId());
        logTest("Search: catalog id: '%s'", GetCatalogMeta()->GetName());

        if (GetCatalogMeta()->IsSingleEpoch())
        {
            if ((tmpList.GetCatalogMeta() != NULL) && (tmpList.GetCatalogMeta()->DoPrecessEpoch() == mcsTRUE))
            {
                // Need to expand radius to get enough candidates (2MASS):

                mcsDOUBLE avgRadius = 0.0;

                FAIL(GetAverageEpochSearchRadius(tmpList, avgRadius))

                radius += avgRadius + 0.05;
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
            useBox = true;

            // Adapt search area according to star's proper motion (epoch):
            mcsDOUBLE deltaRa = 0.0;
            mcsDOUBLE deltaDec = 0.0;

            FAIL(GetEpochSearchArea(tmpList, deltaRa, deltaDec))

            // length = maxMove + 2 x radius (margin):
            deltaRa += 2.0 * radius + 0.05; // for rounding purposes
            deltaDec += 2.0 * radius + 0.05; // for rounding purposes

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
        logTest("Search: Box search area = %s arcsec", separation);

        miscDynBufAppendString(&_query, "&-c.geom=b&-c.bs="); // -c.bs means box in arcsec
    }
    else
    {
        logTest("Search: Cone search area = %s arcsec", separation);

        miscDynBufAppendString(&_query, "&-c.rs="); // -c.rs means radius in arcsec
    }
    miscDynBufAppendString(&_query, separation);

    // Get the given star coordinates (RA+DEC) _1 (ID_TARGET) stored in the 'vobsSTAR_ID_TARGET' property
    // for example: '016.417537-41.369444'
    miscDynBufAppendString(&_query, "&-out.add=_1");

    miscDynBufAppendString(&_query, "&-file=-c");

    return mcsSUCCESS;
}

/**
 * Build the specific part of the asking.
 *
 * Build the specific part of the asking. This is the part of the asking
 * which is write specificaly for each catalog. The constraints of the request
 * which help to build an asking in order to restrict the research.
 *
 * @param request vobsREQUEST which help to restrict the search
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsREMOTE_CATALOG::WriteQuerySpecificPart(vobsREQUEST &request)
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

        deltaRa += 0.5; // for rounding purposes
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
        logTest("Search: Box search area = %s arcmin", separation);

        miscDynBufAppendString(&_query, "&-c.geom=b&-c.bm="); // -c.bm means box in arcmin
    }
    else
    {
        logTest("Search: Cone search area = %s arcmin", separation);

        miscDynBufAppendString(&_query, "&-c.rm="); // -c.rm means radius in arcmin
    }
    miscDynBufAppendString(&_query, separation);

    // Add the magnitude range constraint on the requested band:
    const char* band = request.GetSearchBand();

    mcsSTRING32 rangeMag;
    sprintf(rangeMag, "%.2lf..%.2lf", request.GetMinMagRange(), request.GetMaxMagRange());

    logTest("Search: Magnitude %s range = %s", band, rangeMag);

    FAIL(WriteQueryBandPart(band, rangeMag));

    // properties to retrieve
    return WriteQuerySpecificPart();
}

/**
 * Build the band constraint part of the asking.
 * 
 * @param band requested band
 * @param rangeMag magnitude range constraint ("%.2lf..%.2lf")
 * 
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsREMOTE_CATALOG::WriteQueryBandPart(const char* band, mcsSTRING32 &rangeMag)
{
    // Add the magnitude range constraint on the requested band:
    miscDynBufAppendString(&_query, "&");
    miscDynBufAppendString(&_query, band);
    miscDynBufAppendString(&_query, "mag=");
    miscDynBufAppendString(&_query, rangeMag);

    return mcsSUCCESS;
}

/**
 * Build the specific part of the asking.
 *
 * Build the specific part of the asking. This is the part of the asking
 * which is write specificaly for each catalog.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsREMOTE_CATALOG::WriteQuerySpecificPart(void)
{
    logWarning("vobsREMOTE_CATALOG::WriteQuerySpecificPart used instead of sub class implementation !");

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
mcsCOMPL_STAT vobsREMOTE_CATALOG::WriteReferenceStarPosition(vobsREQUEST &request)
{
    mcsDOUBLE ra, dec;
    mcsDOUBLE pmRa, pmDec;
    mcsSTRING16 raDeg, decDeg;

    ra = request.GetObjectRaInDeg();
    dec = request.GetObjectDecInDeg();

    if (GetCatalogMeta()->IsEpoch2000() == mcsFALSE)
    {
        // proper motion (mas/yr):
        // TODO: let sclgui provide pmRA / pmDec using star resolver (simbad) info:
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
    miscDynBufAppendString(&_query, "&-c=");
    miscDynBufAppendString(&_query, raDeg);

    if (decDeg[0] == '+')
    {
        miscDynBufAppendString(&_query, "%2b");
        miscDynBufAppendString(&_query, &decDeg[1]);
    }
    else
    {
        miscDynBufAppendString(&_query, decDeg);
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
mcsCOMPL_STAT vobsREMOTE_CATALOG::WriteQueryStarListPart(vobsSTAR_LIST &list)
{
    // Build of the stringlist
    miscDYN_BUF strList;
    miscDynBufInit(&strList);

    // write a star list object as a dynamic buffer in order to write it in a
    // string format in the query
    if (StarList2String(strList, list) == mcsFAILURE)
    {
        logError("An Error occured when converting the input star list to string (RA/DEC coordinates) !");

        miscDynBufDestroy(&strList);
        return mcsFAILURE;
    }

    miscDynBufAppendString(&_query, "&-out.form=List");
    miscDynBufAppendString(&_query, miscDynBufGetBuffer(&strList));

    miscDynBufDestroy(&strList);

    return mcsSUCCESS;
}

/**
 * Write option
 *
 * @return always mcsSUCCESS
 */
mcsCOMPL_STAT vobsREMOTE_CATALOG::WriteOption()
{
    miscDynBufAppendString(&_query, GetOption());

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
mcsCOMPL_STAT vobsREMOTE_CATALOG::StarList2String(miscDYN_BUF &strList,
                                                  const vobsSTAR_LIST &list)
{
    const unsigned int nbStars = list.Size();

    // if the list is not empty
    if (nbStars != 0)
    {
        /* buffer capacity = fixed (50) + dynamic (nbStars x 24) */
        const int capacity = 50 + 24 * nbStars;

        miscDynBufAlloc(&strList, capacity);

        // Start the List argument -c=<<====LIST&
        miscDynBufAppendString(&strList, "&-c=%3C%3C%3D%3D%3D%3DLIST&");

        mcsDOUBLE ra, dec;
        mcsSTRING16 raDeg, decDeg;

        const bool doPrecess = !GetCatalogMeta()->IsEpoch2000();
        const mcsDOUBLE epochMed = GetCatalogMeta()->GetEpochMedian();

        if (doPrecess)
        {
            // Prepare the targetId index:
            if (_targetIdIndex == NULL)
            {
                // create the targetId index allocated until destructor is called:
                _targetIdIndex = new vobsTARGET_ID_MAPPING();
            }
            else
            {
                ClearTargetIdIndex();
            }
        }

        // line buffer to avoid too many calls to dynamic buf:
        // Note: 48 bytes is large enough to contain one line
        // No buffer overflow checks !

        char *targetIdFrom, *targetTo;
        mcsSTRING48 value;
        char* valPtr;
        vobsSTAR* star;

        for (unsigned int el = 0; el < nbStars; el++)
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

            FAIL(star->GetRa(ra));
            FAIL(star->GetDec(dec));

            vobsSTAR::raToDeg(ra, raDeg);
            vobsSTAR::decToDeg(dec, decDeg);

            if (doPrecess)
            {
                targetIdFrom = new char[TARGET_ID_LENGTH];
                strcpy(targetIdFrom, raDeg);
                strcat(targetIdFrom, decDeg);

                // ra/dec coordinates are corrected to the catalog's epoch:
                FAIL(star->PrecessRaDecToEpoch(epochMed, ra, dec));

                vobsSTAR::raToDeg(ra, raDeg);
                vobsSTAR::decToDeg(dec, decDeg);

                targetTo = new char[TARGET_ID_LENGTH];
                strcpy(targetTo, raDeg);
                strcat(targetTo, decDeg);

                if (doLog(logDEBUG))
                {
                    logDebug("targetId      '%s'", targetTo);
                    logDebug("targetIdJ2000 '%s'", targetIdFrom);
                }

                _targetIdIndex->insert(vobsTARGET_ID_PAIR(targetTo, targetIdFrom));
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

            miscDynBufAppendString(&strList, value);
        }

        // Close the List argument &====LIST
        miscDynBufAppendString(&strList, "&%3D%3D%3D%3DLIST");
    }

    return mcsSUCCESS;
}

mcsCOMPL_STAT vobsREMOTE_CATALOG::GetEpochSearchArea(const vobsSTAR_LIST &list, mcsDOUBLE &deltaRA, mcsDOUBLE &deltaDEC)
{
    const unsigned int nbStars = list.Size();

    mcsDOUBLE deltaRa = 0.0;
    mcsDOUBLE deltaDec = 0.0;

    // if the list is not empty
    if (nbStars != 0)
    {
        mcsDOUBLE ra, dec;
        mcsDOUBLE pmRa, pmDec; // max/yr

        vobsSTAR* star;

        const mcsDOUBLE deltaEpoch = GetCatalogMeta()->GetEpochDelta();

        for (unsigned int el = 0; el < nbStars; el++)
        {
            // Get next star
            star = list.GetNextStar((mcsLOGICAL) (el == 0));

            if (star != NULL)
            {
                FAIL(star->GetRa(ra));
                FAIL(star->GetDec(dec));
                FAIL(star->GetPmRa(pmRa));
                FAIL(star->GetPmDec(pmDec));

                deltaRa = max(deltaRa, fabs(vobsSTAR::GetDeltaRA(pmRa, deltaEpoch)));
                deltaDec = max(deltaDec, fabs(vobsSTAR::GetDeltaDEC(pmDec, deltaEpoch)));
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
    // TODO: should deal with pmRA arround poles ? ie take into account the declination of each star ?

    // Typical case: ASCC or USNO query (1991.25) with 2MASS stars (1997 - 2001) as input (FAINT)

    const static mcsDOUBLE maxProperMotion = 2.0; // 2 arcsec/yr

    const unsigned int nbStars = list.Size();

    mcsDOUBLE deltaEpoch = 0.0;

    // if the list is not empty
    if (nbStars != 0)
    {
        mcsDOUBLE jdDate, epoch;
        vobsSTAR* star;

        const mcsDOUBLE epochMed = GetCatalogMeta()->GetEpochMedian();

        for (unsigned int el = 0; el < nbStars; el++)
        {
            // Get next star
            star = list.GetNextStar((mcsLOGICAL) (el == 0));

            if (star != NULL)
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
 * Clear the targetId index ie free allocated char arrays for key / value pairs
 */
void vobsREMOTE_CATALOG::ClearTargetIdIndex()
{
    // free targetId index:
    if (_targetIdIndex != NULL)
    {
        for (vobsTARGET_ID_MAPPING::iterator iter = _targetIdIndex->begin(); iter != _targetIdIndex->end(); iter++)
        {
            delete[](iter->first);
            delete[](iter->second);
        }
        _targetIdIndex->clear();
    }
}

/*___oOo___*/
