#ifndef sclsvrREQUEST_H
#define sclsvrREQUEST_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrREQUEST.h,v 1.1 2005-02-07 14:43:41 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 ******************************************************************************/

/**
 * \file
 * Declaration of sclsvrREQUEST class.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif


/*
 * MCS header
 */
#include "mcs.h"

#include "vobsREQUEST.h"

/*
 * Class declaration
 */

/**
 * Class containing user requirements for visibility estimation.
 * 
 * Class used to store observation conditions to estimate visibility of the
 * stars selected to be calibrator.
 */
class sclsvrREQUEST : public vobsREQUEST
{
public:
    // Class constructor
    sclsvrREQUEST();

    // Class destructor
    virtual ~sclsvrREQUEST();

    // Baseline
    virtual mcsCOMPL_STAT SetBaseline(mcsFLOAT minLength, mcsFLOAT maxLength);
    virtual mcsFLOAT GetMinBaselineLength(void);
    virtual mcsFLOAT GetMaxBaselineLength(void);

    // Wavelengh 
    virtual mcsCOMPL_STAT SetObservingWlen(mcsFLOAT wlen);
    virtual mcsFLOAT GetObservingWlen(void);

    // Expected visibility 
    virtual mcsCOMPL_STAT SetExpectedVisibility(mcsFLOAT vis, mcsFLOAT visErr);
    virtual mcsFLOAT GetExpectedVisibility(void);
    virtual mcsFLOAT GetExpectedVisibilityError(void);

protected:
    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    sclsvrREQUEST(const sclsvrREQUEST&);
    sclsvrREQUEST& operator=(const sclsvrREQUEST&);

    mcsFLOAT  _minBaselineLength;
    mcsFLOAT  _maxBaselineLength;
    mcsFLOAT  _observingWlen;
    mcsFLOAT  _vis;
    mcsFLOAT  _visErr;
};

#endif /*!sclsvrREQUEST_H*/

/*___oOo___*/
