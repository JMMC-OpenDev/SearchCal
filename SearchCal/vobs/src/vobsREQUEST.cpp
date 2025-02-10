/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Definition of vobsREQUEST class.
 */


/*
 * System Headers
 */
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

/*
 * MCS Headers
 */
#include "mcs.h"
#include "log.h"
#include "err.h"
#include "miscString.h"

/*
 * Local Headers
 */
#include "vobsREQUEST.h"
#include "vobsSTAR.h"
#include "vobsPrivate.h"

/**
 * Class constructor
 */
vobsREQUEST::vobsREQUEST()
{
    _objectName[0] = '\0';
    _objectRa[0] = '\0';
    _objectRaInDeg = NAN;
    _objectDec[0] = '\0';
    _objectDecInDeg = NAN;
    _pmRa = 0.0;
    _pmDec = 0.0;
    _objectMag = 0.0;
    _searchBand[0] = '\0';
    _minMagRange = 0.0;
    _maxMagRange = 0.0;
    _searchAreaGeometry = vobsUNKNOWN;
    _deltaRa = 0.0;
    _deltaDec = 0.0;
    _radius = 0.0;
    _coneSearchRadius = -1.0; // means undefined
}

/**
 * Class destructor
 */
vobsREQUEST::~vobsREQUEST()
{
}

/**
 * Copy from another request
 *
 * @param request request to copy
 *
 * @return always mcsSUCCESS
 */
mcsCOMPL_STAT vobsREQUEST::Copy(const vobsREQUEST& request)
{
    strncpy(_objectName, request._objectName, sizeof (_objectName));
    strncpy(_objectRa, request._objectRa, sizeof (_objectRa));
    strncpy(_objectDec, request._objectDec, sizeof (_objectDec));
    strncpy(_searchBand, request._searchBand, sizeof (_searchBand));

    _objectRaInDeg = request._objectRaInDeg;
    _objectDecInDeg = request._objectDecInDeg;
    _pmRa = request._pmRa;
    _pmDec = request._pmDec;
    _objectMag = request._objectMag;
    _minMagRange = request._minMagRange;
    _maxMagRange = request._maxMagRange;
    _searchAreaGeometry = request._searchAreaGeometry;
    _deltaRa = request._deltaRa;
    _deltaDec = request._deltaDec;
    _radius = request._radius;
    _coneSearchRadius = request._coneSearchRadius;

    return mcsSUCCESS;
}

/*
 * Public methods
 */

/**
 * Set science object name.
 *
 * @param objectName science object name.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsREQUEST::SetObjectName(const char* objectName)
{
    FAIL_NULL(strncpy(_objectName, objectName, sizeof (_objectName)));

    return mcsSUCCESS;
}

/**
 * Get science object name.
 *
 * @return science object name.
 */
const char* vobsREQUEST::GetObjectName(void) const
{
    return _objectName;
}

/**
 * Set science object right ascension.
 *
 * @param objectRa science object right ascension in hms units (hh mm ss).
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsREQUEST::SetObjectRa(const char* objectRa)
{
    // Check format and get RA in deg
    mcsSTRING32 raString;
    raString[0] = '\0';
    strncpy(raString, objectRa, sizeof (raString) - 1);

    // remove trailing and leading spaces
    TRIM_SPACE(raString);

    // RA can be given as HH:MM:SS.TT or HH MM SS.TT
    // Replace ':' by ' '
    FAIL(miscReplaceChrByChr(raString, ':', ' '));

    mcsDOUBLE raDeg;
    FAIL(vobsSTAR::GetRa(raString, raDeg));

    // Set RA in degrees
    _objectRaInDeg = raDeg;

    // Reformat string as 'HH MM SS.TT'
    // Set RA
    vobsSTAR::ToHms(raDeg, _objectRa);

    return mcsSUCCESS;
}

/**
 * Get science object right ascension.
 *
 * @return science object right ascension in hms units (hh mm ss).
 */
const char* vobsREQUEST::GetObjectRa(void) const
{
    return _objectRa;
}

/**
 * Get science object right ascension in degrees
 *
 * @return science object right ascension in degrees.
 */
mcsDOUBLE vobsREQUEST::GetObjectRaInDeg(void) const
{
    return _objectRaInDeg;
}

