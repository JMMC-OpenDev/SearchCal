/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsREQUEST.cpp,v 1.11 2005-02-15 15:19:30 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
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
 * \file
 *  Definition of vobsREQUEST class.
 */

static char *rcsId="@(#) $Id: vobsREQUEST.cpp,v 1.11 2005-02-15 15:19:30 gzins Exp $"; 
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
#include "vobsREQUEST.h"
#include "vobsPrivate.h"


/**
 * Class constructor
 */
vobsREQUEST::vobsREQUEST()
{
    _objectName = "";
    _objectRa = "";
    _objectDec = "";
    _objectMag = 0.0;
    _searchBand = "";
    _deltaRa = 0.0;
    _deltaDec = 0.0;
    _minMagRange = 0.0;
    _maxMagRange = 0.0;
    _maxNbOfSelectedObjects = 0;
}


/**
 * Class destructor
 */
vobsREQUEST::~vobsREQUEST()
{
}


/*
 * Public methods
 */

/**
 * Set science object name.
 *
 * \param objectName science object name.
 *
 * \return mcsmcsSUCCESS on successful completion. Otherwise mcsmcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT vobsREQUEST::SetObjectName(const char *objectName)
{
    logExtDbg("vobsREQUEST::SetObjectName()");

    _objectName = objectName;

    return mcsSUCCESS;
}

/**
 * Get science object name.
 *
 * \return science object name.
 */
const char * vobsREQUEST::GetObjectName(void) const
{
    logExtDbg("vobsREQUEST::GetObjectName()");

    // Return and convert object name to regular C string
    return _objectName.c_str();
}

/**
 * Set science object right ascension.
 *
 * \param objectRa science object right ascension in hms units (hh mm ss).
 *
 * \return mcsmcsSUCCESS on successful completion. Otherwise mcsmcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT vobsREQUEST::SetObjectRa(const char *objectRa)
{
    logExtDbg("vobsREQUEST::SetObjectRa()");

    _objectRa = objectRa;

    return mcsSUCCESS;
}

/**
 * Get science object right ascension.
 *
 * \return science object right ascension in hms units (hh mm ss).
 */
const char *vobsREQUEST::GetObjectRa(void) const
{
    logExtDbg("vobsREQUEST::GetObjectRa()");

    return _objectRa.c_str();
}

/**
 * Set science object declinaison.
 *
 * \param objectDec science object declinaison in dms units (dd mm ss).
 *
 * \return mcsmcsSUCCESS on successful completion. Otherwise mcsmcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT vobsREQUEST::SetObjectDec(const char *objectDec)
{
    logExtDbg("vobsREQUEST::SetObjectDec()");

    _objectDec = objectDec;
    
    return mcsSUCCESS;
}

/**
 * Get science object declinaison.
 *
 * \return science object declinaison in dms units (dd mm ss).
 */
const char *vobsREQUEST::GetObjectDec(void) const
{
    logExtDbg("vobsREQUEST::GetObjectDec()");

    return _objectDec.c_str();
}

/**
 * Set science object magnitude.
 *
 * \param objectMag science object magnitude.
 *
 * \return mcsmcsSUCCESS on successful completion. Otherwise mcsmcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT vobsREQUEST::SetObjectMag(const mcsFLOAT objectMag)
{
    logExtDbg("vobsREQUEST::SetObjectMag()");

    _objectMag = objectMag;

    return mcsSUCCESS;
}

/**
 * Get science object magnitude.
 *
 * \return science object magnitude.
 */
mcsFLOAT vobsREQUEST::GetObjectMag(void) const
{
    logExtDbg("vobsREQUEST::GetObjectMag()");

    return _objectMag;
}

/**
 * Set search band.
 *
 * \param searchBand search band which is a letter (H, M, N, R, V ...).
 *
 * \return mcsmcsSUCCESS on successful completion. Otherwise mcsmcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT vobsREQUEST::SetSearchBand(const char *searchBand)
{
    logExtDbg("vobsREQUEST::SetSearchBand()");

    _searchBand = searchBand;

    return mcsSUCCESS;
}

/**
 * Get search band.
 *
 * \return search band which is a letter (H, M, N, R, V ...).
 */
const char *vobsREQUEST::GetSearchBand(void) const
{
    logExtDbg("vobsREQUEST::GetSearchBand()");

    return _searchBand.c_str();
}

/**
 * Set object ra difference in which catalog stars will be selected.
 *
 * \param deltaRa accepted object ra difference in hms units (hh mm ss).
 *
 * \return mcsmcsSUCCESS on successful completion. Otherwise mcsmcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT vobsREQUEST::SetDeltaRa(const mcsFLOAT deltaRa)
{
    logExtDbg("vobsREQUEST::SetDeltaRa()");

    _deltaRa = deltaRa;

    return mcsSUCCESS;
}

/**
 * Get object ra difference in which catalog stars will be selected.
 *
 * \return accepted object ra difference in hms units (hh mm ss).
 */
