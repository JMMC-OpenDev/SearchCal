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

    // Command parameters
    virtual mcsCOMPL_STAT Parse(const char *cmdParamLine);
    virtual mcsCOMPL_STAT GetCmdParamLine(mcsSTRING256 cmdParamLine) const;

    // Set search band (overriden)
    virtual mcsCOMPL_STAT SetSearchBand(const char* searchBand);

    // Baseline
    virtual mcsCOMPL_STAT SetMaxBaselineLength(mcsDOUBLE length);
    virtual mcsDOUBLE GetMaxBaselineLength(void) const;

    // Wavelength
    virtual mcsCOMPL_STAT SetObservingWlen(mcsDOUBLE wlen);
    virtual mcsDOUBLE GetObservingWlen(void) const;

    // Bright/Faint scenario
    virtual mcsCOMPL_STAT SetBrightFlag(mcsLOGICAL brightFlag);
    virtual mcsLOGICAL IsBright(void) const;

    // Science star
    virtual mcsCOMPL_STAT SetNoScienceStar(mcsLOGICAL noScienceStar);
    virtual mcsLOGICAL IsNoScienceStar() const;

    // File name where file should be saved
    virtual mcsCOMPL_STAT SetFileName(mcsSTRING256 fileName);
    virtual const char* GetFileName(void) const;

    // Output format
    virtual mcsCOMPL_STAT SetOutputFormat(mcsDOUBLE outputFormat);
    virtual mcsDOUBLE GetOutputFormat(void) const;

    // Diagnose
    virtual mcsCOMPL_STAT SetDiagnose(mcsLOGICAL diagnose);
    virtual mcsLOGICAL IsDiagnose() const;

    virtual mcsCOMPL_STAT AppendParamsToVOTable(string& voTable);

    virtual mcsCOMPL_STAT SetJSDCMode(mcsLOGICAL mode);
    virtual mcsLOGICAL IsJSDCMode() const;

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    sclsvrREQUEST& operator=(const sclsvrREQUEST&) ;

    // GETCAL command
    sclsvrGETCAL_CMD* _getCalCmd;

    // Request parameters
    mcsDOUBLE    _maxBaselineLength;
    mcsDOUBLE    _observingWlen;
    mcsLOGICAL   _brightFlag;
    mcsSTRING256 _fileName;
    mcsLOGICAL   _noScienceObject;
    mcsDOUBLE    _outputFormat;
    mcsLOGICAL   _diagnose;

    // Special flags
    // JSDC mode indicates JSDC scenario (ie skip several useless computation steps)
    mcsLOGICAL   _jsdcMode;
} ;

#endif /*!sclsvrREQUEST_H*/

/*___oOo___*/
