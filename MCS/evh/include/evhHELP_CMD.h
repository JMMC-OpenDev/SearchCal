/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/
/*
 * 
 * This file has been automatically generated
 * 
 * !!!!!!!!!!!  DO NOT MANUALLY EDIT THIS FILE  !!!!!!!!!!!
 */
#ifndef evhHELP_CMD_H
#define evhHELP_CMD_H

/**
 * \file
 * Generated for evhHELP_CMD class declaration.
 * This file has been automatically generated. If this file is missing in your
 * modArea, just type make all to regenerate.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/*
 * MCS Headers
 */
#include "cmd.h"

/*
 * Command name definition
 */
#define evhHELP_CMD_NAME "HELP"

/*
 * Command definition file
 */
#define evhHELP_CDF_NAME "evhHELP.cdf"

/*
 * Class declaration
 */
        
    
/**
 * This class is intented to be used for a
 * reception of the HELP command 
 */

class evhHELP_CMD: public cmdCOMMAND
{
public:
    evhHELP_CMD(string name, string params);
    virtual ~evhHELP_CMD();


    virtual mcsCOMPL_STAT GetCommand(char **_command_);
    virtual mcsLOGICAL IsDefinedCommand(void);

protected:

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
     evhHELP_CMD(const evhHELP_CMD&);
     evhHELP_CMD& operator=(const evhHELP_CMD&);

};

#endif /*!evhHELP_CMD_H*/

/*___oOo___*/
