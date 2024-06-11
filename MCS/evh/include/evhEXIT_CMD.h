/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/
/*
 * 
 * This file has been automatically generated
 * 
 * !!!!!!!!!!!  DO NOT MANUALLY EDIT THIS FILE  !!!!!!!!!!!
 */
#ifndef evhEXIT_CMD_H
#define evhEXIT_CMD_H

/**
 * \file
 * Generated for evhEXIT_CMD class declaration.
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
#define evhEXIT_CMD_NAME "EXIT"

/*
 * Command definition file
 */
#define evhEXIT_CDF_NAME "evhEXIT.cdf"

/*
 * Class declaration
 */
        
    
/**
 * This class is intented to be used for a
 * reception of the EXIT command 
 */

class evhEXIT_CMD: public cmdCOMMAND
{
public:
    evhEXIT_CMD(string name, string params);
    virtual ~evhEXIT_CMD();



protected:

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
     evhEXIT_CMD(const evhEXIT_CMD&);
     evhEXIT_CMD& operator=(const evhEXIT_CMD&);

};

#endif /*!evhEXIT_CMD_H*/

/*___oOo___*/