/**
 * Set science object declination.
 *
 * @param objectDec science object declination in dms units (dd mm ss).
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsREQUEST::SetObjectDec(const char* objectDec)
{
    // Check format
    mcsSTRING32 decString;
    decString[0] = '\0';
    strncpy(decString, objectDec, sizeof (decString) - 1);

    // remove trailing and leading spaces
    TRIM_SPACE(decString);

    // DEC can be given as DD:MM:SS.TT or DD MM SS.TT.
    // Replace ':' by ' '
    FAIL(miscReplaceChrByChr(decString, ':', ' '));

    mcsDOUBLE decDeg;
    FAIL(vobsSTAR::GetDec(decString, decDeg));

    // Set DEC in degrees
    _objectDecInDeg = decDeg;

    // Reformat string as +/-DD:MM:SS.TT
    // Set DEC
    vobsSTAR::ToDms(decDeg, _objectDec);

    return mcsSUCCESS;
}

/**
 * Get science object declination.
 *
 * @return science object declination in dms units (dd mm ss).
 */
const char* vobsREQUEST::GetObjectDec(void) const
{
    return _objectDec;
}

/**
 * Get science object declination in degrees
 *
 * @return science object declination in degrees.
 */
mcsDOUBLE vobsREQUEST::GetObjectDecInDeg(void) const
{
    return _objectDecInDeg;
}

/**
 * Indicates if the science object has coordinates
 * @return mcsTRUE if the science object RA/Dec are defined; mcsFALSE otherwise
 */
mcsLOGICAL vobsREQUEST::hasObjectRaDec() const
{
    return (IS_NULL(_objectRa) || IS_TRUE(miscIsSpaceStr(_objectRa))
            || IS_NULL(_objectDec) || IS_TRUE(miscIsSpaceStr(_objectDec))) ? mcsFALSE : mcsTRUE;
}

/**
 * Set science object proper motion right ascension
 * @param pmRa proper motion right ascension (mas/yr)
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsREQUEST::SetPmRa(const mcsDOUBLE pmRa)
{
    _pmRa = pmRa;
    return mcsSUCCESS;
}

/**
 * Get science object proper motion right ascension
 * @return pmRa proper motion right ascension (mas/yr)
 */
mcsDOUBLE vobsREQUEST::GetPmRa(void) const
{
    return _pmRa;
}

/**
 * Set science object proper motion declination
 * @param pmRa proper motion declination (mas/yr)
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsREQUEST::SetPmDec(const mcsDOUBLE pmDec)
{
    _pmDec = pmDec;
    return mcsSUCCESS;
}

/**
 * Get science object proper motion declination
 * @return proper motion declination (mas/yr)
 */
mcsDOUBLE vobsREQUEST::GetPmDec(void) const
{
    return _pmDec;
}

/**
 * Set science object magnitude.
 *
 * @param objectMag science object magnitude.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsREQUEST::SetObjectMag(const mcsDOUBLE objectMag)
{
    _objectMag = objectMag;

    return mcsSUCCESS;
}

/**
 * Get science object magnitude.
 *
 * @return science object magnitude.
 */
mcsDOUBLE vobsREQUEST::GetObjectMag(void) const
{
    return _objectMag;
}

/**
 * Set search band.
 *
 * @param searchBand search band which is a letter (H, M, N, R, V ...).
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsREQUEST::SetSearchBand(const char* searchBand)
{
    FAIL_NULL(strncpy(_searchBand, searchBand, sizeof (_searchBand)));

    return mcsSUCCESS;
}

/**
 * Get search band.
 *
 * @return search band which is a letter (H, M, N, R, V ...).
 */
const char* vobsREQUEST::GetSearchBand(void) const
{
    return _searchBand;
}

/**
 * Set rectangular search area size.
 *
 * @param deltaRa  ra  range (arcmin) in which catalog stars will be selected
 * @param deltaDec dec range (arcmin) in which catalog stars will be selected
 *
 * @return Always mcsSUCCESS.
 */
mcsCOMPL_STAT vobsREQUEST::SetSearchArea(const mcsDOUBLE deltaRa,
                                         const mcsDOUBLE deltaDec)
{
    _deltaRa = deltaRa;
    _deltaDec = deltaDec;
    _searchAreaGeometry = vobsBOX;

    return mcsSUCCESS;
}

