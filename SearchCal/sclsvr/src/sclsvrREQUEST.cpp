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
#include "sclsvrErrors.h"

/**
 * Class constructor
 */
sclsvrREQUEST::sclsvrREQUEST()
{
    _getCalCmd         = NULL;
    _maxBaselineLength = 0.0;
    _observingWlen     = 0.0;
    _brightFlag        = mcsTRUE;
    _noScienceObject   = mcsFALSE;
    _fileName[0]       = '\0';
    _outputFormat      = 0.0;
    _diagnose          = mcsFALSE;
    _jsdcMode          = mcsFALSE;
}

/**
 * Class destructor
 */
sclsvrREQUEST::~sclsvrREQUEST()
{
    if (IS_NOT_NULL(_getCalCmd))
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
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT sclsvrREQUEST::Parse(const char *cmdParamLine)
{
    // GETCAL command
    if (IS_NOT_NULL(_getCalCmd))
    {
        delete (_getCalCmd);
    }
    _getCalCmd = new sclsvrGETCAL_CMD(sclsvrGETCAL_CMD_NAME, cmdParamLine);

    // Parse command
    FAIL(_getCalCmd->Parse());

    // Check client's output format
    mcsDOUBLE outputFormat;
    _getCalCmd->GetOutputFormat(&outputFormat);

    /* check that requested output format is compatible with this SearchCal server */
    FAIL_COND_DO(outputFormat < vobsVOTABLE_FORMAT,
                 errAdd(sclsvrERR_UNSUPPORTED_OUTPUT_FORMAT, outputFormat, vobsVOTABLE_FORMAT));

    // Object name
    char* objectName = NULL;
    FAIL(_getCalCmd->GetObjectName(&objectName));

    // Observed magnitude
    mcsDOUBLE magnitude;
    FAIL(_getCalCmd->GetMag(&magnitude));

    // Search area size
    mcsDOUBLE deltaRa, deltaDec, radius;
    mcsLOGICAL circularQueryFlag = _getCalCmd->IsDefinedRadius();

    // If a radius is specified
    if (IS_TRUE(circularQueryFlag))
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
    mcsDOUBLE minRangeMag, maxRangeMag;
    // MinRangeMag
    FAIL(_getCalCmd->GetMinMagRange(&minRangeMag));

    // MaxRangeMag
    FAIL(_getCalCmd->GetMaxMagRange(&maxRangeMag));

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

    // Bright/Faint scenario
    mcsLOGICAL brightFlag = mcsTRUE;
    if (IS_TRUE(_getCalCmd->IsDefinedBright()))
    {
        FAIL(_getCalCmd->GetBright(&brightFlag));
    }

    // Science star
    mcsLOGICAL noScienceStar = mcsTRUE;
    if (IS_TRUE(_getCalCmd->IsDefinedNoScienceStar()))
    {
        FAIL(_getCalCmd->GetNoScienceStar(&noScienceStar));
    }

    // File name
    char* fileName = NULL;
    if (IS_TRUE(_getCalCmd->IsDefinedFile()))
    {
        FAIL(_getCalCmd->GetFile(&fileName));
    }

    // Get diagnose flag:
    mcsLOGICAL diagnose = mcsFALSE;
    FAIL(_getCalCmd->GetDiagnose(&diagnose));


    // Build the request object from the parameters of the command
    // Affect the reference object name
    FAIL(SetObjectName(objectName));

    // Affect the right ascension position
    FAIL(SetObjectRa(ra));

    // Affect the declination position
    FAIL(SetObjectDec(dec));

    // Affect the wavelength
    FAIL(SetObservingWlen(wlen));

    // Affect the magnitude
    FAIL(SetObjectMag(magnitude));

    // Magnitude is not used for N band
    // Affect the min of the magitude range
    FAIL(SetMinMagRange(minRangeMag));

    // Affect the max of the magnitude range
    FAIL(SetMaxMagRange(maxRangeMag));

    // Set the search area size
    if (IS_TRUE(circularQueryFlag))
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

    // Affect the brightness flag
    FAIL(SetBrightFlag(brightFlag));

    // Affect the science star flag
    FAIL(SetNoScienceStar(noScienceStar));

    // Affect the file name
    if (IS_NOT_NULL(fileName))
    {
        FAIL(SetFileName(fileName));
    }

    // Affect the output format
    FAIL(SetOutputFormat(outputFormat));

    // Affect the diagnose flag
    FAIL(SetDiagnose(diagnose));

    return mcsSUCCESS;
}

/**
 * Returns the GETCAL command parameter line.
 *
 * @return command parameter line or NULL if it is not initialized
 */
mcsCOMPL_STAT sclsvrREQUEST::GetCmdParamLine(mcsSTRING16384* cmdParamLine) const
{
    memset(cmdParamLine, '\0', mcsLEN16384);

    // If no parameter has been given, return NULL
    if (IS_NULL(_getCalCmd))
    {
        return mcsSUCCESS;
    }

    // Else build the command parameter line and return it
    string str;
    FAIL(_getCalCmd->GetCmdParamLine(str));

    strncpy((char*)cmdParamLine, str.c_str(), mcsLEN16384 - 1);

    return mcsSUCCESS;
}

/**
 * Set search band (overridden) and update the GETCAL command
 *
 * @param searchBand search band which is a letter (H, M, N, R, V ...).
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT sclsvrREQUEST::SetSearchBand(const char* searchBand)
{
    if (vobsREQUEST::SetSearchBand(searchBand) == mcsSUCCESS)
    {
        if (IS_NULL(_getCalCmd))
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
    return _maxBaselineLength;
}

/**
 * Set the observing wavelength (um).
 *
 * @return Always mcsSUCCESS.
 */
mcsCOMPL_STAT sclsvrREQUEST::SetObservingWlen(mcsDOUBLE wlen)
{
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
    return _observingWlen;
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
    return _noScienceObject;
}

/**
 * Set the file name in which the value will be saved.
 *
 * @param fileName the name of the save file
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT sclsvrREQUEST::SetFileName(const mcsSTRING256 fileName)
{
    FAIL_NULL(strncpy((char*)_fileName, fileName, mcsLEN256));

    return mcsSUCCESS;
}

/**
 * Get the name of the save file
 *
 * @return name of the save file
 */
const char *sclsvrREQUEST::GetFileName(void) const
{
    return _fileName;
}

/**
 * Set the output format (2013.7 for example).
 *
 * @return Always mcsSUCCESS.
 */
mcsCOMPL_STAT sclsvrREQUEST::SetOutputFormat(mcsDOUBLE outputFormat)
{
    _outputFormat = outputFormat;

    return mcsSUCCESS;
}

/**
 * Get the output format (2013.7 for example).
 *
 * @return output format.
 */
mcsDOUBLE sclsvrREQUEST::GetOutputFormat(void) const
{
    return _outputFormat;
}

/**
 * Specify whether the diagnostic mode is enabled (do not filter on diamFlag and add request log in VOTABLE).
 *
 * @param diagnose mcsTRUE if the diagnostic mode is enabled
 * otherwise mcsFALSE
 *
 * @return Always mcsSUCCESS.
 */
mcsCOMPL_STAT sclsvrREQUEST::SetDiagnose(mcsLOGICAL diagnose)
{
    _diagnose = diagnose;

    return mcsSUCCESS;
}

/**
 * Return whether diagnostic mode is enabled.
 *
 * @return mcsTRUE if the diagnostic mode is enabled, otherwise mcsFALSE.
 */
mcsLOGICAL sclsvrREQUEST::IsDiagnose() const
{
    return _diagnose;
}

/**
 * Append a VOTable serailization of the request and its parameters.
 *
 * @param voTable the string in which the PARAMs will be appent
 *
 * @returns an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT sclsvrREQUEST::AppendParamsToVOTable(string& voTable)
{
    return _getCalCmd->AppendParamsToVOTable(voTable);
}

/**
 * Specify whether the JSDC mode is enabled.
 *
 * @param diagnose mcsTRUE if the JSDC mode is enabled
 * otherwise mcsFALSE
 *
 * @return Always mcsSUCCESS.
 */
mcsCOMPL_STAT sclsvrREQUEST::SetJSDCMode(mcsLOGICAL mode)
{
    _jsdcMode = mode;

    return mcsSUCCESS;
}

/**
 * Return whether JSDC mode is enabled.
 *
 * @return mcsTRUE if the JSDC mode is enabled, otherwise mcsFALSE.
 */
mcsLOGICAL sclsvrREQUEST::IsJSDCMode() const
{
    return _jsdcMode;
}

/*___oOo___*/
