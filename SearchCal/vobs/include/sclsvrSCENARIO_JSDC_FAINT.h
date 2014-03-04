#ifndef SCLSVRSCENARIO_JSDC_FAINT_H
#define	SCLSVRSCENARIO_JSDC_FAINT_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Declaration of sclsvrSCENARIO_JSDC_FAINT class.
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
 * Specific scenario for JSDC (faint)
 */
class sclsvrSCENARIO_JSDC_FAINT : public vobsSCENARIO
{
public:
    // Class constructor
    sclsvrSCENARIO_JSDC_FAINT(sdbENTRY* progress);

    // Class destructor
    virtual ~sclsvrSCENARIO_JSDC_FAINT();

    virtual mcsCOMPL_STAT Init(vobsSCENARIO_RUNTIME &ctx, vobsREQUEST* request, vobsSTAR_LIST* starList = NULL);

    virtual const char* GetScenarioName() const;

protected:

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    sclsvrSCENARIO_JSDC_FAINT(const sclsvrSCENARIO_JSDC_FAINT&);
    sclsvrSCENARIO_JSDC_FAINT& operator=(const sclsvrSCENARIO_JSDC_FAINT&) ;

    // Star lists used by this scenario
    vobsSTAR_LIST _starList;

    vobsREQUEST _request;
} ;

#endif	/*!sclsvrSCENARIO_JSDC_FAINT*/

/*___oOo___*/
