#ifndef sclsvrSCENARIO_FAINT_K_H
#define sclsvrSCENARIO_FAINT_K_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrSCENARIO_FAINT_K.h,v 1.1 2005-11-23 08:39:03 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
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
    vobsSTAR_LIST     _starListS;
    vobsSTAR_LIST     _starListS1;
    vobsSTAR_LIST     _starListS2;

    vobsREQUEST _request;
    
    vobsSTAR_COMP_CRITERIA_LIST _criteriaListRaDec;    
};

#endif /*!sclsvrSCENARIO_FAINT_K_H*/

/*___oOo___*/