/**
 * Get rectangular search area size.
 *
 * @param deltaRa  ra  range (arcmin) in which catalog stars will be selected
 * @param deltaDec dec range (arcmin) in which catalog stars will be selected
 *
 * @return mcsSUCCESS if the search area geometry is rectangular. Otherwise
 * mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsREQUEST::GetSearchArea(mcsDOUBLE &deltaRa,
                                         mcsDOUBLE &deltaDec) const
{
    FAIL_COND(_searchAreaGeometry != vobsBOX);

    // Compute delta RA taking into account object declination
    // in order to have a large enough RA/DEC range
    // deltaRa = _deltaRa * cos(dec - _deltaDec)
    // Note :
    //   - _deltaDec is given in arcmin and must be converted in degree
    //   - declination is clipped to +/- 85 deg to avoid to have too small box
    //     when observing star very close to a pole.
    mcsDOUBLE dec = fabs(_objectDecInDeg) - _deltaDec / 2.0 / 60.0;
    dec = mcsMIN(dec, 85.0);

    deltaRa = _deltaRa * cos(dec * M_PI / 180.0);

    logDebug("_deltaRa = %lf", _deltaRa);
    logDebug("_deltaDec = %lf", _deltaDec);
    logDebug("_objectDecInDeg = %lf", _objectDecInDeg);
    logDebug("dec = %lf", dec);

    deltaDec = _deltaDec;

    logTest("Search area [%.3lf - %.3lf] (arcmin)", deltaRa, deltaDec);

    return mcsSUCCESS;
}

/**
 * Set circular search area size.
 *
 * @param radius Circular area radius (arcmin) in which catalog stars will be selected
 *
 * @return Always mcsSUCCESS.
 */
mcsCOMPL_STAT vobsREQUEST::SetSearchArea(const mcsDOUBLE radius)
{
    _radius = radius;
    _searchAreaGeometry = vobsCIRCLE;

    return mcsSUCCESS;
}

/**
 * Get circular search area size.
 *
 * @param radius Circular area radius (arcmin) in which catalog stars will be selected
 *
 * @return mcsSUCCESS if the search area geometry is circle. Otherwise
 * mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsREQUEST::GetSearchArea(mcsDOUBLE &radius) const
{
    FAIL_COND(_searchAreaGeometry != vobsCIRCLE);

    radius = _radius;

    return mcsSUCCESS;
}

/**
 * Return the search area geometry, according to which methods were used to set
 * the search area size.
 *
 * @return a vobsSEARCH_AREA_GEOM value.
 */
vobsSEARCH_AREA_GEOM vobsREQUEST::GetSearchAreaGeometry(void) const
{
    return _searchAreaGeometry;
}

/**
 * Set maximum magnitude difference between the selected object minimum
 * magnitude and the science object magnitude.
 *
 * @param minMagRange maximum accepted magnitude difference corresponding to the
 * minimum expected magnitude for the selected object.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsREQUEST::SetMinMagRange(const mcsDOUBLE minMagRange)
{
    _minMagRange = minMagRange;

    return mcsSUCCESS;
}

/**
 * Get maximum magnitude difference between the selected object minimum
 * magnitude and the science object magnitude.
 *
 * @return maximum accepted magnitude difference corresponding to a minimum the
 * expected magnitude for the selected object.
 */
mcsDOUBLE vobsREQUEST::GetMinMagRange(void) const
{
    return _minMagRange;
}

/**
 * Set maximum magnitude difference between the selected object maximum
 * magnitude and the science object magnitude.
 *
 * @param maxMagRange maximum accepted magnitude difference corresponding to the
 * maximum expected magnitude for the selected object.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsREQUEST::SetMaxMagRange(const mcsDOUBLE maxMagRange)
{
    _maxMagRange = maxMagRange;

    return mcsSUCCESS;
}

/**
 * Get maximum magnitude difference between the selected object maximum
 * magnitude and the science object magnitude.
 *
 * @return maximum accepted magnitude difference corresponding to a maximum the
 * expected magnitude for the selected object.
 */
mcsDOUBLE vobsREQUEST::GetMaxMagRange(void) const
{
    return _maxMagRange;
}

/**
 * Display request content (constraints).
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsREQUEST::Display(void) const
{
    logInfo("object name      = %s",  _objectName);
    logInfo("object ra        = %s",  _objectRa);
    logInfo("object dec       = %s",  _objectDec);
    logInfo("object magnitude = %lf", _objectMag);
    logInfo("search band      = %s",  _searchBand);
    logInfo("delta ra         = %lf", _deltaRa);
    logInfo("delta dec        = %lf", _deltaDec);
    logInfo("min mag range    = %lf", _minMagRange);
    logInfo("max mag range    = %lf", _maxMagRange);

    return mcsSUCCESS;
}

/*___oOo___*/
