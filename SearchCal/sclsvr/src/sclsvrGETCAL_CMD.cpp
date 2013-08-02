/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/
/*
 * 
 * This file has been automatically generated
 * 
 * !!!!!!!!!!!  DO NOT MANUALLY EDIT THIS FILE  !!!!!!!!!!!
 */
/**
 * \file
 * Generated for sclsvrGETCAL_CMD class definition.
 */
 
 
/*
 * System Headers
 */
#include <stdio.h>
#include <iostream>
using namespace std;

/*
 * MCS Headers
 */
#include "log.h"

/*
 * Local Headers
 */
#include "cmd.h"
#include "sclsvrGETCAL_CMD.h"
#include "sclsvrPrivate.h"

/*
 * Class constructor
 */

/**
 * Constructs a new class for an easier access for parameters of the
 * sclsvrGETCAL_CMD COMMAND.
 */
 sclsvrGETCAL_CMD::sclsvrGETCAL_CMD(string name, string params):cmdCOMMAND(name, params,sclsvrGETCAL_CDF_NAME)
{
}

/*
 * Class destructor
 */

/**
 * Class destructor
 */
sclsvrGETCAL_CMD::~sclsvrGETCAL_CMD()
{
}

/*
 * Public methods
 */

/**
 * Get the value of the parameter objectName.
 *
 * \param _objectName_ a pointer where to store the parameter.
 * 
 * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */ 
mcsCOMPL_STAT sclsvrGETCAL_CMD::GetObjectName(char **_objectName_)
{
    return GetParamValue("objectName", _objectName_);
}

/**
 * Get the value of the parameter mag.
 *
 * \param _mag_ a pointer where to store the parameter.
 * 
 * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */ 
mcsCOMPL_STAT sclsvrGETCAL_CMD::GetMag(mcsDOUBLE *_mag_)
{
    return GetParamValue("mag", _mag_);
}

/**
 * Get the value of the parameter diffRa.
 *
 * \param _diffRa_ a pointer where to store the parameter.
 * 
 * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */ 
mcsCOMPL_STAT sclsvrGETCAL_CMD::GetDiffRa(mcsDOUBLE *_diffRa_)
{
    return GetParamValue("diffRa", _diffRa_);
}

/**
* Check if the parameter diffRa has a default value.
 *
 * \return mcsTRUE or mcsFALSE if it has no default value.
 */ 
 mcsLOGICAL sclsvrGETCAL_CMD::HasDefaultDiffRa()
{
    return HasDefaultValue("diffRa");
}

/**
 * Get the default value of the parameter diffRa.
 *
 * \param _diffRa_ a pointer where to store the parameter.
 * 
 * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */ 
mcsCOMPL_STAT sclsvrGETCAL_CMD::GetDefaultDiffRa(mcsDOUBLE *_diffRa_)
{
    return GetDefaultParamValue("diffRa", _diffRa_);
}

/**
 * Get the value of the parameter diffDec.
 *
 * \param _diffDec_ a pointer where to store the parameter.
 * 
 * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */ 
mcsCOMPL_STAT sclsvrGETCAL_CMD::GetDiffDec(mcsDOUBLE *_diffDec_)
{
    return GetParamValue("diffDec", _diffDec_);
}

/**
* Check if the parameter diffDec has a default value.
 *
 * \return mcsTRUE or mcsFALSE if it has no default value.
 */ 
 mcsLOGICAL sclsvrGETCAL_CMD::HasDefaultDiffDec()
{
    return HasDefaultValue("diffDec");
}

/**
 * Get the default value of the parameter diffDec.
 *
 * \param _diffDec_ a pointer where to store the parameter.
 * 
 * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */ 
mcsCOMPL_STAT sclsvrGETCAL_CMD::GetDefaultDiffDec(mcsDOUBLE *_diffDec_)
{
    return GetDefaultParamValue("diffDec", _diffDec_);
}

/**
 * Get the value of the parameter radius.
 *
 * \param _radius_ a pointer where to store the parameter.
 * 
 * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */ 
mcsCOMPL_STAT sclsvrGETCAL_CMD::GetRadius(mcsDOUBLE *_radius_)
{
    return GetParamValue("radius", _radius_);
}

/**
 * Check if the optional parameter radius is defined. 
 * 
 * \return mcsTRUE or mcsFALSE if it is not defined.
 */ 
 mcsLOGICAL sclsvrGETCAL_CMD::IsDefinedRadius()
{
    return IsDefined("radius");
}

/**
 * Get the value of the parameter band.
 *
 * \param _band_ a pointer where to store the parameter.
 * 
 * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */ 
mcsCOMPL_STAT sclsvrGETCAL_CMD::GetBand(char **_band_)
{
    return GetParamValue("band", _band_);
}

