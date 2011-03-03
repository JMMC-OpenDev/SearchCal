/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsREQUEST.cpp,v 1.35 2011-03-03 13:09:43 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.34  2006/04/10 11:48:10  gzins
 * Change logTest to logDebug for delta ra/dec parameters
 *
 * Revision 1.33  2006/03/03 15:03:28  scetre
 * Changed rcsId to rcsId __attribute__ ((unused))
 *
 * Revision 1.32  2006/02/22 11:21:03  gzins
 * Fixed bug related to RA/DEC format
 *
 * Revision 1.31  2005/12/07 15:10:09  scetre
 * Removed unused printf
 *
 * Revision 1.30  2005/11/30 15:27:59  gzins
 * Took into account object declinaison when computing box size along X direction
 *
 * Revision 1.29  2005/11/29 08:22:23  scetre
 * Minor changes
 *
 * Revision 1.28  2005/11/23 17:30:21  lafrasse
 * Added circular search box geometry support and normalized area size methods
 *
 * Revision 1.27  2005/11/23 15:49:45  lafrasse
 * Removed _maxNbOfSelectedObjects member and associated code
 *
 * Revision 1.26  2005/11/23 15:41:15  lafrasse
 * Removed GetMaxNbOfSelectedObjects() and SetMaxNbOfSelectedObjects() methods
 *
 * Revision 1.25  2005/11/23 08:37:05  scetre
 * Cancel previous change
 *
 * Revision 1.24  2005/11/23 08:36:01  scetre
 * Added test on ucd and name to retreive properties
 *
 * Revision 1.23  2005/11/16 10:47:55  scetre
 * Updated documentation
 *
 * Revision 1.22  2005/11/16 10:47:54  scetre
 * Updated documentation
 *
 * Revision 1.21  2005/11/15 14:57:56  scetre
 * Added new scenario structure
 * Added possibility to query merand and borde as primary catalog
 *
 * Revision 1.20  2005/06/01 14:16:55  scetre
 * Changed logExtDbg to logTrace
 *
 * Revision 1.19  2005/04/14 14:39:03  scetre
 * Updated documentation.
 * added test on method return.
 *
 * Revision 1.18  2005/03/06 10:41:33  gzins
 * Removed printf
 *
 * Revision 1.17  2005/03/04 16:58:15  scetre
 * Changed format of DEC
 *
 * Revision 1.16  2005/03/04 16:53:16  scetre
 * Removed useless sign when reformatting RA
 *
 * Revision 1.15  2005/03/04 08:07:58  gzins
 * Removed undesirated '\n' when re-formatting RA/DEC
 *
 * Revision 1.14  2005/03/04 07:51:30  gzins
 * Fixed bug in new added code for RA/DEC check
 *
 * Revision 1.13  2005/03/04 06:37:01  gzins
 * Checked and reformated RA and DEC parameter
 *
 * Revision 1.12  2005/02/15 15:41:47  gzins
 * Fixed wrong mcsmcsSUCCESS and mcsmcsFAILURE
 *
 * Revision 1.11  2005/02/15 15:19:30  gzins
 * Changed SUCCESS/FAILURE to mcsSUCCESS/mcsFAILURE
 *
 * Revision 1.10  2005/02/14 15:22:44  scetre
 * changed minRangeMag to minMagRange and maxRangeMag to maxMagRange
 *
 * Revision 1.9  2005/02/13 15:24:50  gzins
 * Change Min/MaxDeltaMag to Min/MaxMagRange
 *
 * Revision 1.8  2005/02/11 16:21:57  gluck
 * Changed Set/Get max number of selected object type parameters: mcsUINT32 -> mcsINT32
 *
 * Revision 1.7  2005/02/07 19:35:05  gzins
 * Changed RA, DEC and band type as well as prototypes of the related methods
 *
 * Revision 1.6  2005/02/07 17:28:11  gluck
 * New vobsREQUEST class implementation
 *
 ******************************************************************************/

