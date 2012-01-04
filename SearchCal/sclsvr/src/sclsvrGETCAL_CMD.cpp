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
    logExtDbg("sclsvrGETCAL_CMD::GetObjectName()");	
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
    logExtDbg("sclsvrGETCAL_CMD::GetMag()");	
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
    logExtDbg("sclsvrGETCAL_CMD::GetDiffRa()");	
    return GetParamValue("diffRa", _diffRa_);
}

/**
* Check if the parameter diffRa has a default value.
 *
 * \return mcsTRUE or mcsFALSE if it has no default value.
 */ 
 mcsLOGICAL sclsvrGETCAL_CMD::HasDefaultDiffRa()
{
    logExtDbg("sclsvrGETCAL_CMD::HasDefaultDiffRa()");
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
    logExtDbg("sclsvrGETCAL_CMD::GetDefaultDiffRa()");
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
    logExtDbg("sclsvrGETCAL_CMD::GetDiffDec()");	
    return GetParamValue("diffDec", _diffDec_);
}

/**
* Check if the parameter diffDec has a default value.
 *
 * \return mcsTRUE or mcsFALSE if it has no default value.
 */ 
 mcsLOGICAL sclsvrGETCAL_CMD::HasDefaultDiffDec()
{
    logExtDbg("sclsvrGETCAL_CMD::HasDefaultDiffDec()");
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
    logExtDbg("sclsvrGETCAL_CMD::GetDefaultDiffDec()");
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
    logExtDbg("sclsvrGETCAL_CMD::GetRadius()");	
    return GetParamValue("radius", _radius_);
}

/**
 * Check if the optional parameter radius is defined. 
 * 
 * \return mcsTRUE or mcsFALSE if it is not defined.
 */ 
 mcsLOGICAL sclsvrGETCAL_CMD::IsDefinedRadius()
{
    logExtDbg("sclsvrGETCAL_CMD::IsDefinedRadius()");
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
    logExtDbg("sclsvrGETCAL_CMD::GetBand()");	
    return GetParamValue("band", _band_);
}

/**
* Check if the parameter band has a default value.
 *
 * \return mcsTRUE or mcsFALSE if it has no default value.
 */ 
 mcsLOGICAL sclsvrGETCAL_CMD::HasDefaultBand()
{
    logExtDbg("sclsvrGETCAL_CMD::HasDefaultBand()");
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
    logExtDbg("sclsvrGETCAL_CMD::GetDefaultBand()");
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
    logExtDbg("sclsvrGETCAL_CMD::GetMinMagRange()");	
    return GetParamValue("minMagRange", _minMagRange_);
}

/**
 * Check if the optional parameter minMagRange is defined. 
 * 
 * \return mcsTRUE or mcsFALSE if it is not defined.
 */ 
 mcsLOGICAL sclsvrGETCAL_CMD::IsDefinedMinMagRange()
{
    logExtDbg("sclsvrGETCAL_CMD::IsDefinedMinMagRange()");
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
    logExtDbg("sclsvrGETCAL_CMD::GetMaxMagRange()");	
    return GetParamValue("maxMagRange", _maxMagRange_);
}

/**
 * Check if the optional parameter maxMagRange is defined. 
 * 
 * \return mcsTRUE or mcsFALSE if it is not defined.
 */ 
 mcsLOGICAL sclsvrGETCAL_CMD::IsDefinedMaxMagRange()
{
    logExtDbg("sclsvrGETCAL_CMD::IsDefinedMaxMagRange()");
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
    logExtDbg("sclsvrGETCAL_CMD::GetRa()");	
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
    logExtDbg("sclsvrGETCAL_CMD::GetDec()");	
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
    logExtDbg("sclsvrGETCAL_CMD::GetBaseMax()");	
    return GetParamValue("baseMax", _baseMax_);
}

/**
* Check if the parameter baseMax has a default value.
 *
 * \return mcsTRUE or mcsFALSE if it has no default value.
 */ 
 mcsLOGICAL sclsvrGETCAL_CMD::HasDefaultBaseMax()
{
    logExtDbg("sclsvrGETCAL_CMD::HasDefaultBaseMax()");
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
    logExtDbg("sclsvrGETCAL_CMD::GetDefaultBaseMax()");
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
    logExtDbg("sclsvrGETCAL_CMD::GetWlen()");	
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
    logExtDbg("sclsvrGETCAL_CMD::GetFile()");	
    return GetParamValue("file", _file_);
}

/**
 * Check if the optional parameter file is defined. 
 * 
 * \return mcsTRUE or mcsFALSE if it is not defined.
 */ 
 mcsLOGICAL sclsvrGETCAL_CMD::IsDefinedFile()
{
    logExtDbg("sclsvrGETCAL_CMD::IsDefinedFile()");
    return IsDefined("file");
}

