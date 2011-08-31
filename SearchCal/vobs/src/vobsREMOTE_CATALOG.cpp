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
/**
 * Export standard iostream objects (cin, cout,...).
 */

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

/*
 * Local Variables
 */
/** vizier URI environment variable */
static const mcsENVNAME vobsVizierUriEnvVarName = "VOBS_VIZIER_URI";
/** vizier URI CGI suffix */
static const char* vobsVizierUriSuffix = "/viz-bin/asu-xml?";
/** vizier URI in use */
static char* vizierURI = NULL;

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
    if (miscGetEnvVarValue2(vobsVizierUriEnvVarName, envVizierUri, sizeof(envVizierUri), mcsTRUE) == mcsSUCCESS)
    {
        // Check the env. var. is not empty
        if (strlen(envVizierUri) != 0)
        {
            logDebug("Found '%s' environment variable content for VIZIER URI.", vobsVizierUriEnvVarName);

            strncpy(uri, envVizierUri, sizeof(envVizierUri) - 1);
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
    strncat(uri, vobsVizierUriSuffix, sizeof(envVizierUri) - 1);
    
    vizierURI = miscDuplicateString(uri);

    logQuiet("Catalogs will get VIZIER data from '%s'", vizierURI);
    
    return vizierURI;
}
/*
 * Class constructor
 */

/**
 * Build a catalog object.
 */
vobsREMOTE_CATALOG::vobsREMOTE_CATALOG(const char *name) : vobsCATALOG(name)
{
    // Initialise dynamic buffer corresponding to query
    miscDynBufInit(&_query);
    
    /* Allocate some memory to store the complete query (4K) */
    miscDynBufAlloc(&_query, 4096);
}


/*
 * Class destructor
 */

/**
 * Delete a catalog object. 
 */
vobsREMOTE_CATALOG::~vobsREMOTE_CATALOG()
{
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
mcsCOMPL_STAT vobsREMOTE_CATALOG::Search(vobsREQUEST &request, vobsSTAR_LIST &list)
{
    logTrace("vobsREMOTE_CATALOG::Search()");
    
    mcsUINT32 listSize = list.Size();
    
    // Prepare file name to log result of the catalog request
    mcsSTRING512 logFileName;
    // if the log level is higher or equal to the debug level
    if (logIsStdoutLogLevel(logDEBUG) == mcsTRUE)
    {
        // Get band used for search
        const char *band;
        band = request.GetSearchBand();

        // build the first part of the file name in the MCSDATA directory
        sprintf(logFileName, "$MCSDATA/tmp/list_%s", band);

        // Get catalog name, and replace '/' by '_'
        mcsSTRING32 catalogName;
        strcpy(catalogName, GetName());
        miscReplaceChrByChr(catalogName, '/', '_');
        strcat(logFileName, "_");
        strcat(logFileName, catalogName);
        
        // the list is mpty the data which will be write in the file will come
        // from a "primary" asking
        if (listSize == 0)
        {
            strcat(logFileName, "_1.log");
        }
        else
        {
            strcat(logFileName, "_2.log");
        }
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
       memset((char *)logFileName , '\0', sizeof(logFileName)); 
    }

    // Check if the list is empty
    // if ok, the asking is writing according to only the request
    if (listSize == 0)
    {
        if (PrepareQuery(request) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
        
        // The parser get the query result through Internet, and analyse it
        vobsPARSER parser;
        if (parser.Parse(vobsGetVizierURI(), miscDynBufGetBuffer(&_query), GetName(), list, logFileName) == mcsFAILURE)
        {
            return mcsFAILURE; 
        }
    }
    // else, the asking is writing according to the request and the star list
    else 
    {
       if (listSize < vobsTHRESHOLD_SIZE)
       {
           if (PrepareQuery(request, list) == mcsFAILURE)
           { 
               return mcsFAILURE; 
           }
           
           // The parser get the query result through Internet, and analyse it
           vobsPARSER parser;
           if (parser.Parse(vobsGetVizierURI(), miscDynBufGetBuffer(&_query), GetName(), list, logFileName) == mcsFAILURE)
           {
               return mcsFAILURE; 
           }
       }
       else
       {
            // shadow is a local copy of the input list:
            vobsSTAR_LIST shadow;
            
            // just move stars into given list:
            shadow.CopyRefs(list);
            
            // purge given list to be able to add stars using CopyRefs(subset):
            list.Clear();

            // subset contains only star pointers (no copy):
            vobsSTAR_LIST subset;
            // define the free pointer flag to avoid double frees (shadow and subset are storing same star pointers):
            subset.SetFreeStarPointers(false);
            
            /*
             * Note: vobsPARSER::parse calls subset.Clear() that restore the free pointer flag to avoid memory leaks
             */

            logTest("Search: list Size = %d, cutting in chunks of %d", shadow.Size(), vobsMAX_QUERY_SIZE);
            
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
                    
                    if (PrepareQuery(request, subset) == mcsFAILURE)
		    { 
                        return mcsFAILURE; 
		    }
                    // The parser get the query result through Internet, and analyse it
                    vobsPARSER parser;
                    if (parser.Parse(vobsGetVizierURI(), miscDynBufGetBuffer(&_query), GetName(), subset, logFileName) == mcsFAILURE)
		    {
  		        return mcsFAILURE; 
		    }
                    
                    // move stars into list:
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

                if (PrepareQuery(request, subset) == mcsFAILURE)
		{ 
                    return mcsFAILURE; 
		}
                // The parser get the query result through Internet, and analyse it
                vobsPARSER parser;
                if (parser.Parse(vobsGetVizierURI(), miscDynBufGetBuffer(&_query), GetName(), subset, logFileName) == mcsFAILURE)
		{
                    return mcsFAILURE; 
		}
                    
                // move stars into list:
                list.CopyRefs(subset);

                // clear subset:
                subset.Clear();
            }
       }
    }

    return mcsSUCCESS;
}

/**
 * Set catalog option
 *
 * @param option the option to add in the query
 *
 * @return always mcsSUCCESS
 */
mcsCOMPL_STAT vobsREMOTE_CATALOG::SetOption(string option)
{
    logTrace("vobsREMOTE_CATALOG::SetOption()");

    _option = option;

    return mcsSUCCESS;
}
/*
 * Protected methods
 */

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
    logTrace("vobsREMOTE_CATALOG::PrepareQuery()");
    
    // Reset the dynamic buffer which contain the query    
    miscDynBufReset(&_query);

    // in this case of request, there are three parts to write :
    // the location
    // the position of the reference star
    // the specific part of the query
    if (WriteQueryURIPart()==mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (WriteReferenceStarPosition(request) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (WriteQuerySpecificPart(request) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (WriteOption() == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
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
    logTrace("vobsREMOTE_CATALOG::PrepareQuery()");
    
    miscDynBufReset(&_query);
    
    // in this case of request, there are four parts to write :
    // the location
    // the constant part of the query
    // the specific part of the query
    // the list to complete
    if (WriteQueryURIPart() == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (WriteQueryConstantPart() == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (WriteQuerySpecificPart() == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (WriteOption() == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (WriteQueryStarListPart(tmpList) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

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
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsREMOTE_CATALOG::WriteQueryURIPart(void)
{
    logTrace("vobsREMOTE_CATALOG::WriteQueryURIPart()");

    if (miscDynBufAppendString(&_query, "-source=") == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (miscDynBufAppendString(&_query, GetName()) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (miscDynBufAppendString(&_query, "&-out.meta=hudU1&-oc.form=sexa") == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    return mcsSUCCESS;
}

/**
 * Build the constant part of the asking
 *
 * Build the constant part of the asking. For each catalog, a part of the
 * asking is the same.
 *
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsREMOTE_CATALOG::WriteQueryConstantPart(void)
{
    logTrace("vobsREMOTE_CATALOG::WriteQueryConstantPart()");

    if (miscDynBufAppendString(&_query, "&-file=-c&-c.eq=J2000&-c.r=5&-c.u=arcsec") == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    if (miscDynBufAppendString(&_query, "&-out.max=1000") == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    if (miscDynBufAppendString(&_query, "&-out.add=_RAJ2000,_DEJ2000&-oc=hms") == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    return mcsSUCCESS;
}

/**
 * Build the specificatic part of the asking.
 *
 * Build the specificatic part of the asking. This is the part of the asking
 * which is write specificaly for each catalog.
 *
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 * 
 *
 */
mcsCOMPL_STAT vobsREMOTE_CATALOG::WriteQuerySpecificPart(void)
{
    logTrace("vobsREMOTE_CATALOG::WriteQuerySpecificPart()");

    
    return mcsSUCCESS;
}

/**
 * Build the specificatic part of the asking.
 *
 * Build the specificatic part of the asking. This is the part of the asking
 * which is write specificaly for each catalog. The constraints of the request
 * which help to build an asking in order to restrict the research.
 *
 * @param request vobsREQUEST which help to restrict the search
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 * 
 * \b Errors codes:\n
 * The possible errors are:
 *
 */
mcsCOMPL_STAT vobsREMOTE_CATALOG::WriteQuerySpecificPart(vobsREQUEST &request)
{
    logTrace("vobsREMOTE_CATALOG::WriteQuerySpecificPart()");

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
    logTrace("vobsREMOTE_CATALOG::WriteReferenceStarPosition()");

    const char *ra;
    ra = request.GetObjectRa();
    const char *dec;
    dec = request.GetObjectDec();
    
    if (miscDynBufAppendString(&_query, "&-c.ra=") == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (miscDynBufAppendString(&_query, ra) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (miscDynBufAppendString(&_query, "&-c.dec=") == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (miscDynBufAppendString(&_query, dec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    return mcsSUCCESS;
}

/**
 * Buil the end part of the asking.
 *
 * The end part of the asking for a search from a star list.
 *
 * @param list vobsSTAR_LIST which help to build the end part
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsREMOTE_CATALOG::WriteQueryStarListPart(vobsSTAR_LIST &list)
{
    logTrace("vobsREMOTE_CATALOG::WriteQueryStarListPart()");
    // Build of the stringlist
    miscDYN_BUF strList;
    miscDynBufInit(&strList);

    // write a star list object as a dynamic buffer in order to write it in a
    // string format in the query
    StarList2String(strList, list);
    
    if ( (miscDynBufAppendString(&_query,"&-out.form=List") == mcsFAILURE) ||
         (miscDynBufAppendString(&_query, miscDynBufGetBuffer(&strList)) == mcsFAILURE))
    {
        miscDynBufDestroy(&strList);
        return mcsFAILURE;
    }

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
    logTrace("vobsREMOTE_CATALOG::WriteOption()");

    miscDynBufAppendString(&_query, _option.c_str());            

    return mcsSUCCESS;
}

/**
 * Convert a star list to a string list.
 *
 * The research of specific star knowong their coordonate need to write in the
 * asking the list of coordonate as a string. This method convert the position
 * of all star present in a star list in a string.
 *
 * @param strList string list as a string
 * @param list star list to cnvert
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 * 
 * \b Errors codes:\n
 * The possible errors are:
 *
 */
mcsCOMPL_STAT vobsREMOTE_CATALOG::StarList2String(miscDYN_BUF &strList,
                                                  vobsSTAR_LIST &list)
{
    logTrace("vobsREMOTE_CATALOG::StarList2String()");

    const unsigned int nbStars = list.Size();
    
    // if the list is not empty
    if (nbStars != 0)
    {
        /* buffer capacity = fixed (75) + dynamic (nbStars x 30) */
        const int capacity = 75 + 30 * nbStars;

        miscDynBufAlloc(&strList, capacity);
        
        miscDynBufAppendString(&strList, "&-c=%3C%3C%3D%3D%3D%3Dresult1%5F280%2Etxt&");
        
        mcsSTRING32 ra;
        mcsSTRING12 hra, mra, sra;
        mcsSTRING32 dec;
        mcsSTRING12 ddec, mdec, sdec;
        
        // line buffer to avoid too many calls to dynamic buf:
        // Note: 48 bytes is large enough to contain one line
        // No buffer overflow checks !
        
        mcsSTRING48 value;
        char*       valPtr;
        
        for (unsigned int el = 0; el < nbStars; el++)
        {            
            // reset value pointer:
            valPtr = value;
        
            if (el == 0)
            {
                value[0] = '\0';
            } 
            else 
            {
                strcpy(value, "&+");
            }
            
            vobsSTAR *star = list.GetNextStar((mcsLOGICAL)(el==0));

            strcpy(ra, star->GetPropertyValue(vobsSTAR_POS_EQ_RA_MAIN));

            if (sscanf(ra, "%s %s %s", (char*)&hra, (char*)&mra, (char*)&sra) != 3)
            {
                return mcsFAILURE;
            }
            vobsStrcatFast(valPtr, hra);
            vobsStrcatFast(valPtr, "+");
            vobsStrcatFast(valPtr, mra);
            vobsStrcatFast(valPtr, "+");
            vobsStrcatFast(valPtr, sra);

            strcpy(dec, star->GetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN));

            if (sscanf(dec, "%s %s %s", (char*)&ddec, (char*)&mdec, (char*)&sdec) != 3)
            {
                return mcsFAILURE;
            }
            if (ddec[0] == '+')
            {
                vobsStrcatFast(valPtr, "%2b");
                vobsStrcatFast(valPtr, &ddec[1]);
            }
            else
            {
                vobsStrcatFast(valPtr, ddec);
            }
            vobsStrcatFast(valPtr, "+");
            vobsStrcatFast(valPtr, mdec);
            vobsStrcatFast(valPtr, "+");
            vobsStrcatFast(valPtr, sdec);

            miscDynBufAppendString(&strList, value);
        }
        
        miscDynBufAppendString(&strList, "&%3D%3D%3D%3Dresult1%5F280%2Etxt");
    }
    
    return mcsSUCCESS;
}

/*___oOo___*/
