/*
 * 
 * This file has been automatically generated
 * 
 * !!!!!!!!!!!  DO NOT MANUALLY EDIT THIS FILE  !!!!!!!!!!!
 */
/**
 * \file
 * sclsvrGETSTAR_CMD class definition.
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
 sclsvrGETSTAR_CMD::sclsvrGETSTAR_CMD(string name, string params):cmdCMD(name, params)
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
 * \param objectName a pointer where to store the parameter.
 * 
 * \return an MCS completion status code (SUCCESS or FAILURE).
 */ 
mcsCOMPL_STAT sclsvrGETSTAR_CMD::getObjectName(char **objectName)
{
    logExtDbg("sclsvrGETSTAR_CMD::getObjectName()");	
    return getParamValue("objectName",objectName);
}


/*___oOo___*/
