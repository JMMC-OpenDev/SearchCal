/*
 * 
 * This file has been automatically generated
 * 
 * !!!!!!!!!!!  DO NOT MANUALLY EDIT THIS FILE  !!!!!!!!!!!
 */
/**
 * \file
 * sclsvrSEARCH_CMD class definition.
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
#include "sclsvrSEARCH_CMD.h"
#include "sclsvrPrivate.h"

/*
 * Class constructor
 */

/**
 * Constructs a new class for an easier access for parameters of the
 * sclsvrSEARCH_CMD COMMAND.
 */
 sclsvrSEARCH_CMD::sclsvrSEARCH_CMD(string name, string params):cmdCOMMAND(name, params)
{
    
}

/*
 * Class destructor
 */

/**
 * Class destructor
 */
sclsvrSEARCH_CMD::~sclsvrSEARCH_CMD()
{

}

/*
 * Public methods
 */
        
 
/**
 * Get the value of the parameter objetName.
 *
 * \param objetName a pointer where to store the parameter.
 * 
 * \return an MCS completion status code (SUCCESS or FAILURE).
 */ 
mcsCOMPL_STAT sclsvrSEARCH_CMD::GetObjetName(char **objetName)
{
    logExtDbg("sclsvrSEARCH_CMD::GetObjetName()");	
    return GetParamValue("objetName",objetName);
}

/**
 * Get the value of the parameter mag.
 *
 * \param mag a pointer where to store the parameter.
 * 
 * \return an MCS completion status code (SUCCESS or FAILURE).
 */ 
mcsCOMPL_STAT sclsvrSEARCH_CMD::GetMag(mcsDOUBLE *mag)
{
    logExtDbg("sclsvrSEARCH_CMD::GetMag()");	
    return GetParamValue("mag",mag);
}

/**
 * Get the value of the parameter maxReturn.
 *
 * \param maxReturn a pointer where to store the parameter.
 * 
 * \return an MCS completion status code (SUCCESS or FAILURE).
 */ 
mcsCOMPL_STAT sclsvrSEARCH_CMD::GetMaxReturn(mcsINT32 *maxReturn)
{
    logExtDbg("sclsvrSEARCH_CMD::GetMaxReturn()");	
    return GetParamValue("maxReturn",maxReturn);
}

/**
* Check if the parameter maxReturn has a default value.
 *
 * \return mcsTRUE or mcsFALSE if it has no default value.
 */ 
 mcsLOGICAL sclsvrSEARCH_CMD::HasDefaultMaxReturn()
{
    logExtDbg("sclsvrSEARCH_CMD::HasDefaultMaxReturn()");
    return HasDefaultValue("maxReturn");
}

/**
 * Get the default value of the parameter maxReturn.
 *
 * \param maxReturn a pointer where to store the parameter.
 * 
 * \return an MCS completion status code (SUCCESS or FAILURE).
 */ 
mcsCOMPL_STAT sclsvrSEARCH_CMD::GetDefaultMaxReturn(mcsINT32 *maxReturn)
{
    logExtDbg("sclsvrSEARCH_CMD::GetDefaultMaxReturn()");
    return GetDefaultParamValue("maxReturn",maxReturn);
}

/**
 * Get the value of the parameter diffRa.
 *
 * \param diffRa a pointer where to store the parameter.
 * 
 * \return an MCS completion status code (SUCCESS or FAILURE).
 */ 
mcsCOMPL_STAT sclsvrSEARCH_CMD::GetDiffRa(mcsINT32 *diffRa)
{
    logExtDbg("sclsvrSEARCH_CMD::GetDiffRa()");	
    return GetParamValue("diffRa",diffRa);
}

/**
* Check if the parameter diffRa has a default value.
 *
 * \return mcsTRUE or mcsFALSE if it has no default value.
 */ 
 mcsLOGICAL sclsvrSEARCH_CMD::HasDefaultDiffRa()
{
    logExtDbg("sclsvrSEARCH_CMD::HasDefaultDiffRa()");
    return HasDefaultValue("diffRa");
}

