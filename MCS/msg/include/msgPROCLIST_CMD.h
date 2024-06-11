/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/
/*
 * 
 * This file has been automatically generated
 * 
 * !!!!!!!!!!!  DO NOT MANUALLY EDIT THIS FILE  !!!!!!!!!!!
 */
#ifndef msgPROCLIST_CMD_H
#define msgPROCLIST_CMD_H

/**
 * \file
 * Generated for msgPROCLIST_CMD class declaration.
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
#define msgPROCLIST_CMD_NAME "PROCLIST"

/*
 * Command definition file
 */
#define msgPROCLIST_CDF_NAME "msgPROCLIST.cdf"

/*
 * Class declaration
 */
        
    
/**
 * This class is intented to be used for a
 * reception of the PROCLIST command 
 */

class msgPROCLIST_CMD: public cmdCOMMAND
{
public:
    msgPROCLIST_CMD(string name, string params);
    virtual ~msgPROCLIST_CMD();



protected:

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
     msgPROCLIST_CMD(const msgPROCLIST_CMD&);
     msgPROCLIST_CMD& operator=(const msgPROCLIST_CMD&);

};

#endif /*!msgPROCLIST_CMD_H*/

/*___oOo___*/
