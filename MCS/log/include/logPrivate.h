#ifndef logLOG_PRIVATE_H
#define logLOG_PRIVATE_H
/*******************************************************************************
*  JMMC Project
*  
*  "@(#) $Id: logPrivate.h,v 1.1 2004-05-14 09:11:56 mella Exp $"
*
* who       when       what
* --------  --------   ----------------------------------------------
* mella     14 05 2004 creation 
* 
*/

#ifdef __cplusplus
extern "C" {
#endif

#define logTEXT_LEN 64
    
/*
 * Define logging definition structure 
 */
typedef struct {
        mcsLOGICAL  log;
        mcsLOGICAL  verbose;
        logLEVEL    logLevel;
        logLEVEL    verboseLevel;
        logLEVEL    actionLevel;
        mcsLOGICAL  printDate;
        mcsLOGICAL  printFileLine;
} logRULE;


/* Private functions */
extern mcsCOMPL_STAT logData(const char * msg);

#ifdef __cplusplus
};
#endif
  
#endif /*!logLOG_PRIVATE_H*/
