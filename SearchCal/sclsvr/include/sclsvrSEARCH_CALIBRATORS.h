#ifndef sclsvrSEARCH_CALIBRATORS_H
#define sclsvrSEARCH_CALIBRATORS_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: sclsvrSEARCH_CALIBRATORS.h,v 1.2 2005-01-27 15:56:29 scetre Exp $"
*
* History
* -------
* scetre    15-Sep-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * sclsvrSEARCH_CALIBRATORS class declaration.
 */


#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include "vobs.h"

/*
 * Class declaration
 */

class sclsvrSEARCH_CALIBRATORS
{
public:
    // Constructors
    sclsvrSEARCH_CALIBRATORS();    
    // Destructor
    virtual ~sclsvrSEARCH_CALIBRATORS();

    mcsCOMPL_STAT DecodeCommand(int argc, char *argv[]);
    mcsCOMPL_STAT Run();
protected:

private:
    // Declaration of assignment operator as private
    // method, in order to hide them from the users.
    sclsvrSEARCH_CALIBRATORS& operator=(const sclsvrSEARCH_CALIBRATORS&);
    sclsvrSEARCH_CALIBRATORS (const sclsvrSEARCH_CALIBRATORS &sclsvr);

    vobsREQUEST request;
    vobsVIRTUAL_OBSERVATORY virtualObservatory;
    vobsSTAR_LIST starList;

    sclsvrCALIBRATOR_LIST calibratorList;
};




#endif /*!sclsvrSEARCH_CALIBRATORS_H*/

/*___oOo___*/
