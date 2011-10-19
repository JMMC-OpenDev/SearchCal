#ifndef vobsCATALOG_MERAND_H
#define vobsCATALOG_MERAND_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Declaration of vobsCATALOG_MERAND class.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif


/*
 * MCS header
 */
#include "mcs.h"

/*
 * Local header
 */
#include "vobsREMOTE_CATALOG.h"

/*
 * Class declaration
 */

/**
 * vobsCATALOG_MERAND is a class which caracterise the Catalog of calibrators
 * star for LBI.
 * 
 * vobsCATALOG_MERAND methods allow to
 * \li Prepare a request
 * \li Send this request to the CDS
 * \li Build a star list from the CDS answer

 */
class vobsCATALOG_MERAND : public vobsREMOTE_CATALOG
{

public:
    // Class constructor
    vobsCATALOG_MERAND();

    // Class destructor
    virtual ~vobsCATALOG_MERAND();
   
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsCATALOG_MERAND(const vobsCATALOG_MERAND&);
    vobsCATALOG_MERAND& operator=(const vobsCATALOG_MERAND&);
    
    // Method to build specific CDS query part
    virtual mcsCOMPL_STAT WriteQuerySpecificPart(void);
    virtual mcsCOMPL_STAT WriteQuerySpecificPart(vobsREQUEST &request);
};

#endif /*!vobsCATALOG_MERAND_H*/

/*___oOo___*/