/**
 * Get the value of the parameter diamVK.
 *
 * \param _diamVK_ a pointer where to store the parameter.
 * 
 * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */ 
mcsCOMPL_STAT sclsvrGETCAL_CMD::GetDiamVK(mcsDOUBLE *_diamVK_)
{
    logExtDbg("sclsvrGETCAL_CMD::GetDiamVK()");	
    return GetParamValue("diamVK", _diamVK_);
}

/**
 * Check if the optional parameter diamVK is defined. 
 * 
 * \return mcsTRUE or mcsFALSE if it is not defined.
 */ 
 mcsLOGICAL sclsvrGETCAL_CMD::IsDefinedDiamVK()
{
    logExtDbg("sclsvrGETCAL_CMD::IsDefinedDiamVK()");
    return IsDefined("diamVK");
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
    logExtDbg("sclsvrGETCAL_CMD::GetBright()");	
    return GetParamValue("bright", _bright_);
}

/**
 * Check if the optional parameter bright is defined. 
 * 
 * \return mcsTRUE or mcsFALSE if it is not defined.
 */ 
 mcsLOGICAL sclsvrGETCAL_CMD::IsDefinedBright()
{
    logExtDbg("sclsvrGETCAL_CMD::IsDefinedBright()");
    return IsDefined("bright");
}

/**
* Check if the parameter bright has a default value.
 *
 * \return mcsTRUE or mcsFALSE if it has no default value.
 */ 
 mcsLOGICAL sclsvrGETCAL_CMD::HasDefaultBright()
{
    logExtDbg("sclsvrGETCAL_CMD::HasDefaultBright()");
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
    logExtDbg("sclsvrGETCAL_CMD::GetDefaultBright()");
    return GetDefaultParamValue("bright", _bright_);
}

/**
 * Get the value of the parameter visErr.
 *
 * \param _visErr_ a pointer where to store the parameter.
 * 
 * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */ 
mcsCOMPL_STAT sclsvrGETCAL_CMD::GetVisErr(mcsDOUBLE *_visErr_)
{
    logExtDbg("sclsvrGETCAL_CMD::GetVisErr()");	
    return GetParamValue("visErr", _visErr_);
}

/**
 * Check if the optional parameter visErr is defined. 
 * 
 * \return mcsTRUE or mcsFALSE if it is not defined.
 */ 
 mcsLOGICAL sclsvrGETCAL_CMD::IsDefinedVisErr()
{
    logExtDbg("sclsvrGETCAL_CMD::IsDefinedVisErr()");
    return IsDefined("visErr");
}

/**
* Check if the parameter visErr has a default value.
 *
 * \return mcsTRUE or mcsFALSE if it has no default value.
 */ 
 mcsLOGICAL sclsvrGETCAL_CMD::HasDefaultVisErr()
{
    logExtDbg("sclsvrGETCAL_CMD::HasDefaultVisErr()");
    return HasDefaultValue("visErr");
}

/**
 * Get the default value of the parameter visErr.
 *
 * \param _visErr_ a pointer where to store the parameter.
 * 
 * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */ 
mcsCOMPL_STAT sclsvrGETCAL_CMD::GetDefaultVisErr(mcsDOUBLE *_visErr_)
{
    logExtDbg("sclsvrGETCAL_CMD::GetDefaultVisErr()");
    return GetDefaultParamValue("visErr", _visErr_);
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
    logExtDbg("sclsvrGETCAL_CMD::GetNoScienceStar()");	
    return GetParamValue("noScienceStar", _noScienceStar_);
}

/**
 * Check if the optional parameter noScienceStar is defined. 
 * 
 * \return mcsTRUE or mcsFALSE if it is not defined.
 */ 
 mcsLOGICAL sclsvrGETCAL_CMD::IsDefinedNoScienceStar()
{
    logExtDbg("sclsvrGETCAL_CMD::IsDefinedNoScienceStar()");
    return IsDefined("noScienceStar");
}

/**
* Check if the parameter noScienceStar has a default value.
 *
 * \return mcsTRUE or mcsFALSE if it has no default value.
 */ 
 mcsLOGICAL sclsvrGETCAL_CMD::HasDefaultNoScienceStar()
{
    logExtDbg("sclsvrGETCAL_CMD::HasDefaultNoScienceStar()");
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
    logExtDbg("sclsvrGETCAL_CMD::GetDefaultNoScienceStar()");
    return GetDefaultParamValue("noScienceStar", _noScienceStar_);
}


/*___oOo___*/
