/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsCATALOG_MIDI.cpp,v 1.6 2005-02-07 09:47:08 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.5  2005/02/07 09:13:43  gzins
 * Added initialisation of _loaded
 * Removed printf
 * Supported RA and DEC expressed with ':' as separator instead of ' '
 *
 * Revision 1.4  2005/02/04 15:10:25  gluck
 * Update documentation
 *
 * Revision 1.3  2005/02/04 10:49:43  gzins
 * Fixed wrong MIDI catalog name
 *
 * Revision 1.2  2005/02/04 10:42:32  gzins
 * Added RA/DEC delta conversion (minutes (dm) to decimal degrees)
 *
 * Revision 1.1  2005/02/04 09:54:42  gzins
 * Created
 *
 ******************************************************************************/

/**
 * \file
 *  Definition of vobsCATALOG_MIDI class.
 */

static char *rcsId="@(#) $Id: vobsCATALOG_MIDI.cpp,v 1.6 2005-02-07 09:47:08 gzins Exp $"; 
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
#include "math.h"

/*
 * Local Headers 
 */
#include "vobsCATALOG_MIDI.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"
#include "vobsCDATA.h"
#include "vobsPARSER.h"

/**
 * Class constructor
 */
vobsCATALOG_MIDI::vobsCATALOG_MIDI() : _catalogFilename("vobsMidiCatalog.cfg")
{
    // Set catalog name
    SetName("MIDI");
    _loaded = mcsFALSE;
}

/**
 * Class destructor
 */
vobsCATALOG_MIDI::~vobsCATALOG_MIDI()
{
}

/*
 * Public methods
 */

/**
 * Search for star list in MIDI catalog.
 *
 * Build star list from MIDI catalog, where each list star verifies constraints
 * specified by user request.
 *
 * \param request user request specifying search contraints
 * \param list star list provided by the search
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT vobsCATALOG_MIDI::Search(vobsREQUEST &request,
                                       vobsSTAR_LIST &list)
{
    logExtDbg("vobsCATALOG_MIDI::Search()");

    //
    // Load MIDI catalog in star list
    // ------------------------------
    if (Load() == mcsFAILURE)
    {
        // Get catalog name
        mcsSTRING256 catalogName;
        if (GetName(catalogName) == mcsFAILURE)
        {
            return mcsFAILURE;
        }

        errAdd(vobsERR_CATALOG_LOAD, catalogName);
        return mcsFAILURE;
    }
    logTest("MIDI catalog is correctly loaded in a star list");
    
    //
    // Build reference (science) object
    // --------------------------------

    // Get reference object properties
    mcsSTRING256 ra;            // reference object right ascension
    mcsSTRING256 dec;           // reference object declinaison
    mcsFLOAT magnitude;         // reference object magnitude

    // ra
    if (request.GetConstraint(RA_ID, ra) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    // dec
    if (request.GetConstraint(DEC_ID, dec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    // magnitude
    if (request.GetConstraint(STAR_MAGNITUDE_ID, &magnitude) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    // Create the reference star
    vobsSTAR referenceStar;

    // Add reference star properties
    // ra is given as 'HH:MM:SS.TTT', replace ':' by ' '
    miscReplaceChrByChr(ra, ':', ' '); 
    if (referenceStar.SetPropertyValue("POS_EQ_RA_MAIN", ra) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // dec is given as 'DD:MM:SS.TTT', replace ':' by ' '
    miscReplaceChrByChr(dec, ':', ' '); 
    if (referenceStar.SetPropertyValue("POS_EQ_DEC_MAIN", dec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // N magnitude
    if (referenceStar.SetPropertyValue
        (vobsSTAR_PHOT_JHN_N, magnitude) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    //
    // Build constraint list
    // ---------------------
    // Aim is to set search field
    
    // Get reference object constraints
    mcsINT32 diffRa;    // reference object ra constaint
    mcsINT32 diffDec;   // reference object dec constaint
    // ra constraint
    if (request.GetConstraint(SEARCH_BOX_RA_ID, &diffRa) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // dec constraint
    if (request.GetConstraint(SEARCH_BOX_DEC_ID, &diffDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    // Convert minutes (dm) to decimal degrees
    diffRa = diffRa / 60;
    diffDec = diffDec / 60;

    // Create reference object constraint list
    vobsSTAR_COMP_CRITERIA_LIST constraintlist;
    
    // Add reference star constraints to constaint list
    // Field on sky: ra constraint
    if (constraintlist.Add("POS_EQ_RA_MAIN", diffRa) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // Field on sky: dec constraint
    if (constraintlist.Add("POS_EQ_DEC_MAIN", diffDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Magnitude range constraint
    // Object magnitude conversion to IRAS flux 12mu
    mcsFLOAT referenceStarFlux;
    mcsFLOAT minNFlux;
    mcsFLOAT maxNFlux;
    mcsFLOAT diffNFlux;
    mcsFLOAT middleNFlux;
    referenceStarFlux = 0.89 * pow(10, -0.4 * (magnitude - 4.1));

    // Calculate N magnitude range constraint depending on object flux
    // N magnitude range constraint is half N magnitude range
    minNFlux = 5;
    if (referenceStarFlux < 10)
    {
        maxNFlux = 20;
    }
    else if (referenceStarFlux >= 10 && referenceStarFlux <= 100)
    {
        maxNFlux = 50;
    }
    else
    {
        // referenceStarFlux > 100
        maxNFlux = 100;
    }
    diffNFlux = (maxNFlux - minNFlux) / 2;
    middleNFlux = (minNFlux + diffNFlux);

    // Add N flux constraint to constraint list
    if (constraintlist.Add("PHOT_FLUX_IR_12", diffNFlux) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    // Set flux for reference star to the middle of the flux range
    if (referenceStar.SetPropertyValue("PHOT_FLUX_IR_12", 
                                       middleNFlux) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    //
    // Select MIDI catalog stars which verifies constraints
    // ----------------------------------------------------

    for (mcsUINT32 i=0; i<_starList.Size(); i++)
    {
        // Get MIDI catalog star
        vobsSTAR *midiCatalogStar;
        midiCatalogStar = _starList.GetNextStar((mcsLOGICAL)(i==0));
        // Compare MIDI catalog star with reference star
        if (midiCatalogStar->IsSame(referenceStar, &constraintlist) == mcsTRUE)
        {
            // If Compare MIDI catalog star verifies constraint list then add it
            // to the resulting list
            list.AddAtTail(*midiCatalogStar);
        }
    }

    return mcsSUCCESS;
}


/*
 * Private methods
 */