/**
 * Get the default value of the parameter diffRa.
 *
 * \param diffRa a pointer where to store the parameter.
 * 
 * \return an MCS completion status code (SUCCESS or FAILURE).
 */ 
mcsCOMPL_STAT sclsvrSEARCH_CMD::GetDefaultDiffRa(mcsINT32 *diffRa)
{
    logExtDbg("sclsvrSEARCH_CMD::GetDefaultDiffRa()");
    return GetDefaultParamValue("diffRa",diffRa);
}

/**
 * Get the value of the parameter diffDec.
 *
 * \param diffDec a pointer where to store the parameter.
 * 
 * \return an MCS completion status code (SUCCESS or FAILURE).
 */ 
mcsCOMPL_STAT sclsvrSEARCH_CMD::GetDiffDec(mcsINT32 *diffDec)
{
    logExtDbg("sclsvrSEARCH_CMD::GetDiffDec()");	
    return GetParamValue("diffDec",diffDec);
}

/**
* Check if the parameter diffDec has a default value.
 *
 * \return mcsTRUE or mcsFALSE if it has no default value.
 */ 
 mcsLOGICAL sclsvrSEARCH_CMD::HasDefaultDiffDec()
{
    logExtDbg("sclsvrSEARCH_CMD::HasDefaultDiffDec()");
    return HasDefaultValue("diffDec");
}

/**
 * Get the default value of the parameter diffDec.
 *
 * \param diffDec a pointer where to store the parameter.
 * 
 * \return an MCS completion status code (SUCCESS or FAILURE).
 */ 
mcsCOMPL_STAT sclsvrSEARCH_CMD::GetDefaultDiffDec(mcsINT32 *diffDec)
{
    logExtDbg("sclsvrSEARCH_CMD::GetDefaultDiffDec()");
    return GetDefaultParamValue("diffDec",diffDec);
}

/**
 * Get the value of the parameter band.
 *
 * \param band a pointer where to store the parameter.
 * 
 * \return an MCS completion status code (SUCCESS or FAILURE).
 */ 
mcsCOMPL_STAT sclsvrSEARCH_CMD::GetBand(char **band)
{
    logExtDbg("sclsvrSEARCH_CMD::GetBand()");	
    return GetParamValue("band",band);
}

/**
* Check if the parameter band has a default value.
 *
 * \return mcsTRUE or mcsFALSE if it has no default value.
 */ 
 mcsLOGICAL sclsvrSEARCH_CMD::HasDefaultBand()
{
    logExtDbg("sclsvrSEARCH_CMD::HasDefaultBand()");
    return HasDefaultValue("band");
}

/**
 * Get the default value of the parameter band.
 *
 * \param band a pointer where to store the parameter.
 * 
 * \return an MCS completion status code (SUCCESS or FAILURE).
 */ 
mcsCOMPL_STAT sclsvrSEARCH_CMD::GetDefaultBand(char **band)
{
    logExtDbg("sclsvrSEARCH_CMD::GetDefaultBand()");
    return GetDefaultParamValue("band",band);
}

/**
 * Get the value of the parameter minMagRange.
 *
 * \param minMagRange a pointer where to store the parameter.
 * 
 * \return an MCS completion status code (SUCCESS or FAILURE).
 */ 
mcsCOMPL_STAT sclsvrSEARCH_CMD::GetMinMagRange(char **minMagRange)
{
    logExtDbg("sclsvrSEARCH_CMD::GetMinMagRange()");	
    return GetParamValue("minMagRange",minMagRange);
}

/**
 * Get the value of the parameter maxMagRange.
 *
 * \param maxMagRange a pointer where to store the parameter.
 * 
 * \return an MCS completion status code (SUCCESS or FAILURE).
 */ 
mcsCOMPL_STAT sclsvrSEARCH_CMD::GetMaxMagRange(char **maxMagRange)
{
    logExtDbg("sclsvrSEARCH_CMD::GetMaxMagRange()");	
    return GetParamValue("maxMagRange",maxMagRange);
}

