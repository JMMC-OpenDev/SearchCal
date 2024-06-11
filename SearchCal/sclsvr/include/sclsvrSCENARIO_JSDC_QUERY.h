#ifndef SCLSVRSCENARIO_JSDC_QUERY_H
#define SCLSVRSCENARIO_JSDC_QUERY_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Declaration of sclsvrSCENARIO_JSDC_QUERY class.
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
 * Specific scenario for JSDC querying
 */
class sclsvrSCENARIO_JSDC_QUERY : public vobsSCENARIO
{
public:
    // Class constructor
    sclsvrSCENARIO_JSDC_QUERY(sdbENTRY* progress);

    // Class destructor
    virtual ~sclsvrSCENARIO_JSDC_QUERY();

    virtual mcsCOMPL_STAT Init(vobsSCENARIO_RUNTIME &ctx, vobsREQUEST* request, vobsSTAR_LIST* starList = NULL);

    // Execute the scenario
    virtual mcsCOMPL_STAT Execute(vobsSCENARIO_RUNTIME &ctx, vobsSTAR_LIST &starList);

    virtual const char* GetScenarioName() const;

    /** preload the JSDC catalog at startup */
    static bool loadData();
    /** free the JSDC catalog at shutdown */
    static void freeData();

protected:

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    sclsvrSCENARIO_JSDC_QUERY(const sclsvrSCENARIO_JSDC_QUERY&);
    sclsvrSCENARIO_JSDC_QUERY& operator=(const sclsvrSCENARIO_JSDC_QUERY&) ;

    // bright flag
    mcsLOGICAL _brightFlag;
    
    // reference star
    vobsSTAR _referenceStar;

    // magnitude Ucd
    mcsSTRING32 _magnitudeUcd;

    // criteria list: RA/DEC within radius (arcsec) + magnitude range
    vobsSTAR_COMP_CRITERIA_LIST _criteriaListRaDecMagRange;

    static bool JSDC_Initialized;
    static vobsSTAR_LIST* JSDC_StarList_Bright;
    static vobsSTAR_LIST* JSDC_StarList_Faint;
    static vobsSTAR_LIST* JSDC_StarList_Complete;
} ;

#endif /*!sclsvrSCENARIO_JSDC_QUERY*/

/*___oOo___*/
