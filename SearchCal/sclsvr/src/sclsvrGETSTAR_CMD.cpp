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
 * Generated for sclsvrGETSTAR_CMD class definition.
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
#include "sclsvrGETSTAR_CMD.h"
#include "sclsvrPrivate.h"

/*
 * Class constructor
 */

/**
 * Constructs a new class for an easier access for parameters of the
 * sclsvrGETSTAR_CMD COMMAND.
 */
 sclsvrGETSTAR_CMD::sclsvrGETSTAR_CMD(string name, string params):cmdCOMMAND(name, params,sclsvrGETSTAR_CDF_NAME)
{
}

/*
 * Class destructor
 */

/**
 * Class destructor
 */
sclsvrGETSTAR_CMD::~sclsvrGETSTAR_CMD()
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
mcsCOMPL_STAT sclsvrGETSTAR_CMD::GetObjectName(char **_objectName_)
{
    return GetParamValue("objectName", _objectName_);
}

/**
 * Get the value of the parameter file.
 *
 * \param _file_ a pointer where to store the parameter.
 * 
 * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */ 
mcsCOMPL_STAT sclsvrGETSTAR_CMD::GetFile(char **_file_)
{
    return GetParamValue("file", _file_);
}

/**
 * Check if the optional parameter file is defined. 
 * 
 * \return mcsTRUE or mcsFALSE if it is not defined.
 */ 
 mcsLOGICAL sclsvrGETSTAR_CMD::IsDefinedFile()
{
    return IsDefined("file");
}

/**
 * Get the value of the parameter baseMax.
 *
 * \param _baseMax_ a pointer where to store the parameter.
 * 
 * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */ 
mcsCOMPL_STAT sclsvrGETSTAR_CMD::GetBaseMax(mcsDOUBLE *_baseMax_)
{
    return GetParamValue("baseMax", _baseMax_);
}

/**
* Check if the parameter baseMax has a default value.
 *
 * \return mcsTRUE or mcsFALSE if it has no default value.
 */ 
 mcsLOGICAL sclsvrGETSTAR_CMD::HasDefaultBaseMax()
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
mcsCOMPL_STAT sclsvrGETSTAR_CMD::GetDefaultBaseMax(mcsDOUBLE *_baseMax_)
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
mcsCOMPL_STAT sclsvrGETSTAR_CMD::GetWlen(mcsDOUBLE *_wlen_)
{
    return GetParamValue("wlen", _wlen_);
}

/**
* Check if the parameter wlen has a default value.
 *
 * \return mcsTRUE or mcsFALSE if it has no default value.
 */ 
 mcsLOGICAL sclsvrGETSTAR_CMD::HasDefaultWlen()
{
    return HasDefaultValue("wlen");
}

/**
 * Get the default value of the parameter wlen.
 *
 * \param _wlen_ a pointer where to store the parameter.
 * 
 * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */ 
mcsCOMPL_STAT sclsvrGETSTAR_CMD::GetDefaultWlen(mcsDOUBLE *_wlen_)
{
    return GetDefaultParamValue("wlen", _wlen_);
}

/**
 * Get the value of the parameter format.
 *
 * \param _format_ a pointer where to store the parameter.
 * 
 * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */ 
mcsCOMPL_STAT sclsvrGETSTAR_CMD::GetFormat(char **_format_)
{
    return GetParamValue("format", _format_);
}

/**
 * Check if the optional parameter format is defined. 
 * 
 * \return mcsTRUE or mcsFALSE if it is not defined.
 */ 
 mcsLOGICAL sclsvrGETSTAR_CMD::IsDefinedFormat()
{
    return IsDefined("format");
}

/**
* Check if the parameter format has a default value.
 *
 * \return mcsTRUE or mcsFALSE if it has no default value.
 */ 
 mcsLOGICAL sclsvrGETSTAR_CMD::HasDefaultFormat()
{
    return HasDefaultValue("format");
}

/**
 * Get the default value of the parameter format.
 *
 * \param _format_ a pointer where to store the parameter.
 * 
 * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */ 
mcsCOMPL_STAT sclsvrGETSTAR_CMD::GetDefaultFormat(char **_format_)
{
    return GetDefaultParamValue("format", _format_);
}

/**
 * Get the value of the parameter diagnose.
 *
 * \param _diagnose_ a pointer where to store the parameter.
 * 
 * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */ 
mcsCOMPL_STAT sclsvrGETSTAR_CMD::GetDiagnose(mcsLOGICAL *_diagnose_)
{
    return GetParamValue("diagnose", _diagnose_);
}

/**
 * Check if the optional parameter diagnose is defined. 
 * 
 * \return mcsTRUE or mcsFALSE if it is not defined.
 */ 
 mcsLOGICAL sclsvrGETSTAR_CMD::IsDefinedDiagnose()
{
    return IsDefined("diagnose");
}

/**
* Check if the parameter diagnose has a default value.
 *
 * \return mcsTRUE or mcsFALSE if it has no default value.
 */ 
 mcsLOGICAL sclsvrGETSTAR_CMD::HasDefaultDiagnose()
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
mcsCOMPL_STAT sclsvrGETSTAR_CMD::GetDefaultDiagnose(mcsLOGICAL *_diagnose_)
{
    return GetDefaultParamValue("diagnose", _diagnose_);
}

