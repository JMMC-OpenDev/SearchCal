#ifndef vobsVIRTUAL_OBSERVATORY_H
#define vobsVIRTUAL_OBSERVATORY_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsVIRTUAL_OBSERVATORY.h,v 1.3 2004-08-03 13:44:10 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    06-Jul-2004  Created
*
*
*******************************************************************************/
/**
 * \file
 * vobsVIRTUAL_OBSERVATORY class declaration.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/*
 * Local header
 */
#include "vobsREQUEST.h"
#include "vobsSTAR_LIST.h"


/*
 * Class declaration
 */

/**
 * vobsVIRTUAL_OBSERVATORY is a class which represent the virtual observatory.
 * 
 * vobsVIRTUAL_OBSERVATORY methods allow to
 * \li Load the scenario of research : the order of interrogation of the
 * catalogs
 * \li search in all the catalogs
 * 
 */

class vobsVIRTUAL_OBSERVATORY
{
public:
     // Constructor
    vobsVIRTUAL_OBSERVATORY();

    // Destructor
    virtual ~vobsVIRTUAL_OBSERVATORY();
 
    // Method to star the research in the different catalog
    virtual mcsCOMPL_STAT Research(vobsREQUEST request,
                                   vobsSTAR_LIST &StarList);

protected:
    // Method to load scenario according to the request
    mcsCOMPL_STAT LoadScenario(vobsREQUEST request,
                               vobsSTAR_LIST &StarList);

private:
    // Declaration of assignment operator as private
    // method, in order to hide them from the users.
    vobsVIRTUAL_OBSERVATORY& operator=(const vobsVIRTUAL_OBSERVATORY&);
    vobsVIRTUAL_OBSERVATORY (const vobsVIRTUAL_OBSERVATORY&);
};


#endif /*!vobsVIRTUAL_OBSERVATORY_H*/


/*___oOo___*/
