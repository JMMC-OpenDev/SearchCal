/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrREQUEST.cpp,v 1.3 2005-02-16 16:56:30 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2005/02/15 15:54:00  gzins
 * Changed remaining SUCCESS/FAILURE to mcsSUCCESS/mcsFAILURE
 *
 * Revision 1.1  2005/02/07 14:43:41  gzins
 * Created
 *
 ******************************************************************************/

/**
 * \file
 * Definition of sclsvrREQUEST class.
 */

static char *rcsId="@(#) $Id: sclsvrREQUEST.cpp,v 1.3 2005-02-16 16:56:30 gzins Exp $"; 
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
#include "sclsvrREQUEST.h"
#include "sclsvrGETCAL_CMD.h"
#include "sclsvrPrivate.h"

/**
 * Class constructor
 */
sclsvrREQUEST::sclsvrREQUEST()
{
    _minBaselineLength = 0.0;
    _maxBaselineLength = 0.0;
    _observingWlen = 0.0;
    _vis = 0.0;
    _visErr = 0.0;
}

/**
 * Class destructor
 */
sclsvrREQUEST::~sclsvrREQUEST()
{
}

/*
 * Public methods
 */
/**
 * Parses the GETCAL command parameters.
 *
 * It retrieves the GETCAL command parameter values and set its corresponding
 * members according.
 *
 * \param cmdParams GETCAL command parameters
 * \return Always mcsSUCCESS.
 */
mcsCOMPL_STAT sclsvrREQUEST::Parse(const char *cmdParams)
{
    logExtDbg("sclsvrREQUEST::Parse()");

    // GETCAL command
    sclsvrGETCAL_CMD getCalCmd(sclsvrGETCAL_CMD_NAME, cmdParams);
    
    // Parse command
    if (getCalCmd.Parse() == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Object name
    char *objectName;
    if (getCalCmd.GetObjectName(&objectName) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Observed magnitude
    mcsDOUBLE magnitude;
    if (getCalCmd.GetMag(&magnitude) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // max calibrator return
    mcsINT32 maxReturn;
    if (getCalCmd.GetMaxReturn(&maxReturn) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // diff ra
    mcsINT32 diffRa;
    if (getCalCmd.GetDiffRa(&diffRa) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    // diff dec
    mcsINT32 diffDec;
    if (getCalCmd.GetDiffDec(&diffDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // band
    char *band;
    if (getCalCmd.GetBand(&band) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // minRangeMag
    mcsDOUBLE minRangeMag;
    if (getCalCmd.GetMinMagRange(&minRangeMag) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // maxRangeMag
    mcsDOUBLE maxRangeMag;
    if (getCalCmd.GetMaxMagRange(&maxRangeMag) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // ra
    char *ra;
    if (getCalCmd.GetRa(&ra) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // dec
    char *dec;
    if (getCalCmd.GetDec(&dec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // baseMin
    mcsDOUBLE baseMin;
    if (getCalCmd.GetBaseMin(&baseMin) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // baseMax
    mcsDOUBLE baseMax;
    if (getCalCmd.GetBaseMax(&baseMax) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // wlen
    mcsDOUBLE wlen;
    if (getCalCmd.GetWlen(&wlen) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // visibility
    mcsDOUBLE vis;
    if (getCalCmd.GetVis(&vis) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // visibility error
    mcsDOUBLE visErr;
    if (getCalCmd.GetVisErr(&visErr) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    // Build the request object from the parameters of the command
    // Affect the reference object name
    if (SetObjectName(objectName) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // Affect the right ascension position
    if (SetObjectRa(ra) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // Affect the declinaison position
    if (SetObjectDec(dec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // Affect the wavelength
    if (SetObservingWlen(wlen) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // Affect the magnitude
    if (SetObjectMag(magnitude) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // Affect the min of the magitude range
    if (SetMinMagRange(minRangeMag) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // Affect the max of the magnitude range
    if (SetMaxMagRange(maxRangeMag) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // Affect the search box ra
    if (SetDeltaRa(diffRa) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // Affect the serach box dec
    if (SetDeltaDec(diffDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // Affect the expected visibility
    if (SetExpectedVisibility(vis, visErr) == mcsFAILURE)
    {
    }
    // Affect the observed band
    if (SetSearchBand(band) ==  mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // Affect the baseline length
    if (SetBaseline(baseMin, baseMax) ==  mcsFAILURE)
    {
        return mcsFAILURE;
    }
    return mcsSUCCESS;
}

/**
 * Set the minimum and maximum lengths (in meter) of baseline.
 *
 * \return Always mcsSUCCESS.
 */
mcsCOMPL_STAT sclsvrREQUEST::SetBaseline(mcsFLOAT minLength, mcsFLOAT maxLength)
{
    logExtDbg("sclsvrREQUEST::SetBaseline()");

    _minBaselineLength = minLength;
    _maxBaselineLength = maxLength;
    
    return mcsSUCCESS;
}

/**
 * Get the minimum baseline length.
 *
 * \return minimum baseline length.
 */
mcsFLOAT sclsvrREQUEST::GetMinBaselineLength(void)
{
    logExtDbg("sclsvrREQUEST::GetMinBaselineLength()");

    return (_minBaselineLength);
}

/**
 * Get the maximum baseline length.
 *
 * \return maximum baseline length.
 */
mcsFLOAT sclsvrREQUEST::GetMaxBaselineLength(void)
{
    logExtDbg("sclsvrREQUEST::GetMaxBaselineLength()");

    return (_maxBaselineLength);
}

/**
 * Set the observing wavelength (um).
 *
 * \return Always mcsSUCCESS.
 */
mcsCOMPL_STAT sclsvrREQUEST::SetObservingWlen(mcsFLOAT wlen)
{
    logExtDbg("sclsvrREQUEST::SetObservingWlen()");

    _observingWlen = wlen;
    
    return mcsSUCCESS;
}

/**
 * Get the observing wavelength.
 *
 * \return observing wavelength.
 */
mcsFLOAT sclsvrREQUEST::GetObservingWlen(void)
{
    logExtDbg("sclsvrREQUEST::GetObservingWlen()");

    return (_observingWlen);
}

/**
 * Set the expected visibility, with the associated error.
 *
 * \return Always mcsSUCCESS.
 */
mcsCOMPL_STAT sclsvrREQUEST::SetExpectedVisibility(mcsFLOAT vis, 
                                                   mcsFLOAT visErr)
{
    logExtDbg("sclsvrREQUEST::SetExpectedVisibility()");

    _vis    = vis;
    _visErr = visErr;
    
    return mcsSUCCESS;
}

/**
 * Get the expected visibility.
 *
 * \return expected visibility.
 */
mcsFLOAT sclsvrREQUEST::GetExpectedVisibility(void)
{
    logExtDbg("sclsvrREQUEST::GetExpectedVisibility()");

    return (_vis);
}

/**
 * Get the expected visibility error.
 *
 * \return expected visibility error.
 */
mcsFLOAT sclsvrREQUEST::GetExpectedVisibilityError(void)
{
    logExtDbg("sclsvrREQUEST::GetExpectedVisibilityError()");

    return (_visErr);
}

/*___oOo___*/
