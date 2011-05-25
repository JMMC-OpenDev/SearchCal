#ifndef sclsvrSCENARIO_BRIGHT_N_H
#define sclsvrSCENARIO_BRIGHT_N_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Declaration of sclsvrSCENARIO_BRIGHT_N class.
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
 * Specific scenario for N band research
 */
class sclsvrSCENARIO_BRIGHT_N : public vobsSCENARIO
{

public:
    // Class constructor
    sclsvrSCENARIO_BRIGHT_N(sdbENTRY* progress);

    // Class destructor
    virtual ~sclsvrSCENARIO_BRIGHT_N();

    virtual mcsCOMPL_STAT Init(vobsREQUEST * request);
    
protected:
    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    sclsvrSCENARIO_BRIGHT_N(const sclsvrSCENARIO_BRIGHT_N&);
    sclsvrSCENARIO_BRIGHT_N& operator=(const sclsvrSCENARIO_BRIGHT_N&);

    // Star lists used in scenarion
    vobsSTAR_LIST     _starListP;
    vobsSTAR_LIST     _starListS;

    vobsREQUEST _request;
};

#endif /*!sclsvrSCENARIO_BRIGHT_N_H*/

/*___oOo___*/
