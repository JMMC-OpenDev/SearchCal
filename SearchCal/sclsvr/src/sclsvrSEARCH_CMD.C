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
 sclsvrSEARCH_CMD::sclsvrSEARCH_CMD(string name, string params):cmdCMD(name, params)
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
mcsCOMPL_STAT sclsvrSEARCH_CMD::get_objetName(char * *objetName)
{
    logExtDbg("sclsvrSEARCH_CMD::get_objetName()");	
    return getParamValue("objetName",objetName);
}

/**
 * Get the value of the parameter mag.
 *
 * \param mag a pointer where to store the parameter.
 * 
 * \return an MCS completion status code (SUCCESS or FAILURE).
 */ 
mcsCOMPL_STAT sclsvrSEARCH_CMD::get_mag(mcsDOUBLE  *mag)
{
    logExtDbg("sclsvrSEARCH_CMD::get_mag()");	
    return getParamValue("mag",mag);
}

/**
 * Get the value of the parameter maxReturn.
 *
 * \param maxReturn a pointer where to store the parameter.
 * 
 * \return an MCS completion status code (SUCCESS or FAILURE).
 */ 
mcsCOMPL_STAT sclsvrSEARCH_CMD::get_maxReturn(mcsINT32  *maxReturn)
{
    logExtDbg("sclsvrSEARCH_CMD::get_maxReturn()");	
    return getParamValue("maxReturn",maxReturn);
}

/**
* Check if the parameter maxReturn has a default value.
 *
 * \return mcsTRUE or mcsFALSE if it has no default value.
 */ 
mcsLOGICAL sclsvrSEARCH_CMD::hasDefault_maxReturn()
{
    logExtDbg("sclsvrSEARCH_CMD::hasDefault_maxReturn()");
    return hasDefaultValue("maxReturn");
}

/**
 * Get the default value of the parameter maxReturn.
 *
 * \param maxReturn a pointer where to store the parameter.
 * 
 * \return an MCS completion status code (SUCCESS or FAILURE).
 */ 
mcsCOMPL_STAT sclsvrSEARCH_CMD::getDefault_maxReturn(mcsINT32  *maxReturn)
{
    logExtDbg("sclsvrSEARCH_CMD::getDefault_maxReturn()");
    return getDefaultParamValue("maxReturn",maxReturn);
}

/**
 * Get the value of the parameter diffRa.
 *
 * \param diffRa a pointer where to store the parameter.
 * 
 * \return an MCS completion status code (SUCCESS or FAILURE).
 */ 
mcsCOMPL_STAT sclsvrSEARCH_CMD::get_diffRa(mcsINT32  *diffRa)
{
    logExtDbg("sclsvrSEARCH_CMD::get_diffRa()");	
    return getParamValue("diffRa",diffRa);
}

/**
* Check if the parameter diffRa has a default value.
 *
 * \return mcsTRUE or mcsFALSE if it has no default value.
 */ 
mcsLOGICAL sclsvrSEARCH_CMD::hasDefault_diffRa()
{
    logExtDbg("sclsvrSEARCH_CMD::hasDefault_diffRa()");
    return hasDefaultValue("diffRa");
}

/**
 * Get the default value of the parameter diffRa.
 *
 * \param diffRa a pointer where to store the parameter.
 * 
 * \return an MCS completion status code (SUCCESS or FAILURE).
 */ 
mcsCOMPL_STAT sclsvrSEARCH_CMD::getDefault_diffRa(mcsINT32  *diffRa)
{
    logExtDbg("sclsvrSEARCH_CMD::getDefault_diffRa()");
    return getDefaultParamValue("diffRa",diffRa);
}

/**
 * Get the value of the parameter diffDec.
 *
 * \param diffDec a pointer where to store the parameter.
 * 
 * \return an MCS completion status code (SUCCESS or FAILURE).
 */ 
mcsCOMPL_STAT sclsvrSEARCH_CMD::get_diffDec(mcsINT32  *diffDec)
{
    logExtDbg("sclsvrSEARCH_CMD::get_diffDec()");	
    return getParamValue("diffDec",diffDec);
}

/**
* Check if the parameter diffDec has a default value.
 *
 * \return mcsTRUE or mcsFALSE if it has no default value.
 */ 
mcsLOGICAL sclsvrSEARCH_CMD::hasDefault_diffDec()
{
    logExtDbg("sclsvrSEARCH_CMD::hasDefault_diffDec()");
    return hasDefaultValue("diffDec");
}

/**
 * Get the default value of the parameter diffDec.
 *
 * \param diffDec a pointer where to store the parameter.
 * 
 * \return an MCS completion status code (SUCCESS or FAILURE).
 */ 
mcsCOMPL_STAT sclsvrSEARCH_CMD::getDefault_diffDec(mcsINT32  *diffDec)
{
    logExtDbg("sclsvrSEARCH_CMD::getDefault_diffDec()");
    return getDefaultParamValue("diffDec",diffDec);
}

/**
 * Get the value of the parameter band.
 *
 * \param band a pointer where to store the parameter.
 * 
 * \return an MCS completion status code (SUCCESS or FAILURE).
 */ 
