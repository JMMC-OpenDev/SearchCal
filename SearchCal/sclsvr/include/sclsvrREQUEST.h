#ifndef sclsvrREQUEST_H
#define sclsvrREQUEST_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrREQUEST.h,v 1.3 2005-02-17 15:32:04 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2005/02/16 17:06:58  gzins
 * Added Parse method
 *
 * Revision 1.1  2005/02/07 14:43:41  gzins
 * Created
 *
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
#include "sclsvrGETCAL_CMD.h"

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

    // Command parameters 
    virtual mcsCOMPL_STAT Parse(const char *cmdParamLine);
    virtual mcsCOMPL_STAT GetCmdParamLine(mcsSTRING256 cmdParamLine);
    
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

    // GETCAL command
    sclsvrGETCAL_CMD *_getCalCmd;

    mcsFLOAT  _minBaselineLength;
    mcsFLOAT  _maxBaselineLength;
    mcsFLOAT  _observingWlen;
    mcsFLOAT  _vis;
    mcsFLOAT  _visErr;
};

#endif /*!sclsvrREQUEST_H*/

/*___oOo___*/