/**
 * Get the value of the parameter ra.
 *
 * \param ra a pointer where to store the parameter.
 * 
 * \return an MCS completion status code (SUCCESS or FAILURE).
 */ 
mcsCOMPL_STAT sclsvrSEARCH_CMD::GetRa(char **ra)
{
    logExtDbg("sclsvrSEARCH_CMD::GetRa()");	
    return GetParamValue("ra",ra);
}

/**
* Check if the parameter ra has a default value.
 *
 * \return mcsTRUE or mcsFALSE if it has no default value.
 */ 
 mcsLOGICAL sclsvrSEARCH_CMD::HasDefaultRa()
{
    logExtDbg("sclsvrSEARCH_CMD::HasDefaultRa()");
    return HasDefaultValue("ra");
}

/**
 * Get the default value of the parameter ra.
 *
 * \param ra a pointer where to store the parameter.
 * 
 * \return an MCS completion status code (SUCCESS or FAILURE).
 */ 
mcsCOMPL_STAT sclsvrSEARCH_CMD::GetDefaultRa(char **ra)
{
    logExtDbg("sclsvrSEARCH_CMD::GetDefaultRa()");
    return GetDefaultParamValue("ra",ra);
}

/**
 * Get the value of the parameter dec.
 *
 * \param dec a pointer where to store the parameter.
 * 
 * \return an MCS completion status code (SUCCESS or FAILURE).
 */ 
mcsCOMPL_STAT sclsvrSEARCH_CMD::GetDec(char **dec)
{
    logExtDbg("sclsvrSEARCH_CMD::GetDec()");	
    return GetParamValue("dec",dec);
}

/**
* Check if the parameter dec has a default value.
 *
 * \return mcsTRUE or mcsFALSE if it has no default value.
 */ 
 mcsLOGICAL sclsvrSEARCH_CMD::HasDefaultDec()
{
    logExtDbg("sclsvrSEARCH_CMD::HasDefaultDec()");
    return HasDefaultValue("dec");
}

/**
 * Get the default value of the parameter dec.
 *
 * \param dec a pointer where to store the parameter.
 * 
 * \return an MCS completion status code (SUCCESS or FAILURE).
 */ 
mcsCOMPL_STAT sclsvrSEARCH_CMD::GetDefaultDec(char **dec)
{
    logExtDbg("sclsvrSEARCH_CMD::GetDefaultDec()");
    return GetDefaultParamValue("dec",dec);
}

/**
 * Get the value of the parameter baseMin.
 *
 * \param baseMin a pointer where to store the parameter.
 * 
 * \return an MCS completion status code (SUCCESS or FAILURE).
 */ 
mcsCOMPL_STAT sclsvrSEARCH_CMD::GetBaseMin(mcsDOUBLE *baseMin)
{
    logExtDbg("sclsvrSEARCH_CMD::GetBaseMin()");	
    return GetParamValue("baseMin",baseMin);
}

/**
* Check if the parameter baseMin has a default value.
 *
 * \return mcsTRUE or mcsFALSE if it has no default value.
 */ 
 mcsLOGICAL sclsvrSEARCH_CMD::HasDefaultBaseMin()
{
    logExtDbg("sclsvrSEARCH_CMD::HasDefaultBaseMin()");
    return HasDefaultValue("baseMin");
}

/**
 * Get the default value of the parameter baseMin.
 *
 * \param baseMin a pointer where to store the parameter.
 * 
 * \return an MCS completion status code (SUCCESS or FAILURE).
 */ 
mcsCOMPL_STAT sclsvrSEARCH_CMD::GetDefaultBaseMin(mcsDOUBLE *baseMin)
{
    logExtDbg("sclsvrSEARCH_CMD::GetDefaultBaseMin()");
    return GetDefaultParamValue("baseMin",baseMin);
}

/**
 * Get the value of the parameter baseMax.
 *
 * \param baseMax a pointer where to store the parameter.
 * 
 * \return an MCS completion status code (SUCCESS or FAILURE).
 */ 
mcsCOMPL_STAT sclsvrSEARCH_CMD::GetBaseMax(mcsDOUBLE *baseMax)
{
    logExtDbg("sclsvrSEARCH_CMD::GetBaseMax()");	
    return GetParamValue("baseMax",baseMax);
}

