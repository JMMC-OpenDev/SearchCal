#ifndef sclsvrSCENARIO_FAINT_K_H
#define sclsvrSCENARIO_FAINT_K_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrSCENARIO_FAINT_K.h,v 1.5 2005-11-29 13:06:50 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.4  2005/11/29 08:23:19  scetre
 * Added check scenario for scenario faint K
 *
 * Revision 1.3  2005/11/25 08:45:55  scetre
 * Added filter Qflag=AAA
 *
 * Revision 1.2  2005/11/24 13:21:38  scetre
 * Clean the presentation of the add entry
 *
 * Revision 1.1  2005/11/23 08:39:03  scetre
 * Added scenario for faint K
 *
 ******************************************************************************/

/**
 * \file
 * Declaration of sclsvrSCENARIO_FAINT_K class.
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
 * Specific scenario for faint K band research 
 */
class sclsvrSCENARIO_FAINT_K : public vobsSCENARIO
{

public:
    // Class constructor
    sclsvrSCENARIO_FAINT_K();

    // Class destructor
    virtual ~sclsvrSCENARIO_FAINT_K();

    virtual mcsCOMPL_STAT Init(vobsREQUEST * request);
    
protected:
    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    sclsvrSCENARIO_FAINT_K(const sclsvrSCENARIO_FAINT_K&);
    sclsvrSCENARIO_FAINT_K& operator=(const sclsvrSCENARIO_FAINT_K&);

    // Star lists used in scenarion
    vobsSTAR_LIST     _starListP;
    vobsSTAR_LIST     _starListS1;
    vobsSTAR_LIST     _starListS2;

    vobsREQUEST _request;
    
    vobsSTAR_COMP_CRITERIA_LIST _criteriaListRaDec;    

    // filter on opt=T
    vobsGENERIC_FILTER _filterOptT;

    // filter on opt=U
    vobsGENERIC_FILTER _filterOptU;
    
    // filter on Qflg=AAA
    vobsGENERIC_FILTER _filterOnQflag;

};

#endif /*!sclsvrSCENARIO_FAINT_K_H*/

/*___oOo___*/