mcsCOMPL_STAT sclsvrSEARCH_CMD::get_band(char * *band)
{
    logExtDbg("sclsvrSEARCH_CMD::get_band()");	
    return getParamValue("band",band);
}

/**
* Check if the parameter band has a default value.
 *
 * \return mcsTRUE or mcsFALSE if it has no default value.
 */ 
mcsLOGICAL sclsvrSEARCH_CMD::hasDefault_band()
{
    logExtDbg("sclsvrSEARCH_CMD::hasDefault_band()");
    return hasDefaultValue("band");
}

/**
 * Get the default value of the parameter band.
 *
 * \param band a pointer where to store the parameter.
 * 
 * \return an MCS completion status code (SUCCESS or FAILURE).
 */ 
mcsCOMPL_STAT sclsvrSEARCH_CMD::getDefault_band(char * *band)
{
    logExtDbg("sclsvrSEARCH_CMD::getDefault_band()");
    return getDefaultParamValue("band",band);
}

/**
 * Get the value of the parameter minMagRange.
 *
 * \param minMagRange a pointer where to store the parameter.
 * 
 * \return an MCS completion status code (SUCCESS or FAILURE).
 */ 
mcsCOMPL_STAT sclsvrSEARCH_CMD::get_minMagRange(char * *minMagRange)
{
    logExtDbg("sclsvrSEARCH_CMD::get_minMagRange()");	
    return getParamValue("minMagRange",minMagRange);
}

/**
 * Get the value of the parameter maxMagRange.
 *
 * \param maxMagRange a pointer where to store the parameter.
 * 
 * \return an MCS completion status code (SUCCESS or FAILURE).
 */ 
mcsCOMPL_STAT sclsvrSEARCH_CMD::get_maxMagRange(char * *maxMagRange)
{
    logExtDbg("sclsvrSEARCH_CMD::get_maxMagRange()");	
    return getParamValue("maxMagRange",maxMagRange);
}

/**
 * Get the value of the parameter ra.
 *
 * \param ra a pointer where to store the parameter.
 * 
 * \return an MCS completion status code (SUCCESS or FAILURE).
 */ 
mcsCOMPL_STAT sclsvrSEARCH_CMD::get_ra(char * *ra)
{
    logExtDbg("sclsvrSEARCH_CMD::get_ra()");	
    return getParamValue("ra",ra);
}

/**
* Check if the parameter ra has a default value.
 *
 * \return mcsTRUE or mcsFALSE if it has no default value.
 */ 
mcsLOGICAL sclsvrSEARCH_CMD::hasDefault_ra()
{
    logExtDbg("sclsvrSEARCH_CMD::hasDefault_ra()");
    return hasDefaultValue("ra");
}

/**
 * Get the default value of the parameter ra.
 *
 * \param ra a pointer where to store the parameter.
 * 
 * \return an MCS completion status code (SUCCESS or FAILURE).
 */ 
mcsCOMPL_STAT sclsvrSEARCH_CMD::getDefault_ra(char * *ra)
{
    logExtDbg("sclsvrSEARCH_CMD::getDefault_ra()");
    return getDefaultParamValue("ra",ra);
}

/**
 * Get the value of the parameter dec.
 *
 * \param dec a pointer where to store the parameter.
 * 
 * \return an MCS completion status code (SUCCESS or FAILURE).
 */ 
mcsCOMPL_STAT sclsvrSEARCH_CMD::get_dec(char * *dec)
{
    logExtDbg("sclsvrSEARCH_CMD::get_dec()");	
    return getParamValue("dec",dec);
}

/**
* Check if the parameter dec has a default value.
 *
 * \return mcsTRUE or mcsFALSE if it has no default value.
 */ 
mcsLOGICAL sclsvrSEARCH_CMD::hasDefault_dec()
{
    logExtDbg("sclsvrSEARCH_CMD::hasDefault_dec()");
    return hasDefaultValue("dec");
}

/**
 * Get the default value of the parameter dec.
 *
 * \param dec a pointer where to store the parameter.
 * 
 * \return an MCS completion status code (SUCCESS or FAILURE).
 */ 
mcsCOMPL_STAT sclsvrSEARCH_CMD::getDefault_dec(char * *dec)
{
    logExtDbg("sclsvrSEARCH_CMD::getDefault_dec()");
    return getDefaultParamValue("dec",dec);
}

/**
 * Get the value of the parameter baseMin.
 *
 * \param baseMin a pointer where to store the parameter.
 * 
 * \return an MCS completion status code (SUCCESS or FAILURE).
 */ 
mcsCOMPL_STAT sclsvrSEARCH_CMD::get_baseMin(mcsDOUBLE  *baseMin)
{
    logExtDbg("sclsvrSEARCH_CMD::get_baseMin()");	
    return getParamValue("baseMin",baseMin);
}

/**
* Check if the parameter baseMin has a default value.
 *
 * \return mcsTRUE or mcsFALSE if it has no default value.
 */ 