mcsFLOAT vobsREQUEST::GetDeltaRa(void) const
{
    logExtDbg("vobsREQUEST::GetDeltaRa()");

    return _deltaRa;
}

/**
 * Set object dec difference in which catalog stars will be selected.
 *
 * \param deltaDec accepted object dec difference in dms units (dd mm ss).
 *
 * \return mcsmcsSUCCESS on successful completion. Otherwise mcsmcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT vobsREQUEST::SetDeltaDec(const mcsFLOAT deltaDec)
{
    logExtDbg("vobsREQUEST::SetDeltaDec()");

    _deltaDec = deltaDec;

    return mcsSUCCESS;
}

/**
 * Get object dec difference in which catalog stars will be selected.
 *
 * \return accepted object dec difference in dms units (dd mm ss).
 */
mcsFLOAT vobsREQUEST::GetDeltaDec(void) const
{
    logExtDbg("vobsREQUEST::GetDeltaDec()");

    return _deltaDec;
}

/**
 * Set maximum magnitude difference between the selected object minimum
 * magnitude and the science object magnitude.
 *
 * \param minMagRange maximum accepted magnitude difference correcponding to the
 * minimum expected magnitude for the selected object.
 *
 * \return mcsmcsSUCCESS on successful completion. Otherwise mcsmcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT vobsREQUEST::SetMinMagRange(const mcsFLOAT minMagRange)
{
    logExtDbg("vobsREQUEST::SetMinMagRange()");

    _minMagRange = minMagRange;

    return mcsSUCCESS;
}

/**
 * Get maximum magnitude difference between the selected object minimum
 * magnitude and the science object magnitude.
 *
 * \return maximum accepted magnitude difference correcponding to a minimum the
 * expected magnitude for the selected object.
 */
mcsFLOAT vobsREQUEST::GetMinMagRange(void) const
{
    logExtDbg("vobsREQUEST::GetMinMagRange()");

    return _minMagRange;
}


/**
 * Set maximum magnitude difference between the selected object maximum
 * magnitude and the science object magnitude.
 *
 * \param maxMagRange maximum accepted magnitude difference correcponding to the
 * maximum expected magnitude for the selected object.
 *
 * \return mcsmcsSUCCESS on successful completion. Otherwise mcsmcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT vobsREQUEST::SetMaxMagRange(const mcsFLOAT maxMagRange)
{
    logExtDbg("vobsREQUEST::SetMaxMagRange()");

    _maxMagRange = maxMagRange;

    return mcsSUCCESS;
}

/**
 * Get maximum magnitude difference between the selected object maximum
 * magnitude and the science object magnitude.
 *
 * \return maximum accepted magnitude difference correcponding to a maximum the
 * expected magnitude for the selected object.
 */
mcsFLOAT vobsREQUEST::GetMaxMagRange(void) const
{
    logExtDbg("vobsREQUEST::GetMaxMagRange()");

    return _maxMagRange;
}

/**
 * Set maximum number of selected objects.
 *
 * \param maxNbOfSelectedObjects maximum number of selected objects.
 *
 * \return mcsmcsSUCCESS on successful completion. Otherwise mcsmcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT vobsREQUEST::SetMaxNbOfSelectedObjects(const mcsINT32 
                                                     maxNbOfSelectedObjects)
{
    logExtDbg("vobsREQUEST::SetMaxNbOfSelectedObjects()");

    _maxNbOfSelectedObjects = maxNbOfSelectedObjects;

    return mcsSUCCESS;
}

/**
 * Get maximum number of selected objects.
 *
 * \return maximum number of selected objects.
 */
mcsINT32 vobsREQUEST::GetMaxNbOfSelectedObjects(void) const
{
    logExtDbg("vobsREQUEST::GetMaxNbOfSelectedObjects()");

    return _maxNbOfSelectedObjects;
}

/**
 * Display request containt (constraints).
 *
 * \return mcsmcsSUCCESS on successful completion. Otherwise mcsmcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT vobsREQUEST::Display(void)
{
    logExtDbg("vobsREQUEST::Display()");
    
    logInfo("object name = %s", _objectName.c_str());
    logInfo("object ra = %s", _objectRa.c_str());
    logInfo("object dec = %s", _objectDec.c_str());
    logInfo("object magnitude = %f", _objectMag);
    logInfo("search band = %s", _searchBand.c_str());
    logInfo("delta ra = %f", _deltaRa);
    logInfo("delta dec = %f", _deltaDec);
    logInfo("min mag range = %f", _minMagRange);
    logInfo("max mag range = %f", _maxMagRange);
    logInfo("max nb of selected object = %i", _maxNbOfSelectedObjects);
    
    return mcsSUCCESS;
}

/*___oOo___*/
