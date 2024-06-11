/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/
/*
 * 
 * This file has been automatically generated
 * 
 * !!!!!!!!!!!  DO NOT MANUALLY EDIT THIS FILE  !!!!!!!!!!!
 */
#ifndef evhSTATE_CMD_H
#define evhSTATE_CMD_H

/**
 * \file
 * Generated for evhSTATE_CMD class declaration.
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
#define evhSTATE_CMD_NAME "STATE"

/*
 * Command definition file
 */
#define evhSTATE_CDF_NAME "evhSTATE.cdf"

/*
 * Class declaration
 */
        
    
/**
 * This class is intented to be used for a
 * reception of the STATE command 
 */

class evhSTATE_CMD: public cmdCOMMAND
{
public:
    evhSTATE_CMD(string name, string params);
    virtual ~evhSTATE_CMD();



protected:

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
     evhSTATE_CMD(const evhSTATE_CMD&);
     evhSTATE_CMD& operator=(const evhSTATE_CMD&);

};

#endif /*!evhSTATE_CMD_H*/

/*___oOo___*/
