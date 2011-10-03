#ifndef sclsvrREQUEST_H
#define sclsvrREQUEST_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Declaration of sclsvrREQUEST class.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif


/*
 * MCS header
 */
#include "mcs.h"


/*
 * SCALIB header
 */
#include "vobsREQUEST.h"


/*
 * Local header
 */
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

    // Copy
    virtual mcsCOMPL_STAT Copy(const sclsvrREQUEST& request);
    
    // Command parameters
    virtual mcsCOMPL_STAT Parse(const char *cmdParamLine);
    virtual mcsCOMPL_STAT GetCmdParamLine(mcsSTRING256 cmdParamLine) const;

    // Baseline
    virtual mcsCOMPL_STAT SetMaxBaselineLength(mcsDOUBLE length);
    virtual mcsDOUBLE     GetMaxBaselineLength(void) const;

    // Wavelengh
    virtual mcsCOMPL_STAT SetObservingWlen(mcsDOUBLE wlen);
    virtual mcsDOUBLE     GetObservingWlen(void) const;

    // DiamVK
    virtual mcsCOMPL_STAT SetDiamVK(mcsDOUBLE diamVK);
    virtual mcsCOMPL_STAT ResetDiamVK();
    virtual mcsLOGICAL    IsDiamVKDefined(void) const;
    virtual mcsDOUBLE     GetDiamVK(void) const;

    // Expected visibility error
    virtual mcsCOMPL_STAT SetExpectedVisErr(mcsDOUBLE expectedVisErr);
    virtual mcsDOUBLE     GetExpectedVisErr(void) const;
    
    // File name where file should be saved
    virtual mcsCOMPL_STAT SetFileName(mcsSTRING256 fileName);
    virtual const char*   GetFileName(void) const;

    // Bright/Faint scenario
    virtual mcsCOMPL_STAT SetBrightFlag(mcsLOGICAL brightFlag);
    virtual mcsLOGICAL    IsBright(void) const;

    // Science star
    virtual mcsCOMPL_STAT SetNoScienceStar(mcsLOGICAL noScienceStar);
    virtual mcsLOGICAL    IsNoScienceStar() const;

    virtual const mcsCOMPL_STAT AppendParamsToVOTable(string& voTable);
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    sclsvrREQUEST& operator=(const sclsvrREQUEST&);

    // GETCAL command
    sclsvrGETCAL_CMD *_getCalCmd;

    // Request parameters
    mcsDOUBLE      _maxBaselineLength;
    mcsDOUBLE      _observingWlen;
    mcsDOUBLE      _diamVK;
    mcsLOGICAL    _diamVKDefined;
    mcsDOUBLE      _expectedVisibilityError;
    mcsLOGICAL    _brightFlag;
    mcsSTRING256  _fileName;
    mcsLOGICAL    _noScienceObject;
};

#endif /*!sclsvrREQUEST_H*/

/*___oOo___*/