mcsLOGICAL sclsvrSEARCH_CMD::hasDefault_baseMin()
{
    logExtDbg("sclsvrSEARCH_CMD::hasDefault_baseMin()");
    return hasDefaultValue("baseMin");
}

/**
 * Get the default value of the parameter baseMin.
 *
 * \param baseMin a pointer where to store the parameter.
 * 
 * \return an MCS completion status code (SUCCESS or FAILURE).
 */ 
mcsCOMPL_STAT sclsvrSEARCH_CMD::getDefault_baseMin(mcsDOUBLE  *baseMin)
{
    logExtDbg("sclsvrSEARCH_CMD::getDefault_baseMin()");
    return getDefaultParamValue("baseMin",baseMin);
}

/**
 * Get the value of the parameter baseMax.
 *
 * \param baseMax a pointer where to store the parameter.
 * 
 * \return an MCS completion status code (SUCCESS or FAILURE).
 */ 
mcsCOMPL_STAT sclsvrSEARCH_CMD::get_baseMax(mcsDOUBLE  *baseMax)
{
    logExtDbg("sclsvrSEARCH_CMD::get_baseMax()");	
    return getParamValue("baseMax",baseMax);
}

/**
* Check if the parameter baseMax has a default value.
 *
 * \return mcsTRUE or mcsFALSE if it has no default value.
 */ 
mcsLOGICAL sclsvrSEARCH_CMD::hasDefault_baseMax()
{
    logExtDbg("sclsvrSEARCH_CMD::hasDefault_baseMax()");
    return hasDefaultValue("baseMax");
}

/**
 * Get the default value of the parameter baseMax.
 *
 * \param baseMax a pointer where to store the parameter.
 * 
 * \return an MCS completion status code (SUCCESS or FAILURE).
 */ 
mcsCOMPL_STAT sclsvrSEARCH_CMD::getDefault_baseMax(mcsDOUBLE  *baseMax)
{
    logExtDbg("sclsvrSEARCH_CMD::getDefault_baseMax()");
    return getDefaultParamValue("baseMax",baseMax);
}

/**
 * Get the value of the parameter lambda.
 *
 * \param lambda a pointer where to store the parameter.
 * 
 * \return an MCS completion status code (SUCCESS or FAILURE).
 */ 
mcsCOMPL_STAT sclsvrSEARCH_CMD::get_lambda(mcsDOUBLE  *lambda)
{
    logExtDbg("sclsvrSEARCH_CMD::get_lambda()");	
    return getParamValue("lambda",lambda);
}

/**
 * Get the value of the parameter vis.
 *
 * \param vis a pointer where to store the parameter.
 * 
 * \return an MCS completion status code (SUCCESS or FAILURE).
 */ 
mcsCOMPL_STAT sclsvrSEARCH_CMD::get_vis(mcsDOUBLE  *vis)
{
    logExtDbg("sclsvrSEARCH_CMD::get_vis()");	
    return getParamValue("vis",vis);
}

/**
* Check if the parameter vis has a default value.
 *
 * \return mcsTRUE or mcsFALSE if it has no default value.
 */ 
mcsLOGICAL sclsvrSEARCH_CMD::hasDefault_vis()
{
    logExtDbg("sclsvrSEARCH_CMD::hasDefault_vis()");
    return hasDefaultValue("vis");
}

/**
 * Get the default value of the parameter vis.
 *
 * \param vis a pointer where to store the parameter.
 * 
 * \return an MCS completion status code (SUCCESS or FAILURE).
 */ 
mcsCOMPL_STAT sclsvrSEARCH_CMD::getDefault_vis(mcsDOUBLE  *vis)
{
    logExtDbg("sclsvrSEARCH_CMD::getDefault_vis()");
    return getDefaultParamValue("vis",vis);
}

/**
 * Get the value of the parameter visErr.
 *
 * \param visErr a pointer where to store the parameter.
 * 
 * \return an MCS completion status code (SUCCESS or FAILURE).
 */ 
mcsCOMPL_STAT sclsvrSEARCH_CMD::get_visErr(mcsDOUBLE  *visErr)
{
    logExtDbg("sclsvrSEARCH_CMD::get_visErr()");	
    return getParamValue("visErr",visErr);
}

/**
* Check if the parameter visErr has a default value.
 *
 * \return mcsTRUE or mcsFALSE if it has no default value.
 */ 
mcsLOGICAL sclsvrSEARCH_CMD::hasDefault_visErr()
{
    logExtDbg("sclsvrSEARCH_CMD::hasDefault_visErr()");
    return hasDefaultValue("visErr");
}

/**
 * Get the default value of the parameter visErr.
 *
 * \param visErr a pointer where to store the parameter.
 * 
 * \return an MCS completion status code (SUCCESS or FAILURE).
 */ 
mcsCOMPL_STAT sclsvrSEARCH_CMD::getDefault_visErr(mcsDOUBLE  *visErr)
{
    logExtDbg("sclsvrSEARCH_CMD::getDefault_visErr()");
    return getDefaultParamValue("visErr",visErr);
}


/*___oOo___*/
