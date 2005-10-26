#ifndef sclsvrREQUEST_H
#define sclsvrREQUEST_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrREQUEST.h,v 1.8 2005-10-26 11:27:24 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
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

    // Command parameters 
    virtual mcsCOMPL_STAT Parse(const char *cmdParamLine);
    virtual mcsCOMPL_STAT GetCmdParamLine(mcsSTRING256 cmdParamLine);

    // Baseline
    virtual mcsCOMPL_STAT SetMaxBaselineLength(mcsFLOAT length);
    virtual mcsFLOAT GetMaxBaselineLength(void);

    // Wavelengh 
    virtual mcsCOMPL_STAT SetObservingWlen(mcsFLOAT wlen);
    virtual mcsFLOAT GetObservingWlen(void);

    // File name where file should be saved 
    virtual mcsCOMPL_STAT SetFileName(mcsSTRING256 fileName);
    virtual const char *GetFileName(void);


protected:


private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    sclsvrREQUEST(const sclsvrREQUEST&);
    sclsvrREQUEST& operator=(const sclsvrREQUEST&);

    // GETCAL command
    sclsvrGETCAL_CMD *_getCalCmd;

    mcsFLOAT        _maxBaselineLength;
    mcsFLOAT        _observingWlen;
    mcsSTRING256    _fileName;
};

#endif /*!sclsvrREQUEST_H*/

/*___oOo___*/
