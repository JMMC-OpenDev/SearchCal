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
#include "sclsvrSCENARIO_BRIGHT_K_CATALOG.h"
#include "sclsvrSCENARIO_BRIGHT_V.h"
#include "sclsvrSCENARIO_BRIGHT_N.h"
#include "sclsvrSCENARIO_FAINT_K.h"
#include "sclsvrSCENARIO_SINGLE_STAR.h"

/* initialize sclsvr module (vobsSTAR meta data) */
void sclsvrInit();

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
    sclsvrSERVER(mcsLOGICAL unique=mcsTRUE);

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