/**
 * @file
 *  Definition of vobsREQUEST class.
 */

static char *rcsId __attribute__ ((unused)) ="@(#) $Id: vobsREQUEST.cpp,v 1.35 2011-03-03 13:09:43 lafrasse Exp $"; 

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
#include "vobsREQUEST.h"
#include "vobsSTAR.h"
#include "vobsPrivate.h"


/**
 * Class constructor
 */
vobsREQUEST::vobsREQUEST()
{
    _objectName         = "";
    _objectRa           = "";
    _objectDec          = "";
    _objectDecInDeg     = 0.0;
    _objectMag          = 0.0;
    _searchBand         = "";
    _minMagRange        = 0.0;
    _maxMagRange        = 0.0;
    _searchAreaGeometry = vobsUNKNOWN;
    _deltaRa            = 0.0;
    _deltaDec           = 0.0;
    _radius             = 0.0;
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
mcsCOMPL_STAT vobsREQUEST::Copy(vobsREQUEST& request)
{
    logTrace("vobsREQUEST::Copy()");
    
    _objectName         = request._objectName;
    _objectRa           = request._objectRa;
    _objectDec          = request._objectDec;
    _objectDecInDeg     = request._objectDecInDeg;
    _objectMag          = request._objectMag;
    _searchBand         = request._searchBand;
    _minMagRange        = request._minMagRange;
    _maxMagRange        = request._maxMagRange;
    _searchAreaGeometry = request._searchAreaGeometry;
    _deltaRa            = request._deltaRa;
    _deltaDec           = request._deltaDec;
    _radius             = request._radius;

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
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT vobsREQUEST::SetObjectName(const char *objectName)
{
    logTrace("vobsREQUEST::SetObjectName()");

    _objectName = objectName;

    return mcsSUCCESS;
}

/**
 * Get science object name.
 *
 * @return science object name.
 */
const char * vobsREQUEST::GetObjectName(void) const
{
    logTrace("vobsREQUEST::GetObjectName()");

    // Return and convert object name to regular C string
    return _objectName.c_str();
}

/**
 * Set science object right ascension.
 *
 * @param objectRa science object right ascension in hms units (hh mm ss).
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT vobsREQUEST::SetObjectRa(const char *objectRa)
{
    logTrace("vobsREQUEST::SetObjectRa()");

    // Check format and get RA in deg
    vobsSTAR star;
    if (star.SetPropertyValue(vobsSTAR_POS_EQ_RA_MAIN, 
                              objectRa, "") == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    mcsDOUBLE ra;
    if (star.GetRa(ra) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Reformat string as +/-HH:MM:SS.TT
    mcsSTRING64 raHms;
    mcsDOUBLE    hh, hm, hs;
    // Be sure RA is positive [0 - 360]
    if (ra < 0)
    {
        ra = ra + 360;
    }
    ra = ra/15.0;
    hh = (int) (ra);
    hm = (int) ((ra - hh)*60.0);
    hs = (ra - hh - hm/60.0)*3600.0;

    sprintf(raHms, "%02d:%02d:%05.2f", 
            (int)fabs(hh), (int)fabs(hm), fabs(hs));

    // Set RA
    _objectRa = raHms;

    return mcsSUCCESS;
}

/**
 * Get science object right ascension.
 *
 * @return science object right ascension in hms units (hh mm ss).
 */
const char *vobsREQUEST::GetObjectRa(void) const
{
    logTrace("vobsREQUEST::GetObjectRa()");

    return _objectRa.c_str();
}

/**
 * Set science object declinaison.
 *
 * @param objectDec science object declinaison in dms units (dd mm ss).
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT vobsREQUEST::SetObjectDec(const char *objectDec)
{
    logTrace("vobsREQUEST::SetObjectDec()");

    // Check format
    vobsSTAR star;
    if (star.SetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN, 
                              objectDec, "") == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (star.GetDec(_objectDecInDeg) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Reformat string as +/-DD:MM:SS.TT
    mcsSTRING64 decDms;
    mcsDOUBLE    dd, hm, hs;
    dd = (int) (_objectDecInDeg);
    hm = (int) ((_objectDecInDeg - dd)*60.0);
    hs = (_objectDecInDeg - dd - hm/60.0)*3600.0;

    sprintf(decDms, "%c%02d:%02d:%04.1f", 
            (_objectDecInDeg < 0)?'-':'+', (int)fabs(dd), (int)fabs(hm), fabs(hs));

    // Set DEC
    _objectDec = decDms;
    
    return mcsSUCCESS;
}

/**
 * Get science object declinaison.
 *
 * @return science object declinaison in dms units (dd mm ss).
 */
const char *vobsREQUEST::GetObjectDec(void) const
{
    logTrace("vobsREQUEST::GetObjectDec()");

    return _objectDec.c_str();
}

/**
 * Set science object magnitude.
 *
 * @param objectMag science object magnitude.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT vobsREQUEST::SetObjectMag(const mcsDOUBLE objectMag)
{
    logTrace("vobsREQUEST::SetObjectMag()");

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
    logTrace("vobsREQUEST::GetObjectMag()");

    return _objectMag;
}

/**
 * Set search band.
 *
 * @param searchBand search band which is a letter (H, M, N, R, V ...).
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT vobsREQUEST::SetSearchBand(const char *searchBand)
{
    logTrace("vobsREQUEST::SetSearchBand()");

    _searchBand = searchBand;

    return mcsSUCCESS;
}

/**
 * Get search band.
 *
 * @return search band which is a letter (H, M, N, R, V ...).
 */
const char *vobsREQUEST::GetSearchBand(void) const
{
    logTrace("vobsREQUEST::GetSearchBand()");

    return _searchBand.c_str();
}

/**
 * Set rectangular search area size.
 *
 * @param deltaRa ra range in which catalog stars will be selected in hms unit
 * (hh mm ss).
 * @param deltaDec dec range in which catalog stars will be selected in dms unit
 * (dd mm ss).
 *
 * @return Always mcsSUCCESS.
 */
mcsCOMPL_STAT vobsREQUEST::SetSearchArea(const mcsDOUBLE deltaRa,
                                         const mcsDOUBLE deltaDec)
{
    logTrace("vobsREQUEST::SetSearchArea(mcsDOUBLE, mcsDOUBLE)");

    _deltaRa  = deltaRa;
    _deltaDec = deltaDec;
    _searchAreaGeometry = vobsBOX;

    return mcsSUCCESS;
}

/**
 * Get rectangular search area size.
 *
 * @param deltaRa ra range in which catalog stars will be selected in hms unit
 * (hh mm ss).
 * @param deltaDec dec range in which catalog stars will be selected in dms unit
 * (dd mm ss).
 *
 * @return mcsSUCCESS if the search area geometry is rectangular. Otherwise
 * mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsREQUEST::GetSearchArea(mcsDOUBLE &deltaRa,
                                         mcsDOUBLE &deltaDec) const
{
    logTrace("vobsREQUEST::GetSearchArea(mcsDOUBLE&, mcsDOUBLE&)");

    if (_searchAreaGeometry != vobsBOX)
    {
        return mcsFAILURE;
    }

    // Compute delta RA taking into account object declinaison.
    // deltaRa = _deltaRa * cos(dec - _deltaDec)
    // Note :
    //   - _deltaDec is given in arcmin and must be convert in degree
    //   - declinaison is clipped to +/- 85 deg to avoid to have too small box
    //     when observing star very close to a pole.
    mcsDOUBLE dec;
    dec =fabs(_objectDecInDeg) - _deltaDec/2.0/60.0;
    dec = mcsMIN (dec, 85.0);

    deltaRa  = _deltaRa * cos(dec * M_PI / 180.0);

    logDebug("_deltaRa = %f", _deltaRa); 
    logDebug("_objectDecInDeg = %f", _objectDecInDeg); 
    logDebug("_deltaDec = %f", _deltaDec); 
    logDebug("dec = %f", dec); 

    deltaDec = _deltaDec;

    logTest("Search area [%.3f - %.3f]", deltaRa, deltaDec); 

    return mcsSUCCESS;
}

/**
 * Set circular search area size.
 *
 * @param radius Circular area radius in which catalog stars will be selected in
 * arcmin.
 *
 * @return Always mcsSUCCESS.
 */
mcsCOMPL_STAT vobsREQUEST::SetSearchArea(const mcsDOUBLE radius)
{
    logTrace("vobsREQUEST::SetSearchArea(mcsDOUBLE)");

    _radius  = radius;
    _searchAreaGeometry = vobsCIRCLE;

    return mcsSUCCESS;
}

/**
 * Get circular search area size.
 *
 * @param radius Circular area radius in which catalog stars will be selected in
 * arcmin.
 *
 * @return mcsSUCCESS if the search area geometry is circle. Otherwise
 * mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsREQUEST::GetSearchArea(mcsDOUBLE &radius) const
{
    logTrace("vobsREQUEST::GetSearchArea(mcsDOUBLE&)");

    if (_searchAreaGeometry != vobsCIRCLE)
    {
        return mcsFAILURE;
    }

    radius  = _radius;

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
 * @param minMagRange maximum accepted magnitude difference correcponding to the
 * minimum expected magnitude for the selected object.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT vobsREQUEST::SetMinMagRange(const mcsDOUBLE minMagRange)
{
    logTrace("vobsREQUEST::SetMinMagRange()");

    _minMagRange = minMagRange;

    return mcsSUCCESS;
}

/**
 * Get maximum magnitude difference between the selected object minimum
 * magnitude and the science object magnitude.
 *
 * @return maximum accepted magnitude difference correcponding to a minimum the
 * expected magnitude for the selected object.
 */
mcsDOUBLE vobsREQUEST::GetMinMagRange(void) const
{
    logTrace("vobsREQUEST::GetMinMagRange()");

    return _minMagRange;
}

/**
 * Set maximum magnitude difference between the selected object maximum
 * magnitude and the science object magnitude.
 *
 * @param maxMagRange maximum accepted magnitude difference correcponding to the
 * maximum expected magnitude for the selected object.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT vobsREQUEST::SetMaxMagRange(const mcsDOUBLE maxMagRange)
{
    logTrace("vobsREQUEST::SetMaxMagRange()");

    _maxMagRange = maxMagRange;

    return mcsSUCCESS;
}

/**
 * Get maximum magnitude difference between the selected object maximum
 * magnitude and the science object magnitude.
 *
 * @return maximum accepted magnitude difference correcponding to a maximum the
 * expected magnitude for the selected object.
 */
mcsDOUBLE vobsREQUEST::GetMaxMagRange(void) const
{
    logTrace("vobsREQUEST::GetMaxMagRange()");

    return _maxMagRange;
}

/**
 * Display request containt (constraints).
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT vobsREQUEST::Display(void)
{
    logTrace("vobsREQUEST::Display()");
    
    logInfo("object name      = %s", _objectName.c_str());
    logInfo("object ra        = %s", _objectRa.c_str());
    logInfo("object dec       = %s", _objectDec.c_str());
    logInfo("object magnitude = %f", _objectMag);
    logInfo("search band      = %s", _searchBand.c_str());
    logInfo("delta ra         = %f", _deltaRa);
    logInfo("delta dec        = %f", _deltaDec);
    logInfo("min mag range    = %f", _minMagRange);
    logInfo("max mag range    = %f", _maxMagRange);
    
    return mcsSUCCESS;
}

/*___oOo___*/
