#ifndef sclsvrSCENARIO_BRIGHT_V_H
#define sclsvrSCENARIO_BRIGHT_V_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrSCENARIO_BRIGHT_V.h,v 1.3 2005-11-23 08:38:14 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2005/11/16 14:28:02  scetre
 * Used criteria list as member of the class. All necessary criteria list are now built at the beginning of the Init() method
 *
 * Revision 1.1  2005/11/15 15:00:33  scetre
 * Added scenario K V and N
 *
 ******************************************************************************/

/**
 * \file
 * Declaration of sclsvrSCENARIO_BRIGHT_V class.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif


/*
 * MCS header
 */
#include "mcs.h"
#include "vobs.h"

/*
 * Class declaration
 */

/**
 * Specific scenario for V band research
 */
class sclsvrSCENARIO_BRIGHT_V : public vobsSCENARIO
{

public:
    // Class constructor
    sclsvrSCENARIO_BRIGHT_V();

    // Class destructor
    virtual ~sclsvrSCENARIO_BRIGHT_V();

    virtual mcsCOMPL_STAT Init(vobsREQUEST * request);

protected:
    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    sclsvrSCENARIO_BRIGHT_V(const sclsvrSCENARIO_BRIGHT_V&);
    sclsvrSCENARIO_BRIGHT_V& operator=(const sclsvrSCENARIO_BRIGHT_V&);

    // Star lists used in scenarion
    vobsSTAR_LIST     _starListP;
    vobsSTAR_LIST     _starListS;

    vobsREQUEST _request;
    
    vobsSTAR_COMP_CRITERIA_LIST _criteriaList;    
    vobsSTAR_COMP_CRITERIA_LIST _criteriaListHd;    
};

#endif /*!sclsvrSCENARIO_BRIGHT_V_H*/

/*___oOo___*/