/**
* Check if the parameter band has a default value.
 *
 * \return mcsTRUE or mcsFALSE if it has no default value.
 */ 
 mcsLOGICAL sclsvrGETCAL_CMD::HasDefaultBand()
{
    return HasDefaultValue("band");
}

/**
 * Get the default value of the parameter band.
 *
 * \param _band_ a pointer where to store the parameter.
 * 
 * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */ 
mcsCOMPL_STAT sclsvrGETCAL_CMD::GetDefaultBand(char **_band_)
{
    return GetDefaultParamValue("band", _band_);
}

/**
 * Get the value of the parameter minMagRange.
 *
 * \param _minMagRange_ a pointer where to store the parameter.
 * 
 * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */ 
mcsCOMPL_STAT sclsvrGETCAL_CMD::GetMinMagRange(mcsDOUBLE *_minMagRange_)
{
    return GetParamValue("minMagRange", _minMagRange_);
}

/**
 * Check if the optional parameter minMagRange is defined. 
 * 
 * \return mcsTRUE or mcsFALSE if it is not defined.
 */ 
 mcsLOGICAL sclsvrGETCAL_CMD::IsDefinedMinMagRange()
{
    return IsDefined("minMagRange");
}

/**
 * Get the value of the parameter maxMagRange.
 *
 * \param _maxMagRange_ a pointer where to store the parameter.
 * 
 * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */ 
mcsCOMPL_STAT sclsvrGETCAL_CMD::GetMaxMagRange(mcsDOUBLE *_maxMagRange_)
{
    return GetParamValue("maxMagRange", _maxMagRange_);
}

/**
 * Check if the optional parameter maxMagRange is defined. 
 * 
 * \return mcsTRUE or mcsFALSE if it is not defined.
 */ 
 mcsLOGICAL sclsvrGETCAL_CMD::IsDefinedMaxMagRange()
{
    return IsDefined("maxMagRange");
}

/**
 * Get the value of the parameter ra.
 *
 * \param _ra_ a pointer where to store the parameter.
 * 
 * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */ 
mcsCOMPL_STAT sclsvrGETCAL_CMD::GetRa(char **_ra_)
{
    return GetParamValue("ra", _ra_);
}

/**
 * Get the value of the parameter dec.
 *
 * \param _dec_ a pointer where to store the parameter.
 * 
 * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */ 
mcsCOMPL_STAT sclsvrGETCAL_CMD::GetDec(char **_dec_)
{
    return GetParamValue("dec", _dec_);
}

/**
 * Get the value of the parameter baseMax.
 *
 * \param _baseMax_ a pointer where to store the parameter.
 * 
 * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */ 
mcsCOMPL_STAT sclsvrGETCAL_CMD::GetBaseMax(mcsDOUBLE *_baseMax_)
{
    return GetParamValue("baseMax", _baseMax_);
}

/**
* Check if the parameter baseMax has a default value.
 *
 * \return mcsTRUE or mcsFALSE if it has no default value.
 */ 
 mcsLOGICAL sclsvrGETCAL_CMD::HasDefaultBaseMax()
{
    return HasDefaultValue("baseMax");
}

/**
 * Get the default value of the parameter baseMax.
 *
 * \param _baseMax_ a pointer where to store the parameter.
 * 
 * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */ 
mcsCOMPL_STAT sclsvrGETCAL_CMD::GetDefaultBaseMax(mcsDOUBLE *_baseMax_)
{
    return GetDefaultParamValue("baseMax", _baseMax_);
}

/**
 * Get the value of the parameter wlen.
 *
 * \param _wlen_ a pointer where to store the parameter.
 * 
 * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */ 
mcsCOMPL_STAT sclsvrGETCAL_CMD::GetWlen(mcsDOUBLE *_wlen_)
{
    return GetParamValue("wlen", _wlen_);
}

/**
 * Get the value of the parameter file.
 *
 * \param _file_ a pointer where to store the parameter.
 * 
 * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */ 
mcsCOMPL_STAT sclsvrGETCAL_CMD::GetFile(char **_file_)
{
    return GetParamValue("file", _file_);
}

/**
 * Check if the optional parameter file is defined. 
 * 
 * \return mcsTRUE or mcsFALSE if it is not defined.
 */ 
 mcsLOGICAL sclsvrGETCAL_CMD::IsDefinedFile()
{
    return IsDefined("file");
}

/**
 * Get the value of the parameter bright.
 *
 * \param _bright_ a pointer where to store the parameter.
 * 
 * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */ 
mcsCOMPL_STAT sclsvrGETCAL_CMD::GetBright(mcsLOGICAL *_bright_)
{
    return GetParamValue("bright", _bright_);
}

