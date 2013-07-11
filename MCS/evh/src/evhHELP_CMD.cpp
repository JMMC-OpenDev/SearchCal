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
 * Generated for evhHELP_CMD class definition.
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
#include "evhHELP_CMD.h"
#include "evhPrivate.h"

/*
 * Class constructor
 */

/**
 * Constructs a new class for an easier access for parameters of the
 * evhHELP_CMD COMMAND.
 */
 evhHELP_CMD::evhHELP_CMD(string name, string params):cmdCOMMAND(name, params,evhHELP_CDF_NAME)
{
}

/*
 * Class destructor
 */

/**
 * Class destructor
 */
evhHELP_CMD::~evhHELP_CMD()
{
}

/*
 * Public methods
 */

/**
 * Get the value of the parameter command.
 *
 * \param _command_ a pointer where to store the parameter.
 * 
 * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */ 
mcsCOMPL_STAT evhHELP_CMD::GetCommand(char **_command_)
{
    return GetParamValue("command", _command_);
}

/**
 * Check if the optional parameter command is defined. 
 * 
 * \return mcsTRUE or mcsFALSE if it is not defined.
 */ 
 mcsLOGICAL evhHELP_CMD::IsDefinedCommand()
{
    return IsDefined("command");
}


/*___oOo___*/
