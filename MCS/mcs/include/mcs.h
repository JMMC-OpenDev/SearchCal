#ifndef MCS_H
#define MCS_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: mcs.h,v 1.20 2005-02-16 09:22:42 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.19  2005/02/15 12:36:53  gzins
 * Removed SUCCESS and FAILURE definitions
 *
 * Revision 1.18  2005/01/29 07:27:29  gzins
 *  Changed (again) command and module max lenght to 15
 *  Changed (again) process name max lenght to 31
 *
 * Revision 1.17  2005/01/28 23:37:35  gzins
 * Changed (back) command name max length to 8
 * Changed (back) module name max length to 7
 * Changed (back) process name max length to 19
 *
 * Revision 1.16  2005/01/28 18:50:22  gzins
 * Changed command and module max lenght to 15
 * Changed process name max lenght to 31
 *
 * mella     07-May-2004  Preliminary version based on MCS from VLT/ESO
 * gzins     11-May-2004  Add RCS Id and removed unused definitions
 * lafrasse  01-Dec-2004  Added mcsUNKNOWN_ENV
 * gzins     09-Dec-2004  Defined mcsLOGICAL as an enumerate
 * lafrasse  14-Dec-2004  Increased environment name length constant
 *                        mcsENVNAME_LEN from 7 to 15
 * gzins     06-Jan-2005  Changed FAILURE to mcsFAILURE and SUCCESS to
 *                        mcsSUCCESS
 *                        Changed value for mcsFAILURE to -1
 *                        Added FAILURE and SUCCESS macros for backward
 *                        compatibility
 * lafrasse  26-Jan-2005  Changed mcsUNKNOWN_ENV from "none" to "default"
 *
 ******************************************************************************/

/* The following piece of code alternates the linkage type to C for all 
functions declared within the braces, which is necessary to use the functions
in C++-code.
*/

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************
 *                           MCS  Constants                             *
 ************************************************************************/
#define mcsPROCNAME_LEN        31   /* max. length of a process name    */
#define mcsMODULEID_LEN        15   /* max. length of a module name     */
                                    /* 15 characters + 1 byte alignement */
#define mcsENVNAME_LEN         15   /* max. length of an environnement  */
                                    /* 15 characters + 1 byte alignement*/
#define mcsCMD_LEN             15   /* max. length of a command name    */
#define mcsUNKNOWN_PROC "unknown"   /* name used for unknown processes  */
#define mcsUNKNOWN_ENV  "default"   /* name used for unknown environment*/

/************************************************************************
 *                          MCS   Data  Types                           *
 ************************************************************************/
typedef char               mcsINT8;       /*  8 bits integers           */
typedef unsigned char      mcsUINT8;      /*  8 bits unsigned integers  */
typedef short              mcsINT16;      /* 16 bits integers           */
typedef unsigned short     mcsUINT16;     /* 16 bits unsigned integers  */
typedef int                mcsINT32;      /* 32 bits integers           */
typedef unsigned int       mcsUINT32;     /* 32 bits unsigned integers  */
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
typedef unsigned char      mcsBYTES512[512]; 
typedef unsigned char      mcsBYTES1024[1024]; 

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
typedef char               mcsSTRING512[512];
typedef char               mcsSTRING1024[1024];

typedef char mcsPROCNAME[mcsPROCNAME_LEN+1];   /* Process name          */
typedef char mcsENVNAME[mcsENVNAME_LEN+1];     /* Environnement name    */
typedef char mcsMODULEID[mcsMODULEID_LEN+1];   /* Software module name  */
typedef char mcsFILE_LINE[64];                 /* File/line information */
typedef char mcsCMD[mcsCMD_LEN+1];             /* Command name          */

#define mcsNULL_CMD  ""

/*
 *   Definition of logical  
 */
typedef enum 
{
    mcsFALSE = 0,   /* False Logical */
    mcsTRUE  = 1    /* True Logical  */
} mcsLOGICAL;     

/*
 *   Definition of the routine completion status
 */
typedef enum 
{
    mcsFAILURE = -1,
    mcsSUCCESS 
} mcsCOMPL_STAT;       /* Completion status returned by subroutines */

/*
 * Public functions
 */
mcsCOMPL_STAT mcsInit(const mcsPROCNAME  procName);
const char *mcsGetProcName();
const char *mcsGetEnvName();
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
