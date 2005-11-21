#ifndef vobsCATALOG_UNSO_H
#define vobsCATALOG_UNSO_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsCATALOG_UNSO.h,v 1.1 2005-11-21 13:49:26 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 ******************************************************************************/

/**
 * \file
 * Declaration of vobsCATALOG_UNSO class.
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
 * vobsCATALOG_UNSO is a class which caracterise the UNSO-B1.0 catalog
 *
 * vobsCATALOG_UNSO methods allow to
 * \li Prepare a request
 * \li Send this request to the CDS
 * \li Build a star list from the CDS answer
 */
class vobsCATALOG_UNSO : public vobsREMOTE_CATALOG
{

public:
    // Class constructor
    vobsCATALOG_UNSO();

    // Class destructor
    virtual ~vobsCATALOG_UNSO();

protected:
   virtual mcsCOMPL_STAT WriteQuerySpecificPart(vobsREQUEST &request);

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsCATALOG_UNSO(const vobsCATALOG_UNSO&);
    vobsCATALOG_UNSO& operator=(const vobsCATALOG_UNSO&);
};

#endif /*!vobsCATALOG_UNSO_H*/

/*___oOo___*/
