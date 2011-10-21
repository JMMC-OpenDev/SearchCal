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
    _maxBaselineLength        = 0.0;
    _observingWlen            = 0.0;
    _diamVK                   = 0.0;
    _diamVKDefined            = mcsFALSE;
    _expectedVisibilityError  = 0.0;
    _getCalCmd                = NULL;
    _brightFlag               = mcsTRUE;
    _noScienceObject          = mcsFALSE;
    _fileName[0]              = '\0';
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

/**
 * Copy from another request
 *
 * @param request request to copy
 *
 * @return always mcsSUCCESS
 */
mcsCOMPL_STAT sclsvrREQUEST::Copy(const sclsvrREQUEST& request)
{
    logTrace("sclsvrREQUEST::Copy()");
   
    vobsREQUEST::Copy(request);
    
    _maxBaselineLength        = request._maxBaselineLength;
    _observingWlen            = request._observingWlen;
    _diamVK                   = request._diamVK;
    _diamVKDefined            = request._diamVKDefined;
    _expectedVisibilityError  = request._expectedVisibilityError;
    _getCalCmd                = request._getCalCmd;
    _brightFlag               = request._brightFlag;
    _noScienceObject          = request._noScienceObject;
    strncpy(_fileName, request._fileName, sizeof(_fileName));

    return mcsSUCCESS;
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
    if (_getCalCmd->Parse() == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Object name
    char* objectName = NULL;
    if (_getCalCmd->GetObjectName(&objectName) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Observed magnitude
    mcsDOUBLE magnitude;
    if (_getCalCmd->GetMag(&magnitude) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Search area size
    mcsDOUBLE deltaRa, deltaDec;
    mcsDOUBLE radius;
    mcsLOGICAL circularQueryFlag = _getCalCmd->IsDefinedRadius();
    // If a radius is specified
    if (circularQueryFlag == mcsTRUE)
    {
        if (_getCalCmd->GetRadius(&radius) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }
    else
    {
        // delta ra
        if (_getCalCmd->GetDiffRa(&deltaRa) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
        
        // delta dec
        if (_getCalCmd->GetDiffDec(&deltaDec) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }

    // Band
    char *band;
    if (_getCalCmd->GetBand(&band) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Magnitude is not used for N band
    mcsDOUBLE minRangeMag;
    mcsDOUBLE maxRangeMag;
    if (strcmp(band, "N") != 0)
    {
        // MinRangeMag
        if (_getCalCmd->GetMinMagRange(&minRangeMag) == mcsFAILURE)
        {
            return mcsFAILURE;
        }

        // MaxRangeMag
        if (_getCalCmd->GetMaxMagRange(&maxRangeMag) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }

    // Ra
    char* ra = NULL;
    if (_getCalCmd->GetRa(&ra) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Dec
    char* dec = NULL;
    if (_getCalCmd->GetDec(&dec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // BaseMax
    mcsDOUBLE baseMax;
    if (_getCalCmd->GetBaseMax(&baseMax) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Wlen
    mcsDOUBLE wlen;
    if (_getCalCmd->GetWlen(&wlen) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // DiamVK
    mcsDOUBLE diamVK;
    if (_getCalCmd->IsDefinedDiamVK() == mcsTRUE)
    {
        if (_getCalCmd->GetDiamVK(&diamVK) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }

    // VisErr
    mcsDOUBLE visErr;
    if (_getCalCmd->GetVisErr(&visErr) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Bright/Faint scenario
    mcsLOGICAL brightFlag = mcsTRUE;
    if (_getCalCmd->IsDefinedBright() == mcsTRUE)
    {
        if (_getCalCmd->GetBright(&brightFlag) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }

    // Science star
    mcsLOGICAL noScienceStar = mcsTRUE;
    if (_getCalCmd->IsDefinedNoScienceStar() == mcsTRUE)
    {
        if (_getCalCmd->GetNoScienceStar(&noScienceStar) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }

    // File name
    char* fileName = NULL;
    if (_getCalCmd->IsDefinedFile() == mcsTRUE)
    {
        if (_getCalCmd->GetFile(&fileName) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
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
    // Affect the expected visibility error
    if (SetExpectedVisErr(visErr) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // Affect the magnitude
    if (SetObjectMag(magnitude) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // Magnitude is not used for N band
    if (strcmp(band, "N") != 0)
    {
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
    }
    // Set the search area size
    if (circularQueryFlag == mcsTRUE)
    {
        if (SetSearchArea(radius) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }
    else
    {
        // Set the search area size
        if (SetSearchArea(deltaRa, deltaDec) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }
    // Affect the observed band
    if (SetSearchBand(band) ==  mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // Affect the baseline length
    if (SetMaxBaselineLength(baseMax) ==  mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // Affect the VK diameter
    if (_getCalCmd->IsDefinedDiamVK() == mcsTRUE)
    {
        if (SetDiamVK(diamVK) ==  mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }
    else
    {
        if (ResetDiamVK() == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }
    // Affect the brightness flag
    if (SetBrightFlag(brightFlag) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Affect the science star flag
    if (SetNoScienceStar(noScienceStar) == mcsFAILURE)
    {
        return mcsFAILURE;
    } 
    // Affect the file name
    if (fileName != NULL)
    {
        if (SetFileName(fileName) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
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

    memset(cmdParamLine, '\0', sizeof(mcsSTRING256)); 

    // If no parameter has been given, return NULL 
    if (_getCalCmd == NULL)
    {
        return mcsSUCCESS;
    }

    // Else build the command parameter line and return it
    string str;
    if (_getCalCmd->GetCmdParamLine(str) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    strncpy(cmdParamLine, str.c_str(), sizeof(mcsSTRING256) -1);

    return mcsSUCCESS;
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

    _diamVK        = diamVK;
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

    _diamVK        = 0.0;
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
    
    if (strncpy(_fileName, fileName, sizeof(_fileName)) == NULL)
    {
        return mcsFAILURE;
    }

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
