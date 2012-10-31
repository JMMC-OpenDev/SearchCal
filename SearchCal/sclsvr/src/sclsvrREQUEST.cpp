/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Definition of sclsvrREQUEST class.
 */



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
    _maxBaselineLength = 0.0;
    _observingWlen = 0.0;
    _diamVK = 0.0;
    _diamVKDefined = mcsFALSE;
    _expectedVisibilityError = 0.0;
    _getCalCmd = NULL;
    _brightFlag = mcsTRUE;
    _noScienceObject = mcsFALSE;
    _fileName[0] = '\0';
}

/**
 * Class destructor
 */
sclsvrREQUEST::~sclsvrREQUEST()
{
    if (_getCalCmd != NULL)
    {
        delete(_getCalCmd);
    }
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
 * @param cmdParamLine GETCAL command parameters
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT sclsvrREQUEST::Parse(const char *cmdParamLine)
{
    logTrace("sclsvrREQUEST::Parse()");

    // GETCAL command
    if (_getCalCmd != NULL)
    {
        delete (_getCalCmd);
    }
    _getCalCmd = new sclsvrGETCAL_CMD(sclsvrGETCAL_CMD_NAME, cmdParamLine);

    // Parse command
    FAIL(_getCalCmd->Parse());

    // Object name
    char* objectName = NULL;
    FAIL(_getCalCmd->GetObjectName(&objectName));

    // Observed magnitude
    mcsDOUBLE magnitude;
    FAIL(_getCalCmd->GetMag(&magnitude));

    // Search area size
    mcsDOUBLE deltaRa, deltaDec;
    mcsDOUBLE radius;
    mcsLOGICAL circularQueryFlag = _getCalCmd->IsDefinedRadius();
    // If a radius is specified
    if (circularQueryFlag == mcsTRUE)
    {
        FAIL(_getCalCmd->GetRadius(&radius));
    }
    else
    {
        // delta ra
        FAIL(_getCalCmd->GetDiffRa(&deltaRa));

        // delta dec
        FAIL(_getCalCmd->GetDiffDec(&deltaDec));
    }

    // Band
    char *band;
    FAIL(_getCalCmd->GetBand(&band));

    // Magnitude is not used for N band
    mcsDOUBLE minRangeMag;
    mcsDOUBLE maxRangeMag;
    if (strcmp(band, "N") != 0)
    {
        // MinRangeMag
        FAIL(_getCalCmd->GetMinMagRange(&minRangeMag));

        // MaxRangeMag
        FAIL(_getCalCmd->GetMaxMagRange(&maxRangeMag));
    }

    // Ra
    char* ra = NULL;
    FAIL(_getCalCmd->GetRa(&ra));

    // Dec
    char* dec = NULL;
    FAIL(_getCalCmd->GetDec(&dec));

    // BaseMax
    mcsDOUBLE baseMax;
    FAIL(_getCalCmd->GetBaseMax(&baseMax));

    // Wlen
    mcsDOUBLE wlen;
    FAIL(_getCalCmd->GetWlen(&wlen));

    // DiamVK
    mcsDOUBLE diamVK;
    if (_getCalCmd->IsDefinedDiamVK() == mcsTRUE)
    {
        FAIL(_getCalCmd->GetDiamVK(&diamVK));
    }

    // VisErr
    mcsDOUBLE visErr;
    FAIL(_getCalCmd->GetVisErr(&visErr));

    // Bright/Faint scenario
    mcsLOGICAL brightFlag = mcsTRUE;
    if (_getCalCmd->IsDefinedBright() == mcsTRUE)
    {
        FAIL(_getCalCmd->GetBright(&brightFlag));
    }

    // Science star
    mcsLOGICAL noScienceStar = mcsTRUE;
    if (_getCalCmd->IsDefinedNoScienceStar() == mcsTRUE)
    {
        FAIL(_getCalCmd->GetNoScienceStar(&noScienceStar));
    }

    // File name
    char* fileName = NULL;
    if (_getCalCmd->IsDefinedFile() == mcsTRUE)
    {
        FAIL(_getCalCmd->GetFile(&fileName));
    }

    // Build the request object from the parameters of the command
    // Affect the reference object name
    FAIL(SetObjectName(objectName));

    // Affect the right ascension position
    FAIL(SetObjectRa(ra));

    // Affect the declinaison position
    FAIL(SetObjectDec(dec));

    // Affect the wavelength
    FAIL(SetObservingWlen(wlen));

    // Affect the expected visibility error
    FAIL(SetExpectedVisErr(visErr));

    // Affect the magnitude
    FAIL(SetObjectMag(magnitude));

    // Magnitude is not used for N band
    if (strcmp(band, "N") != 0)
    {
        // Affect the min of the magitude range
        FAIL(SetMinMagRange(minRangeMag));

        // Affect the max of the magnitude range
        FAIL(SetMaxMagRange(maxRangeMag));
    }
    // Set the search area size
    if (circularQueryFlag == mcsTRUE)
    {
        FAIL(SetSearchArea(radius));
    }
    else
    {
        // Set the search area size
        FAIL(SetSearchArea(deltaRa, deltaDec));
    }
    // Affect the observed band
    FAIL(SetSearchBand(band));

    // Affect the baseline length
    FAIL(SetMaxBaselineLength(baseMax));

    // Affect the VK diameter
    if (_getCalCmd->IsDefinedDiamVK() == mcsTRUE)
    {
        FAIL(SetDiamVK(diamVK));
    }
    else
    {
        FAIL(ResetDiamVK());
    }
    // Affect the brightness flag
    FAIL(SetBrightFlag(brightFlag));

    // Affect the science star flag
    FAIL(SetNoScienceStar(noScienceStar));

    // Affect the file name
    if (fileName != NULL)
    {
        FAIL(SetFileName(fileName));
    }

    return mcsSUCCESS;
}

/**
 * Returns the GETCAL command parameter line.
 *
 * @return command parameter line or NULL if it is not initialized 
 */
mcsCOMPL_STAT sclsvrREQUEST::GetCmdParamLine(mcsSTRING256 cmdParamLine) const
{
    logTrace("sclsvrREQUEST::GetCmdParamLine()");

    memset(cmdParamLine, '\0', sizeof (mcsSTRING256));

    // If no parameter has been given, return NULL 
    if (_getCalCmd == NULL)
    {
        return mcsSUCCESS;
    }

    // Else build the command parameter line and return it
    string str;
    FAIL(_getCalCmd->GetCmdParamLine(str));

    strncpy(cmdParamLine, str.c_str(), sizeof (mcsSTRING256) - 1);

    return mcsSUCCESS;
}

/**
 * Set search band (overriden) and update the GETCAL command
 *
 * @param searchBand search band which is a letter (H, M, N, R, V ...).
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT sclsvrREQUEST::SetSearchBand(const char* searchBand)
{
    if (vobsREQUEST::SetSearchBand(searchBand) == mcsSUCCESS)
    {
        if (_getCalCmd == NULL)
        {
            return mcsSUCCESS;
        }
        // create a pointer of cmdPARAM
        cmdPARAM* p;
        FAIL_DO(_getCalCmd->GetParam("band", &p), logWarning("%s parameter doesn't exist", "band"));

        // Assign value to the parameter
        return p->SetUserValue(searchBand);
    }

    return mcsFAILURE;
}

/**
 * Set the minimum and maximum lengths (in meter) of baseline.
 *
 * @return Always mcsSUCCESS.
 */
mcsCOMPL_STAT sclsvrREQUEST::SetMaxBaselineLength(mcsDOUBLE length)
{
    logTrace("sclsvrREQUEST::SetMaxBaselineLength()");

    _maxBaselineLength = length;

    return mcsSUCCESS;
}

/**
 * Get the maximum baseline length.
 *
 * @return maximum baseline length.
 */
mcsDOUBLE sclsvrREQUEST::GetMaxBaselineLength(void) const
{
    logTrace("sclsvrREQUEST::GetMaxBaselineLength()");

    return _maxBaselineLength;
}

/**
 * Set the observing wavelength (um).
 *
 * @return Always mcsSUCCESS.
 */
mcsCOMPL_STAT sclsvrREQUEST::SetObservingWlen(mcsDOUBLE wlen)
{
    logTrace("sclsvrREQUEST::SetObservingWlen()");

    _observingWlen = wlen;

    return mcsSUCCESS;
}

/**
 * Get the observing wavelength.
 *
 * @return observing wavelength.
 */
mcsDOUBLE sclsvrREQUEST::GetObservingWlen(void) const
{
    logTrace("sclsvrREQUEST::GetObservingWlen()");

    return _observingWlen;
}

/**
 * Set the VK diameter.
 *
 * @return Always mcsSUCCESS.
 */
mcsCOMPL_STAT sclsvrREQUEST::SetDiamVK(mcsDOUBLE diamVK)
{
    logTrace("sclsvrREQUEST::SetDiamVK()");

    _diamVK = diamVK;
    _diamVKDefined = mcsTRUE;

    return mcsSUCCESS;
}

/**
 * Reset the VK diameter.
 *
 * @return Always mcsSUCCESS.
 */
mcsCOMPL_STAT sclsvrREQUEST::ResetDiamVK()
{
    logTrace("sclsvrREQUEST::ResetDiamVK()");

    _diamVK = 0.0;
    _diamVKDefined = mcsFALSE;

    return mcsSUCCESS;
}

/**
 * Return whether VK diameter is defined or not.
 *
 * @return mcsTRUE if VK diameter is defined, mcsFALSE otherwise.
 */
mcsLOGICAL sclsvrREQUEST::IsDiamVKDefined(void) const
{
    logTrace("sclsvrREQUEST::IsDiamVKDefined()");

    return _diamVKDefined;
}

/**
 * Get the VK diameter.
 *
 * @return VK diameter.
 */
mcsDOUBLE sclsvrREQUEST::GetDiamVK(void) const
{
    logTrace("sclsvrREQUEST::GetDiamVK()");

    return _diamVK;
}

/**
 * Set the expected visibility error.
 *
 * @return Always mcsSUCCESS.
 */
mcsCOMPL_STAT sclsvrREQUEST::SetExpectedVisErr(mcsDOUBLE expectedVisErr)
{
    logTrace("sclsvrREQUEST::SetExpectedVisErr()");

    _expectedVisibilityError = expectedVisErr;

    return mcsSUCCESS;
}

/**
 * Get the expected visibility error.
 *
 * @return expected visibility error.
 */
mcsDOUBLE sclsvrREQUEST::GetExpectedVisErr(void) const
{
    logTrace("sclsvrREQUEST::GetExpectedVisErr()");

    return _expectedVisibilityError;
}

/**
 * Specify whether the query should return bright (by default) or faint stars.
 *
 * @param brightFlag mcsTRUE if the query should return bright starts, otherwise
 * mcsFALSE to get faint stars.
 *
 * @return Always mcsSUCCESS.
 */
mcsCOMPL_STAT sclsvrREQUEST::SetBrightFlag(mcsLOGICAL brightFlag)
{
    logTrace("sclsvrREQUEST::SetBrightFlag()");

    _brightFlag = brightFlag;

    return mcsSUCCESS;
}

/**
 * Return whether the query should return bright or faint stars.
 *
 * @return mcsTRUE if the query should return bright stars, otherwise mcsFALSE
 * for faint stars.
 */
mcsLOGICAL sclsvrREQUEST::IsBright(void) const
{
    logTrace("sclsvrREQUEST::IsBright()");

    return _brightFlag;
}

/**
 * Specify whether the query should return science star in list or not.
 *
 * @param noScienceStar mcsTRUE if the query should return science star
 * otherwise mcsFALSE to not return science star
 *
 * @return Always mcsSUCCESS.
 */
mcsCOMPL_STAT sclsvrREQUEST::SetNoScienceStar(mcsLOGICAL noScienceStar)
{
    logTrace("sclsvrREQUEST::SetNoScienceStar()");

    _noScienceObject = noScienceStar;

    return mcsSUCCESS;
}

/**
 * Return whether the query should return bright or faint stars.
 *
 * @return mcsTRUE if the query should return science star, otherwise mcsFALSE.
 */
mcsLOGICAL sclsvrREQUEST::IsNoScienceStar(void) const
{
    logTrace("sclsvrREQUEST::IsNoScienceStar()");

    return _noScienceObject;
}

/**
 * Set the file name in which the value will be saved.
 *
 * @param fileName the name of the save file
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT sclsvrREQUEST::SetFileName(mcsSTRING256 fileName)
{
    logTrace("sclsvrREQUEST::SetFileName()");

    FAIL_NULL(strncpy(_fileName, fileName, sizeof (_fileName)));

    return mcsSUCCESS;
}

/**
 * Append a VOTable serailization of the request and its parameters.
 *
 * @param voTable the string in which the PARAMs will be appent
 *
 * @returns an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
const mcsCOMPL_STAT sclsvrREQUEST::AppendParamsToVOTable(string& voTable)
{
    logTrace("sclsvrREQUEST::AppendParamsToVOTable()");

    return _getCalCmd->AppendParamsToVOTable(voTable);
}

/**
 * Get the name of the save file
 *
 * @return name of the save file
 */
const char *sclsvrREQUEST::GetFileName(void) const
{
    logTrace("sclsvrREQUEST::GetFileName()");

    return _fileName;
}

/*___oOo___*/
