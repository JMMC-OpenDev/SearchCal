/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/
/*
 * 
 * This file has been automatically generated
 * 
 * !!!!!!!!!!!  DO NOT MANUALLY EDIT THIS FILE  !!!!!!!!!!!
 */
#ifndef evhVERSION_CMD_H
#define evhVERSION_CMD_H

/**
 * \file
 * Generated for evhVERSION_CMD class declaration.
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
#define evhVERSION_CMD_NAME "VERSION"

/*
 * Command definition file
 */
#define evhVERSION_CDF_NAME "evhVERSION.cdf"

/*
 * Class declaration
 */
        
    
/**
 * This class is intented to be used for a
 * reception of the VERSION command 
 */

class evhVERSION_CMD: public cmdCOMMAND
{
public:
    evhVERSION_CMD(string name, string params);
    virtual ~evhVERSION_CMD();



protected:

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
     evhVERSION_CMD(const evhVERSION_CMD&);
     evhVERSION_CMD& operator=(const evhVERSION_CMD&);

};

#endif /*!evhVERSION_CMD_H*/

/*___oOo___*/
