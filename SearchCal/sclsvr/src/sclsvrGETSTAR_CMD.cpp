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
    logExtDbg("sclsvrGETSTAR_CMD::GetObjectName()");	
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
    logExtDbg("sclsvrGETSTAR_CMD::GetFile()");	
    return GetParamValue("file", _file_);
}

/**
 * Get the value of the parameter baseline.
 *
 * \param _baseline_ a pointer where to store the parameter.
 * 
 * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */ 
mcsCOMPL_STAT sclsvrGETSTAR_CMD::GetBaseline(mcsDOUBLE *_baseline_)
{
    logExtDbg("sclsvrGETSTAR_CMD::GetBaseline()");	
    return GetParamValue("baseline", _baseline_);
}

/**
* Check if the parameter baseline has a default value.
 *
 * \return mcsTRUE or mcsFALSE if it has no default value.
 */ 
 mcsLOGICAL sclsvrGETSTAR_CMD::HasDefaultBaseline()
{
    logExtDbg("sclsvrGETSTAR_CMD::HasDefaultBaseline()");
    return HasDefaultValue("baseline");
}

/**
 * Get the default value of the parameter baseline.
 *
 * \param _baseline_ a pointer where to store the parameter.
 * 
 * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */ 
mcsCOMPL_STAT sclsvrGETSTAR_CMD::GetDefaultBaseline(mcsDOUBLE *_baseline_)
{
    logExtDbg("sclsvrGETSTAR_CMD::GetDefaultBaseline()");
    return GetDefaultParamValue("baseline", _baseline_);
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
    logExtDbg("sclsvrGETSTAR_CMD::GetWlen()");	
    return GetParamValue("wlen", _wlen_);
}

/**
* Check if the parameter wlen has a default value.
 *
 * \return mcsTRUE or mcsFALSE if it has no default value.
 */ 
 mcsLOGICAL sclsvrGETSTAR_CMD::HasDefaultWlen()
{
    logExtDbg("sclsvrGETSTAR_CMD::HasDefaultWlen()");
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
    logExtDbg("sclsvrGETSTAR_CMD::GetDefaultWlen()");
    return GetDefaultParamValue("wlen", _wlen_);
}


/*___oOo___*/
