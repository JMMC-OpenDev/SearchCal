#ifndef sclsvrSCENARIO_SINGLE_STAR_H
#define sclsvrSCENARIO_SINGLE_STAR_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Declaration of sclsvrSCENARIO_SINGLE_STAR class.
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
 * Specific scenario for single star research 
 */
class sclsvrSCENARIO_SINGLE_STAR : public vobsSCENARIO
{

public:
    // Class constructor
    sclsvrSCENARIO_SINGLE_STAR(sdbENTRY* progress);

    // Class destructor
    virtual ~sclsvrSCENARIO_SINGLE_STAR();

    virtual mcsCOMPL_STAT Init(vobsREQUEST * request,
                               vobsSTAR_LIST &starList);
    
protected:
    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    sclsvrSCENARIO_SINGLE_STAR(const sclsvrSCENARIO_SINGLE_STAR&);
    sclsvrSCENARIO_SINGLE_STAR& operator=(const sclsvrSCENARIO_SINGLE_STAR&);

    // Star lists used in scenarion
    vobsSTAR_LIST     _starListP;
    vobsSTAR_LIST     _starListS;

    vobsREQUEST _request;   

    // criteria list for LBSI, Merand, charm2, denis JK, 2MASS, II/225
    vobsSTAR_COMP_CRITERIA_LIST _criteriaListRaDec;
    // criteria list for II/7A
    vobsSTAR_COMP_CRITERIA_LIST _criteriaListRaDecMagV;
    // criteria list for I/196, BSC, SBSC
    vobsSTAR_COMP_CRITERIA_LIST _criteriaListRaDecHd;
};

#endif /*!sclsvrSCENARIO_SINGLE_STAR_H*/

/*___oOo___*/
