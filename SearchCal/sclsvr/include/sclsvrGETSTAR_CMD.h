/*
 * 
 * This file has been automatically generated
 * 
 * !!!!!!!!!!!  DO NOT MANUALLY EDIT THIS FILE  !!!!!!!!!!!
 */
#ifndef sclsvrGETSTAR_CMD_H
#define sclsvrGETSTAR_CMD_H

/**
 * \file
 * sclsvrGETSTAR_CMD class declaration.
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
 * Class declaration
 */
        
    
/**
 * This class is intented to be used for a
 * reception of the GETSTAR command 
 */

class sclsvrGETSTAR_CMD: public cmdCMD
{
public:
sclsvrGETSTAR_CMD(string name, string params);
    virtual ~sclsvrGETSTAR_CMD();
    virtual mcsCOMPL_STAT getObjectName(char **objectName);

protected:

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
     sclsvrGETSTAR_CMD(const sclsvrGETSTAR_CMD&);
     sclsvrGETSTAR_CMD& operator=(const sclsvrGETSTAR_CMD&);

};

#endif /*!sclsvrGETSTAR_CMD_H*/

/*___oOo___*/