/**
 * Load MIDI catalog.
 *
 * Build star list from MIDI catalog stars.
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT vobsCATALOG_MIDI::Load(void)
{
    logExtDbg("vobsCATALOG_MIDI::Load()");
    
    //
    // Load MIDI catalog into a buffer
    // -------------------------------
    logDebug("Load MIDI catalog ...");

    // Catalog has already been loaded
    if (_loaded == mcsTRUE)
    {
        return mcsSUCCESS;
    }

    // Catalog has not already been loaded
    
    // Search for file location
    const char *catalogFileName;
    catalogFileName = miscLocateFile(_catalogFilename);
    if (catalogFileName == NULL)
    {
        return mcsFAILURE;
    }

    // Resolve environment variables (if any)
    catalogFileName = miscResolvePath(catalogFileName);
    if (catalogFileName == NULL)
    {
        return mcsFAILURE;
    }
    
    // Init buffer to load catalog file
    miscDYN_BUF dynBuf;
    miscDynBufInit(&dynBuf);

    // Load catalog file except comment lines beginning by '#'
    if (miscDynBufLoadFile(&dynBuf, catalogFileName, "#") == mcsFAILURE)
    {
        miscDynBufDestroy(&dynBuf);
        return mcsFAILURE;
    }

    
    //
    // Load CDATA structure with MIDI stars
    // ------------------------------------
    logDebug("Load CDATA structure ...");

    // Create an object CDATA to transfer MIDI catalog from the dynamic
    // buffer
    vobsCDATA cDataStructure;
    
    // Set line to skip to -1 because in the CDATA AppendLines method, number of
    // lines to skip is increased of 1, because there is an empty line between
    // each data line (due to \n to \0 conversion). The CDATA AppendLines method
    // will have to be changed for this reason.
    cDataStructure.SetNbLinesToSkip(-1);

    // For each dynamic buffer line
    int  lineNb = 1;
    char *line = NULL;
    // While it's not the end of file
    while ((line = miscDynBufGetNextLine(&dynBuf, line, mcsTRUE)) != NULL)
    {
        // Trim leading and trailing spaces from the line
        miscTrimString (line, " ");
        
        // If line is not empty
        if (strlen(line) != 0)
        {
            if (lineNb == 1)
            {
                // It's the first line: UCD name line
                // Parse UCD name line and strore them in an array
                mcsSTRING256 ucdNameArray[25];
                mcsUINT32 nbOfUcdName;
                if (miscSplitString(line, '\t', ucdNameArray, 
                                    25, &nbOfUcdName) == mcsFAILURE)
                {
                    errAdd(vobsERR_INVALID_PARSING_UCD_FORMAT);
                    
                    miscDynBufDestroy(&dynBuf);
                    return mcsFAILURE;
                }
                // For each UCD name stored in the array
                logDebug("\t-> Add UCD name to CDATA structure ...");
                logDebug("\t\tNumber of UCD name = %i", nbOfUcdName);
                for (mcsUINT32 i=0; i<nbOfUcdName; i++)
                {
                    // add UCD name to CDATA structure
                    logDebug("\t\t%d- %s\n", i, ucdNameArray[i]);
                    if (cDataStructure.AddUcdName(ucdNameArray[i]) == 
                                                                    mcsFAILURE)
                    {
                        // Do not raise an error because the method returns
                        // always SUCCESS
                        return mcsFAILURE;
                    }
                }
            }
            else if (lineNb == 2)
            {
                // It's the second line: UCD column name line
                // Parse UCD column name line and strore them in an array
                mcsSTRING256 ucdColNameArray[25];
                mcsUINT32 nbOfUcdColName;
                if (miscSplitString(line, '\t', ucdColNameArray, 
                                    25, &nbOfUcdColName) == mcsFAILURE)
                {
                    errAdd(vobsERR_INVALID_PARSING_UCD_FORMAT);

                    miscDynBufDestroy(&dynBuf);
                    return mcsFAILURE;
                }
                // For each UCD column description stored in the array
                logDebug("\t-> Add UCD column name to CDATA structure ...");
                logDebug("\t\tNumber of UCD column name = %i", \
                         nbOfUcdColName);
                for (mcsUINT32 i=0; i<nbOfUcdColName; i++)
                {
                    // add UCD colomn name to CDATA structure
                    logDebug("\t\t%d- %s\n", i, ucdColNameArray[i]);
                    if (cDataStructure.AddColName(ucdColNameArray[i]) == 
                        mcsFAILURE)
                    {
                        // Do not raise an error because the method returns
                        // always SUCCESS
                        return mcsFAILURE;
                    }
                }
            }
            else
            {
                // It's neither the first line the second one: CDATA line
                if (cDataStructure.AppendLines(line) == mcsFAILURE)
                {
                    errAdd(vobsERR_APPEND_LINE_FAILED, line);
                    return mcsFAILURE;
                }
            }
            
            /* Next line */
            lineNb++;
       }
    }
    
    // Return to 0 line to skip (see previous explanation)
    cDataStructure.SetNbLinesToSkip(0);    
    
    // Destroy dynamic buffer
    miscDynBufDestroy(&dynBuf);

    //
    // Parse CDATA and set star list
    // -----------------------------
    logDebug("Parse CDATA and build star list ...");
    
    // Create a parser to parse CDATA
    vobsPARSER parser;
    // Parse CDATA
    if (parser.ParseCData(&cDataStructure, _starList) == mcsFAILURE)
    {
        errAdd(vobsERR_PARSE_CDATA);
    }

    // Compute magnitude in N band
    mcsUINT32 starIdx;
    for (starIdx = 0; starIdx < _starList.Size(); starIdx++)
    {
        // Get star
        vobsSTAR *starPtr;
        starPtr = _starList.GetNextStar((mcsLOGICAL)(starIdx==0));
        
        // Get IR flux
        mcsFLOAT flux;
        mcsFLOAT magnitude;
        starPtr->GetPropertyValue(vobsSTAR_PHOT_FLUX_IR_12, &flux);
        
        // Compute magnitude
        magnitude = 4.1 - (2.5 * log(flux/0.89));
        starPtr->SetPropertyValue(vobsSTAR_PHOT_JHN_N, magnitude);
    }

    // If log level is DEBUG or EXTDBG
    if (logGetStdoutLogLevel() >= logDEBUG)
    {
        if (_starList.Save("$MCSDATA/tmp/catalogMIDI.dat") == mcsFAILURE)
        {
            // Ignore error (for test only)
            errCloseStack();
        }
    }

    // Set flag indicating a correct catalog load
    _loaded = mcsTRUE;
    
    return mcsSUCCESS;
}



/*___oOo___*/
