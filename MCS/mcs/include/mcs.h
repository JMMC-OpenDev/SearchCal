/*************************************************************************
* JMMC project
*
* "@(#) $Id: mcs.h,v 1.8 2004-09-27 10:13:06 gzins Exp $"
*
* mcs.h  -  MCS/Common Definitions - Interface File
*
* who        when      what
* ---------  --------  ----------------------------------------------
* G.Mella    07/05/04  Preliminary version based on MCS from VLT/ESO
* gzins      11/05/04  Add RCS Id and removed unused definitions
*
****************************************************************************/

#ifndef MCS_H
#define MCS_H

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************
 *                           MCS  Constants                             *
 ************************************************************************/
#define mcsPROCNAME_LEN        19   /* max. length of a process name      */
#define mcsMODULEID_LEN         7   /* max. length of a module name       */
                                    /* 6 characters + 1 byte alignement   */
#define mcsCMD_LEN 8                /* max. length of a command name */
#define mcsUNKNOWN_PROC "unknown"   /* name used for unknown processes */
#define mcsFALSE                0   /* False Logical */
#define mcsTRUE                 1   /* True Logical */

/************************************************************************
 *                          MCS   Data  Types                           *
 ************************************************************************/
typedef char               mcsINT8;      /*  8 bits integers           */
typedef unsigned char      mcsUINT8;     /*  8 bits unsigned integers  */
typedef short              mcsINT16;     /* 16 bits integers           */
typedef unsigned short     mcsUINT16;    /* 16 bits unsigned integers  */
typedef int                mcsINT32;     /* 32 bits integers           */
typedef unsigned int       mcsUINT32;    /* 32 bits unsigned integers  */
typedef unsigned char      mcsLOGICAL;   /* logical (rtLogical)        */
typedef double             mcsDOUBLE;    
typedef float              mcsFLOAT;

typedef unsigned char      mcsBYTES4[4]; 
typedef unsigned char      mcsBYTES8[8]; 
typedef unsigned char      mcsBYTES12[12]; 
typedef unsigned char      mcsBYTES16[16]; 
typedef unsigned char      mcsBYTES20[20]; 
typedef unsigned char      mcsBYTES32[32]; 
typedef unsigned char      mcsBYTES48[48]; 
typedef unsigned char      mcsBYTES64[64]; 
typedef unsigned char      mcsBYTES80[80]; 
typedef unsigned char      mcsBYTES128[128]; 
typedef unsigned char      mcsBYTES256[256]; 

typedef char               mcsSTRING4[4]; 
typedef char               mcsSTRING8[8]; 
typedef char               mcsSTRING12[12]; 
typedef char               mcsSTRING16[16]; 
typedef char               mcsSTRING20[20]; 
typedef char               mcsSTRING32[32]; 
typedef char               mcsSTRING48[48]; 
typedef char               mcsSTRING64[64]; 
typedef char               mcsSTRING80[80]; 
typedef char               mcsSTRING128[128]; 
typedef char               mcsSTRING256[256];

typedef char mcsPROCNAME[mcsPROCNAME_LEN+1];      /* Process name           */
typedef char mcsMODULEID[mcsMODULEID_LEN+1];      /* Software module name   */
typedef char mcsFILE_LINE[64];                    /* File/line information  */
typedef char mcsCMD[mcsCMD_LEN+1];                /* Command name */

#define mcsNULL_CMD  ""

/*
 *   Definition of the routine completion status
 */
typedef enum 
{
    FAILURE = 1,
    SUCCESS 
} mcsCOMPL_STAT;       /* Completion status returned by subroutines */

/*
 * Public functions
 */
mcsCOMPL_STAT mcsInit(const mcsPROCNAME  procName);
const char *mcsGetProcName();
void mcsExit();

/*
 * Convenience macros
 */
#define mcsMAX(a,b)  ((a)>=(b)?(a):(b))
#define mcsMIN(a,b)  ((a)<=(b)?(a):(b))

#ifndef __FILE_LINE__
#define mcsIToStr(a) #a
#define mcsIToStr2(a) mcsIToStr(a) 
#define __FILE_LINE__ __FILE__ ":" mcsIToStr2(__LINE__)
#endif /*!__FILE_LINE__*/

#ifdef __cplusplus
}
#endif

#endif /*!MCS_H*/
