#ifndef sclsvrSCENARIO_FAINT_K_H
#define sclsvrSCENARIO_FAINT_K_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
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
    sclsvrSCENARIO_FAINT_K(sdbENTRY* progress);

    // Class destructor
    virtual ~sclsvrSCENARIO_FAINT_K();

    virtual mcsCOMPL_STAT Init(vobsSCENARIO_RUNTIME &ctx, vobsREQUEST* request, vobsSTAR_LIST* starList = NULL);

    virtual const char* GetScenarioName() const;

protected:

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    sclsvrSCENARIO_FAINT_K(const sclsvrSCENARIO_FAINT_K&);
    sclsvrSCENARIO_FAINT_K& operator=(const sclsvrSCENARIO_FAINT_K&);

    // Star lists used by this scenario
    vobsSTAR_LIST _starListP;
    vobsSTAR_LIST _starListS1;
    vobsSTAR_LIST _starListS2;

    vobsREQUEST _request;

    // filter on opt=T
    vobsGENERIC_FILTER _filterOptT;

    // filter on opt=U
    vobsGENERIC_FILTER _filterOptU;
};

#endif /*!sclsvrSCENARIO_FAINT_K_H*/

/*___oOo___*/
