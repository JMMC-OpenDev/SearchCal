#ifndef sclsvrSERVER_H
#define sclsvrSERVER_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrSERVER.h,v 1.19.2.1 2011-04-15 22:38:05 duvert Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.19  2010/11/10 15:45:16  lafrasse
 * Decorelated GETSTAR from EVH callback mecanism.
 *
 * Revision 1.18  2009/12/17 15:14:31  lafrasse
 * *** empty log message ***
 *
 * Revision 1.17  2007/10/31 11:28:20  gzins
 * Removed oldScenario parameter
 * Updated to use new sdbENTRY non-blocking class
 * Updated API related to web-service
 *
 * Revision 1.16  2007/02/04 20:50:37  lafrasse
 * Replaced GetCalStatus() by WaitForCurrentCatalogName() and IsLastCatalog().
 * Added GetCatalogIndex() API to get the index of the catalog being queried.
 * Added GetNbOfCatalogs() API to get the number of catalogs of the scenario.
 *
 * Revision 1.15  2006/12/21 15:16:05  lafrasse
 * Updated progression monitoring code (moved from static-based to instance-based).
 *
 * Revision 1.14  2006/07/17 09:10:36  scetre
 * Added old scenario option
 *
 * Revision 1.13  2006/03/07 15:33:39  scetre
 * Removed old scenario in band K
 *
 * Revision 1.12  2005/12/12 14:10:00  scetre
 * Added old scenario with 2mass in primary request
 *
 * Revision 1.11  2005/11/24 13:21:26  scetre
 * Added an element scenario faint K
 *
 * Revision 1.10  2005/11/21 13:51:47  scetre
 * Changed bad scenario name
 * Added scenario for single star research -> updated getStar
 *
 * Revision 1.9  2005/11/15 15:01:19  scetre
 * Updated with new scenario structure
 *
 * Revision 1.8  2005/10/26 11:27:24  lafrasse
 * Code review
 *
 * Revision 1.7  2005/03/06 20:29:40  gzins
 * Added GetSwVersion method
 *
 * Revision 1.6  2005/02/07 15:00:23  gzins
 * Added CVS log as modification history
 *
 * scetre    04-Oct-2004  Created
 *
 ******************************************************************************/

/**
 * @file
 * sclsvrSERVER class declaration.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif


/*
 * MCS Headers 
 */
#include "evh.h"
#include "sdb.h"


/*
 * SCALIB Headers 
 */
#include "vobs.h"
#include "sclsvrSCENARIO_BRIGHT_K.h"
#include "sclsvrSCENARIO_JSDC.h"
#include "sclsvrSCENARIO_BRIGHT_K_CATALOG.h"
#include "sclsvrSCENARIO_BRIGHT_V.h"
#include "sclsvrSCENARIO_BRIGHT_N.h"
#include "sclsvrSCENARIO_FAINT_K.h"
#include "sclsvrSCENARIO_SINGLE_STAR.h"

/*
 * Class declaration
 */
/**
 * Server main class 
 */
class sclsvrSERVER : public evhSERVER 
{

public:
    // Constructor
    sclsvrSERVER();

    // Destructor
    virtual ~sclsvrSERVER();
    
    // Application initialization 
    virtual mcsCOMPL_STAT AppInit();
    
    // Software version 
    virtual const char *GetSwVersion();

    // Command callbacks
    virtual evhCB_COMPL_STAT GetCalCB(msgMESSAGE &msg, void*);
    virtual evhCB_COMPL_STAT GetStarCB(msgMESSAGE &msg, void*);

    // Method to invoke command directly
    virtual mcsCOMPL_STAT GetCal(const char* query, miscoDYN_BUF &dynBuf);
    virtual mcsCOMPL_STAT GetStar(const char* query, miscoDYN_BUF &dynBuf);


    // Get request execution status 
    virtual mcsCOMPL_STAT GetStatus(char* buffer, mcsINT32 timeoutInSec=300);

protected:
    virtual mcsCOMPL_STAT ProcessGetCalCmd(const char* query, 
                                           miscoDYN_BUF &dynBuf,
                                           msgMESSAGE* msg);
    virtual evhCB_COMPL_STAT ProcessGetStarCmd(const char* query, 
                                               miscoDYN_BUF &dynBuf,
                                               msgMESSAGE* msg);

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    sclsvrSERVER(const sclsvrSERVER&);
    sclsvrSERVER& operator=(const sclsvrSERVER&); 

    // Query progression monitoring
    sdbENTRY  _status;

    // Virtual observatory
    vobsVIRTUAL_OBSERVATORY          _virtualObservatory;
    sclsvrSCENARIO_BRIGHT_K          _scenarioBrightK;
    sclsvrSCENARIO_JSDC              _scenarioJSDC;
    sclsvrSCENARIO_BRIGHT_K_CATALOG  _scenarioBrightKCatalog;
    sclsvrSCENARIO_BRIGHT_V          _scenarioBrightV;
    sclsvrSCENARIO_BRIGHT_N          _scenarioBrightN;
    sclsvrSCENARIO_FAINT_K           _scenarioFaintK;
    sclsvrSCENARIO_SINGLE_STAR       _scenarioSingleStar;
};

#endif /*!sclsvrSERVER_H*/

/*___oOo___*/
