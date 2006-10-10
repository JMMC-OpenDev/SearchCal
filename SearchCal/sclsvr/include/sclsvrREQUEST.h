#ifndef sclsvrREQUEST_H
#define sclsvrREQUEST_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrREQUEST.h,v 1.19 2006-10-10 11:30:17 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.18  2006/10/09 15:07:05  lafrasse
 * Added request XML serialization in VOTables.
 *
 * Revision 1.17  2006/07/17 09:10:36  scetre
 * Added old scenario option
 *
 * Revision 1.16  2006/04/10 12:07:31  gzins
 * Renamed SetExpectingVisErr to SetExpectedVisErr
 *
 * Revision 1.15  2006/03/07 15:33:39  scetre
 * Removed old scenario in band K
 *
 * Revision 1.14  2006/03/06 21:23:28  lafrasse
 * Added diamVK resetabilty
 *
 * Revision 1.13  2006/03/06 17:09:47  lafrasse
 * Added diamVK parameter
 *
 * Revision 1.12  2005/12/16 14:17:11  scetre
 * Added test when computing visibility Added visibility error as parameter
 *
 * Revision 1.11  2005/12/12 14:11:01  scetre
 * Added -oldScenario option to the GETCAL command -> request can managed it
 *
 * Revision 1.10  2005/11/23 14:35:33  lafrasse
 * Added fileName proper management (strncpy() calls instead of strcpy())
 * Removed unused 'MaxReturn' command parmater
 * Added 'bright' command parameter
 *
 * Revision 1.9  2005/11/15 15:01:19  scetre
 * Updated with new scenario structure
 *
 * Revision 1.8  2005/10/26 11:27:24  lafrasse
 * Code review
 *
 * Revision 1.7  2005/03/07 16:06:06  gzins
 * Removed automatic sort on visibility
 *
 * Revision 1.6  2005/03/07 13:39:46  gzins
 * Remove min baseline length
 *
 * Revision 1.5  2005/03/04 09:46:04  gzins
 * Rename Set/GetSaveFileName to  Set/GetFileName
 *
 * Revision 1.4  2005/02/28 13:48:56  scetre
 * Added save file option in the request
 *
 * Revision 1.3  2005/02/17 15:32:04  gzins
 * Added GetCmdParamLine method
 *
 * Revision 1.2  2005/02/16 17:06:58  gzins
 * Added Parse method
 *
 * Revision 1.1  2005/02/07 14:43:41  gzins
 * Created
 *
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
    virtual mcsCOMPL_STAT Copy(sclsvrREQUEST& request);
    
    // Command parameters
    virtual mcsCOMPL_STAT Parse(const char *cmdParamLine);
    virtual mcsCOMPL_STAT GetCmdParamLine(mcsSTRING256 cmdParamLine);

    // Baseline
    virtual mcsCOMPL_STAT SetMaxBaselineLength(mcsFLOAT length);
    virtual mcsFLOAT      GetMaxBaselineLength(void);

    // Wavelengh
    virtual mcsCOMPL_STAT SetObservingWlen(mcsFLOAT wlen);
    virtual mcsFLOAT      GetObservingWlen(void);

    // DiamVK
    virtual mcsCOMPL_STAT SetDiamVK(mcsFLOAT diamVK);
    virtual mcsCOMPL_STAT ResetDiamVK();
    virtual mcsLOGICAL    IsDiamVKDefined(void);
    virtual mcsFLOAT      GetDiamVK(void);

    // Expected visibility error
    virtual mcsCOMPL_STAT SetExpectedVisErr(mcsFLOAT expectedVisErr);
    virtual mcsFLOAT      GetExpectedVisErr(void);
    
    // File name where file should be saved
    virtual mcsCOMPL_STAT SetFileName(mcsSTRING256 fileName);
    virtual const char*   GetFileName(void);

    // Brightness
    virtual mcsCOMPL_STAT SetBrightFlag(mcsLOGICAL brightFlag);
    virtual mcsLOGICAL    IsBright(void);

    // Scenario
    virtual mcsCOMPL_STAT SetOldScenario(mcsLOGICAL oldScenario);
    virtual mcsLOGICAL    IsOldScenario();

    virtual const mcsCOMPL_STAT SerializeToXML(string& xmlOutput);
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    sclsvrREQUEST& operator=(const sclsvrREQUEST&);

    // GETCAL command
    sclsvrGETCAL_CMD *_getCalCmd;

    // Request parameters
    mcsFLOAT      _maxBaselineLength;
    mcsFLOAT      _observingWlen;
    mcsFLOAT      _diamVK;
    mcsLOGICAL    _diamVKDefined;
    mcsFLOAT      _expectedVisibilityError;
    mcsLOGICAL    _brightFlag;
    mcsLOGICAL    _oldScenario;    
    mcsSTRING256  _fileName;
};

#endif /*!sclsvrREQUEST_H*/

/*___oOo___*/
