#ifndef sclsvrSERVER_H
#define sclsvrSERVER_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
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
#include "sclsvrSCENARIO_JSDC_FAINT.h"
#include "sclsvrSCENARIO_BRIGHT_V.h"
#include "sclsvrSCENARIO_FAINT_K.h"
#include "sclsvrSCENARIO_SINGLE_STAR.h"
#include "sclsvrSCENARIO_JSDC_QUERY.h"

/* initialize sclsvr module (vobsSTAR meta data) */
void sclsvrInit(bool loadJSDC);

/* clean sclsvr module on exit */
void sclsvrExit();

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
    sclsvrSERVER(mcsLOGICAL unique = mcsTRUE);

    // Destructor
    virtual ~sclsvrSERVER();

    // Application initialization
    virtual mcsCOMPL_STAT AppInit();

    // Software version
    virtual const char* GetSwVersion();

    // Command callbacks
    virtual evhCB_COMPL_STAT GetCalCB(msgMESSAGE &msg, void*);
    virtual evhCB_COMPL_STAT GetStarCB(msgMESSAGE &msg, void*);

    // Method to invoke command directly
    virtual mcsCOMPL_STAT GetCal(const char* query, miscoDYN_BUF* dynBuf = NULL);
    virtual mcsCOMPL_STAT GetStar(const char* query, miscoDYN_BUF* dynBuf = NULL);


    // Get request execution status
    virtual mcsCOMPL_STAT GetStatus(char* buffer, mcsINT32 timeoutInSec = 300);

    // Dump the configuration as xml files
    mcsCOMPL_STAT DumpConfigAsXML();

    inline bool* GetCancelFlag(void) const __attribute__((always_inline))
    {
        return _cancelFlag;
    }

    inline bool IsCancelled(void) const __attribute__((always_inline))
    {
        return *_cancelFlag;
    }

    static void SetQueryJSDC(bool flag)
    {
        sclsvrSERVER_queryJSDC = flag;
    }

    static bool IsQueryJSDC(void)
    {
        return sclsvrSERVER_queryJSDC;
    }


protected:
    virtual mcsCOMPL_STAT ProcessGetCalCmd(const char* query,
                                           miscoDYN_BUF* dynBuf,
                                           msgMESSAGE* msg = NULL);
    virtual evhCB_COMPL_STAT ProcessGetStarCmd(const char* query,
                                               miscoDYN_BUF* dynBuf,
                                               msgMESSAGE* msg);

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    sclsvrSERVER(const sclsvrSERVER&);
    sclsvrSERVER& operator=(const sclsvrSERVER&) ;

    // Query progression monitoring
    sdbENTRY _status;

    // Virtual observatory
    vobsVIRTUAL_OBSERVATORY _virtualObservatory;
    sclsvrSCENARIO_BRIGHT_K _scenarioBrightK;
    sclsvrSCENARIO_JSDC _scenarioJSDC;
    sclsvrSCENARIO_JSDC_FAINT _scenarioJSDC_Faint;
    sclsvrSCENARIO_JSDC_QUERY _scenarioJSDC_Query;
    sclsvrSCENARIO_BRIGHT_V _scenarioBrightV;
    sclsvrSCENARIO_FAINT_K _scenarioFaintK;
    sclsvrSCENARIO_SINGLE_STAR _scenarioSingleStar;

    // flag to load/save vobsStarList results:
    bool _useVOStarListBackup;

    // cancellation flag as bool Pointer
    bool* _cancelFlag;

    // flag to use the JSDC Query scenario instead of "live" scenarios
    static bool sclsvrSERVER_queryJSDC;
} ;

#endif /*!sclsvrSERVER_H*/

/*___oOo___*/