/**
 * Check if the optional parameter bright is defined. 
 * 
 * \return mcsTRUE or mcsFALSE if it is not defined.
 */ 
 mcsLOGICAL sclsvrGETCAL_CMD::IsDefinedBright()
{
    return IsDefined("bright");
}

/**
* Check if the parameter bright has a default value.
 *
 * \return mcsTRUE or mcsFALSE if it has no default value.
 */ 
 mcsLOGICAL sclsvrGETCAL_CMD::HasDefaultBright()
{
    return HasDefaultValue("bright");
}

/**
 * Get the default value of the parameter bright.
 *
 * \param _bright_ a pointer where to store the parameter.
 * 
 * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */ 
mcsCOMPL_STAT sclsvrGETCAL_CMD::GetDefaultBright(mcsLOGICAL *_bright_)
{
    return GetDefaultParamValue("bright", _bright_);
}

/**
 * Get the value of the parameter noScienceStar.
 *
 * \param _noScienceStar_ a pointer where to store the parameter.
 * 
 * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */ 
mcsCOMPL_STAT sclsvrGETCAL_CMD::GetNoScienceStar(mcsLOGICAL *_noScienceStar_)
{
    return GetParamValue("noScienceStar", _noScienceStar_);
}

/**
 * Check if the optional parameter noScienceStar is defined. 
 * 
 * \return mcsTRUE or mcsFALSE if it is not defined.
 */ 
 mcsLOGICAL sclsvrGETCAL_CMD::IsDefinedNoScienceStar()
{
    return IsDefined("noScienceStar");
}

/**
* Check if the parameter noScienceStar has a default value.
 *
 * \return mcsTRUE or mcsFALSE if it has no default value.
 */ 
 mcsLOGICAL sclsvrGETCAL_CMD::HasDefaultNoScienceStar()
{
    return HasDefaultValue("noScienceStar");
}

/**
 * Get the default value of the parameter noScienceStar.
 *
 * \param _noScienceStar_ a pointer where to store the parameter.
 * 
 * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */ 
mcsCOMPL_STAT sclsvrGETCAL_CMD::GetDefaultNoScienceStar(mcsLOGICAL *_noScienceStar_)
{
    return GetDefaultParamValue("noScienceStar", _noScienceStar_);
}

/**
 * Get the value of the parameter outputFormat.
 *
 * \param _outputFormat_ a pointer where to store the parameter.
 * 
 * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */ 
mcsCOMPL_STAT sclsvrGETCAL_CMD::GetOutputFormat(mcsDOUBLE *_outputFormat_)
{
    return GetParamValue("outputFormat", _outputFormat_);
}

/**
 * Check if the optional parameter outputFormat is defined. 
 * 
 * \return mcsTRUE or mcsFALSE if it is not defined.
 */ 
 mcsLOGICAL sclsvrGETCAL_CMD::IsDefinedOutputFormat()
{
    return IsDefined("outputFormat");
}

/**
* Check if the parameter outputFormat has a default value.
 *
 * \return mcsTRUE or mcsFALSE if it has no default value.
 */ 
 mcsLOGICAL sclsvrGETCAL_CMD::HasDefaultOutputFormat()
{
    return HasDefaultValue("outputFormat");
}

/**
 * Get the default value of the parameter outputFormat.
 *
 * \param _outputFormat_ a pointer where to store the parameter.
 * 
 * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */ 
mcsCOMPL_STAT sclsvrGETCAL_CMD::GetDefaultOutputFormat(mcsDOUBLE *_outputFormat_)
{
    return GetDefaultParamValue("outputFormat", _outputFormat_);
}

/**
 * Get the value of the parameter diagnose.
 *
 * \param _diagnose_ a pointer where to store the parameter.
 * 
 * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */ 
mcsCOMPL_STAT sclsvrGETCAL_CMD::GetDiagnose(mcsLOGICAL *_diagnose_)
{
    return GetParamValue("diagnose", _diagnose_);
}

/**
 * Check if the optional parameter diagnose is defined. 
 * 
 * \return mcsTRUE or mcsFALSE if it is not defined.
 */ 
 mcsLOGICAL sclsvrGETCAL_CMD::IsDefinedDiagnose()
{
    return IsDefined("diagnose");
}

/**
* Check if the parameter diagnose has a default value.
 *
 * \return mcsTRUE or mcsFALSE if it has no default value.
 */ 
 mcsLOGICAL sclsvrGETCAL_CMD::HasDefaultDiagnose()
{
    return HasDefaultValue("diagnose");
}

/**
 * Get the default value of the parameter diagnose.
 *
 * \param _diagnose_ a pointer where to store the parameter.
 * 
 * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */ 
mcsCOMPL_STAT sclsvrGETCAL_CMD::GetDefaultDiagnose(mcsLOGICAL *_diagnose_)
{
    return GetDefaultParamValue("diagnose", _diagnose_);
}


/*___oOo___*/
