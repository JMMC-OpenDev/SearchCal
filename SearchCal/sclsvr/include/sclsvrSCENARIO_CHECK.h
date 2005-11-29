#ifndef sclsvrSCENARIO_CHECK_H
#define sclsvrSCENARIO_CHECK_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrSCENARIO_CHECK.h,v 1.1 2005-11-29 08:23:19 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 ******************************************************************************/

/**
 * \file
 * Declaration of sclsvrSCENARIO_CHECK class.
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
 * Decisionnal scenario
 *
 * This senario is a simple query to 2MASS catalog in order to know how many
 * stars are returned
 */
class sclsvrSCENARIO_CHECK : public vobsSCENARIO
{

public:
    // Class constructor
    sclsvrSCENARIO_CHECK();

    // Class destructor
    virtual ~sclsvrSCENARIO_CHECK();

    virtual mcsCOMPL_STAT Init(vobsREQUEST * request);    
protected:
    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    sclsvrSCENARIO_CHECK(const sclsvrSCENARIO_CHECK&);
    sclsvrSCENARIO_CHECK& operator=(const sclsvrSCENARIO_CHECK&);

    // Star lists used in scenarion
    vobsSTAR_LIST     _starListP;
    vobsSTAR_LIST     _starListS;

    vobsREQUEST _request;

};

#endif /*!sclsvrSCENARIO_CHECK_H*/

/*___oOo___*/
