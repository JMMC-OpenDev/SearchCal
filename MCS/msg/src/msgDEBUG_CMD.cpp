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
 * Generated for msgDEBUG_CMD class definition.
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
#include "msgDEBUG_CMD.h"
#include "msgPrivate.h"

/*
 * Class constructor
 */

/**
 * Constructs a new class for an easier access for parameters of the
 * msgDEBUG_CMD COMMAND.
 */
 msgDEBUG_CMD::msgDEBUG_CMD(string name, string params):cmdCOMMAND(name, params,msgDEBUG_CDF_NAME)
{
}

/*
 * Class destructor
 */

/**
 * Class destructor
 */
msgDEBUG_CMD::~msgDEBUG_CMD()
{
}

/*
 * Public methods
 */

/**
 * Get the value of the parameter stdoutLevel.
 *
 * \param _stdoutLevel_ a pointer where to store the parameter.
 * 
 * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */ 
mcsCOMPL_STAT msgDEBUG_CMD::GetStdoutLevel(mcsINT32 *_stdoutLevel_)
{
    return GetParamValue("stdoutLevel", _stdoutLevel_);
}

/**
 * Check if the optional parameter stdoutLevel is defined. 
 * 
 * \return mcsTRUE or mcsFALSE if it is not defined.
 */ 
 mcsLOGICAL msgDEBUG_CMD::IsDefinedStdoutLevel()
{
    return IsDefined("stdoutLevel");
}

/**
 * Get the value of the parameter logfileLevel.
 *
 * \param _logfileLevel_ a pointer where to store the parameter.
 * 
 * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */ 
mcsCOMPL_STAT msgDEBUG_CMD::GetLogfileLevel(mcsINT32 *_logfileLevel_)
{
    return GetParamValue("logfileLevel", _logfileLevel_);
}

/**
 * Check if the optional parameter logfileLevel is defined. 
 * 
 * \return mcsTRUE or mcsFALSE if it is not defined.
 */ 
 mcsLOGICAL msgDEBUG_CMD::IsDefinedLogfileLevel()
{
    return IsDefined("logfileLevel");
}

/**
 * Get the value of the parameter printDate.
 *
 * \param _printDate_ a pointer where to store the parameter.
 * 
 * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */ 
mcsCOMPL_STAT msgDEBUG_CMD::GetPrintDate(mcsLOGICAL *_printDate_)
{
    return GetParamValue("printDate", _printDate_);
}

/**
 * Check if the optional parameter printDate is defined. 
 * 
 * \return mcsTRUE or mcsFALSE if it is not defined.
 */ 
 mcsLOGICAL msgDEBUG_CMD::IsDefinedPrintDate()
{
    return IsDefined("printDate");
}

/**
 * Get the value of the parameter printFileLine.
 *
 * \param _printFileLine_ a pointer where to store the parameter.
 * 
 * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */ 
mcsCOMPL_STAT msgDEBUG_CMD::GetPrintFileLine(mcsLOGICAL *_printFileLine_)
{
    return GetParamValue("printFileLine", _printFileLine_);
}

/**
 * Check if the optional parameter printFileLine is defined. 
 * 
 * \return mcsTRUE or mcsFALSE if it is not defined.
 */ 
 mcsLOGICAL msgDEBUG_CMD::IsDefinedPrintFileLine()
{
    return IsDefined("printFileLine");
}


/*___oOo___*/