/**
* Check if the parameter baseMax has a default value.
 *
 * \return mcsTRUE or mcsFALSE if it has no default value.
 */ 
 mcsLOGICAL sclsvrSEARCH_CMD::HasDefaultBaseMax()
{
    logExtDbg("sclsvrSEARCH_CMD::HasDefaultBaseMax()");
    return HasDefaultValue("baseMax");
}

/**
 * Get the default value of the parameter baseMax.
 *
 * \param baseMax a pointer where to store the parameter.
 * 
 * \return an MCS completion status code (SUCCESS or FAILURE).
 */ 
mcsCOMPL_STAT sclsvrSEARCH_CMD::GetDefaultBaseMax(mcsDOUBLE *baseMax)
{
    logExtDbg("sclsvrSEARCH_CMD::GetDefaultBaseMax()");
    return GetDefaultParamValue("baseMax",baseMax);
}

/**
 * Get the value of the parameter lambda.
 *
 * \param lambda a pointer where to store the parameter.
 * 
 * \return an MCS completion status code (SUCCESS or FAILURE).
 */ 
mcsCOMPL_STAT sclsvrSEARCH_CMD::GetLambda(mcsDOUBLE *lambda)
{
    logExtDbg("sclsvrSEARCH_CMD::GetLambda()");	
    return GetParamValue("lambda",lambda);
}

/**
 * Get the value of the parameter vis.
 *
 * \param vis a pointer where to store the parameter.
 * 
 * \return an MCS completion status code (SUCCESS or FAILURE).
 */ 
mcsCOMPL_STAT sclsvrSEARCH_CMD::GetVis(mcsDOUBLE *vis)
{
    logExtDbg("sclsvrSEARCH_CMD::GetVis()");	
    return GetParamValue("vis",vis);
}

/**
* Check if the parameter vis has a default value.
 *
 * \return mcsTRUE or mcsFALSE if it has no default value.
 */ 
 mcsLOGICAL sclsvrSEARCH_CMD::HasDefaultVis()
{
    logExtDbg("sclsvrSEARCH_CMD::HasDefaultVis()");
    return HasDefaultValue("vis");
}

/**
 * Get the default value of the parameter vis.
 *
 * \param vis a pointer where to store the parameter.
 * 
 * \return an MCS completion status code (SUCCESS or FAILURE).
 */ 
mcsCOMPL_STAT sclsvrSEARCH_CMD::GetDefaultVis(mcsDOUBLE *vis)
{
    logExtDbg("sclsvrSEARCH_CMD::GetDefaultVis()");
    return GetDefaultParamValue("vis",vis);
}

/**
 * Get the value of the parameter visErr.
 *
 * \param visErr a pointer where to store the parameter.
 * 
 * \return an MCS completion status code (SUCCESS or FAILURE).
 */ 
mcsCOMPL_STAT sclsvrSEARCH_CMD::GetVisErr(mcsDOUBLE *visErr)
{
    logExtDbg("sclsvrSEARCH_CMD::GetVisErr()");	
    return GetParamValue("visErr",visErr);
}

/**
* Check if the parameter visErr has a default value.
 *
 * \return mcsTRUE or mcsFALSE if it has no default value.
 */ 
 mcsLOGICAL sclsvrSEARCH_CMD::HasDefaultVisErr()
{
    logExtDbg("sclsvrSEARCH_CMD::HasDefaultVisErr()");
    return HasDefaultValue("visErr");
}

/**
 * Get the default value of the parameter visErr.
 *
 * \param visErr a pointer where to store the parameter.
 * 
 * \return an MCS completion status code (SUCCESS or FAILURE).
 */ 
mcsCOMPL_STAT sclsvrSEARCH_CMD::GetDefaultVisErr(mcsDOUBLE *visErr)
{
    logExtDbg("sclsvrSEARCH_CMD::GetDefaultVisErr()");
    return GetDefaultParamValue("visErr",visErr);
}


/*___oOo___*/
