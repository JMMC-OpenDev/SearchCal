/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrREQUEST.cpp,v 1.2 2005-02-15 15:54:00 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2005/02/07 14:43:41  gzins
 * Created
 *
 ******************************************************************************/

/**
 * \file
 * Definition of sclsvrREQUEST class.
 */

static char *rcsId="@(#) $Id: sclsvrREQUEST.cpp,v 1.2 2005-02-15 15:54:00 gzins Exp $"; 
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
