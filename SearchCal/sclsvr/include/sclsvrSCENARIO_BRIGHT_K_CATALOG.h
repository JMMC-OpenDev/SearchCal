#ifndef sclsvrSCENARIO_BRIGHT_K_CATALOG_H
#define sclsvrSCENARIO_BRIGHT_K_CATALOG_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Declaration of sclsvrSCENARIO_BRIGHT_K_CATALOG class.
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
 * Specific scenario for K band research
 */
class sclsvrSCENARIO_BRIGHT_K_CATALOG : public vobsSCENARIO
{
public:
    // Class constructor
    sclsvrSCENARIO_BRIGHT_K_CATALOG(sdbENTRY* progress);

    // Class destructor
    virtual ~sclsvrSCENARIO_BRIGHT_K_CATALOG();

    virtual mcsCOMPL_STAT Init(vobsREQUEST* request, vobsSTAR_LIST* starList = NULL);

    virtual const char* GetScenarioName();

protected:

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    sclsvrSCENARIO_BRIGHT_K_CATALOG(const sclsvrSCENARIO_BRIGHT_K_CATALOG&);
    sclsvrSCENARIO_BRIGHT_K_CATALOG& operator=(const sclsvrSCENARIO_BRIGHT_K_CATALOG&);

    // Star list used by this scenario
    vobsSTAR_LIST _starList;

    vobsREQUEST _request;
    vobsREQUEST _requestI280;

    // filter on origin
    vobsORIGIN_FILTER _originFilter;
    // filter on magnitude
    vobsMAGNITUDE_FILTER _magnitudeFilter;
    // filter list build with the 2 filter on origin and magnitude
    vobsFILTER_LIST _filterList;
};

#endif /*!sclsvrSCENARIO_BRIGHT_K_CATALOG_H*/

/*___oOo___*/