/**
 * Get the value of the parameter V.
 *
 * \param _V_ a pointer where to store the parameter.
 * 
 * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */ 
mcsCOMPL_STAT sclsvrGETSTAR_CMD::GetV(mcsDOUBLE *_V_)
{
    return GetParamValue("V", _V_);
}

/**
 * Check if the optional parameter V is defined. 
 * 
 * \return mcsTRUE or mcsFALSE if it is not defined.
 */ 
 mcsLOGICAL sclsvrGETSTAR_CMD::IsDefinedV()
{
    return IsDefined("V");
}

/**
 * Get the value of the parameter e_V.
 *
 * \param _e_V_ a pointer where to store the parameter.
 * 
 * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */ 
mcsCOMPL_STAT sclsvrGETSTAR_CMD::GetE_V(mcsDOUBLE *_e_V_)
{
    return GetParamValue("e_V", _e_V_);
}

/**
 * Check if the optional parameter e_V is defined. 
 * 
 * \return mcsTRUE or mcsFALSE if it is not defined.
 */ 
 mcsLOGICAL sclsvrGETSTAR_CMD::IsDefinedE_V()
{
    return IsDefined("e_V");
}

/**
 * Get the value of the parameter J.
 *
 * \param _J_ a pointer where to store the parameter.
 * 
 * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */ 
mcsCOMPL_STAT sclsvrGETSTAR_CMD::GetJ(mcsDOUBLE *_J_)
{
    return GetParamValue("J", _J_);
}

/**
 * Check if the optional parameter J is defined. 
 * 
 * \return mcsTRUE or mcsFALSE if it is not defined.
 */ 
 mcsLOGICAL sclsvrGETSTAR_CMD::IsDefinedJ()
{
    return IsDefined("J");
}

/**
 * Get the value of the parameter e_J.
 *
 * \param _e_J_ a pointer where to store the parameter.
 * 
 * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */ 
mcsCOMPL_STAT sclsvrGETSTAR_CMD::GetE_J(mcsDOUBLE *_e_J_)
{
    return GetParamValue("e_J", _e_J_);
}

/**
 * Check if the optional parameter e_J is defined. 
 * 
 * \return mcsTRUE or mcsFALSE if it is not defined.
 */ 
 mcsLOGICAL sclsvrGETSTAR_CMD::IsDefinedE_J()
{
    return IsDefined("e_J");
}

/**
 * Get the value of the parameter H.
 *
 * \param _H_ a pointer where to store the parameter.
 * 
 * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */ 
mcsCOMPL_STAT sclsvrGETSTAR_CMD::GetH(mcsDOUBLE *_H_)
{
    return GetParamValue("H", _H_);
}

/**
 * Check if the optional parameter H is defined. 
 * 
 * \return mcsTRUE or mcsFALSE if it is not defined.
 */ 
 mcsLOGICAL sclsvrGETSTAR_CMD::IsDefinedH()
{
    return IsDefined("H");
}

/**
 * Get the value of the parameter e_H.
 *
 * \param _e_H_ a pointer where to store the parameter.
 * 
 * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */ 
mcsCOMPL_STAT sclsvrGETSTAR_CMD::GetE_H(mcsDOUBLE *_e_H_)
{
    return GetParamValue("e_H", _e_H_);
}

/**
 * Check if the optional parameter e_H is defined. 
 * 
 * \return mcsTRUE or mcsFALSE if it is not defined.
 */ 
 mcsLOGICAL sclsvrGETSTAR_CMD::IsDefinedE_H()
{
    return IsDefined("e_H");
}

/**
 * Get the value of the parameter K.
 *
 * \param _K_ a pointer where to store the parameter.
 * 
 * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */ 
mcsCOMPL_STAT sclsvrGETSTAR_CMD::GetK(mcsDOUBLE *_K_)
{
    return GetParamValue("K", _K_);
}

/**
 * Check if the optional parameter K is defined. 
 * 
 * \return mcsTRUE or mcsFALSE if it is not defined.
 */ 
 mcsLOGICAL sclsvrGETSTAR_CMD::IsDefinedK()
{
    return IsDefined("K");
}

/**
 * Get the value of the parameter e_K.
 *
 * \param _e_K_ a pointer where to store the parameter.
 * 
 * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */ 
mcsCOMPL_STAT sclsvrGETSTAR_CMD::GetE_K(mcsDOUBLE *_e_K_)
{
    return GetParamValue("e_K", _e_K_);
}

/**
 * Check if the optional parameter e_K is defined. 
 * 
 * \return mcsTRUE or mcsFALSE if it is not defined.
 */ 
 mcsLOGICAL sclsvrGETSTAR_CMD::IsDefinedE_K()
{
    return IsDefined("e_K");
}

/**
 * Get the value of the parameter SP_TYPE.
 *
 * \param _SP_TYPE_ a pointer where to store the parameter.
 * 
 * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */ 
mcsCOMPL_STAT sclsvrGETSTAR_CMD::GetSP_TYPE(char **_SP_TYPE_)
{
    return GetParamValue("SP_TYPE", _SP_TYPE_);
}

/**
 * Check if the optional parameter SP_TYPE is defined. 
 * 
 * \return mcsTRUE or mcsFALSE if it is not defined.
 */ 
 mcsLOGICAL sclsvrGETSTAR_CMD::IsDefinedSP_TYPE()
{
    return IsDefined("SP_TYPE");
}


/*___oOo___*/
