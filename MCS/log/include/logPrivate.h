#ifndef logLOG_PRIVATE_H
#define logLOG_PRIVATE_H
/*******************************************************************************
*  JMMC Project
*  
*  "@(#) $Id: logPrivate.h,v 1.4 2004-06-21 16:52:08 gzins Exp $"
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

#ifdef __cplusplus
};
#endif
  
#endif /*!logLOG_PRIVATE_H*/
