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

    virtual const char* GetScenarioName() const;

    virtual mcsCOMPL_STAT Init(vobsSCENARIO_RUNTIME &ctx, vobsREQUEST* request, vobsSTAR_LIST* starList);

protected:

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    sclsvrSCENARIO_SINGLE_STAR(const sclsvrSCENARIO_SINGLE_STAR&);
    sclsvrSCENARIO_SINGLE_STAR& operator=(const sclsvrSCENARIO_SINGLE_STAR&);

    // Star lists used by this scenario
    vobsSTAR_LIST _starList;

    vobsREQUEST _request;
};

#endif /*!sclsvrSCENARIO_SINGLE_STAR_H*/

/*___oOo___*/
