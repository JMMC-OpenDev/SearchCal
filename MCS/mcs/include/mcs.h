#ifndef MCS_H
#define MCS_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/* The following piece of code alternates the linkage type to C for all
functions declared within the braces, which is necessary to use the functions
in C++-code.
 */
#ifdef __cplusplus
extern "C"
{
#endif

/*
 * System header
 */
#include <pthread.h>

/************************************************************************
 *                           MCS  Constants                             *
 ************************************************************************/
#define mcsPROCNAME_LEN        31   /* max. length of a process name    */
#define mcsMODULEID_LEN        15   /* max. length of a module name     */
/* 15 characters + 1 byte alignement */
#define mcsENVNAME_LEN         20   /* max. length of an environnement  */
/* 15 characters + 1 byte alignement*/
#define mcsCMD_LEN             15   /* max. length of a command name    */
#define mcsUNKNOWN_PROC "unknown"   /* name used for unknown processes  */
#define mcsUNKNOWN_ENV  "default"   /* name used for unknown environment*/

/************************************************************************
 *                          MCS   Data  Types                           *
 ************************************************************************/
typedef char           mcsINT8;    /*  8 bits integers          */
typedef unsigned char  mcsUINT8;   /*  8 bits unsigned integers */
typedef short          mcsINT16;   /* 16 bits integers          */
typedef unsigned short mcsUINT16;  /* 16 bits unsigned integers */
typedef int            mcsINT32;   /* 32 bits integers          */
typedef unsigned int   mcsUINT32;  /* 32 bits unsigned integers */
typedef long           mcsINT64;   /* 64 bits integers          */
typedef unsigned long  mcsUINT64;  /* 64 bits unsigned integers */
typedef double         mcsDOUBLE;  /* 64 bits floating point numbers      */
typedef long double    mcsLDOUBLE; /* 80/128 bits floating point numbers */
typedef float          mcsFLOAT;   /* 32 bits floating point numbers */

typedef unsigned char mcsBYTES4[4];
typedef unsigned char mcsBYTES8[8];
typedef unsigned char mcsBYTES12[12];
typedef unsigned char mcsBYTES16[16];
typedef unsigned char mcsBYTES20[20];
typedef unsigned char mcsBYTES32[32];
typedef unsigned char mcsBYTES48[48];
typedef unsigned char mcsBYTES64[64];
typedef unsigned char mcsBYTES80[80];
typedef unsigned char mcsBYTES128[128];
typedef unsigned char mcsBYTES256[256];
typedef unsigned char mcsBYTES512[512];
typedef unsigned char mcsBYTES1024[1024];

typedef char mcsSTRING4[4];
typedef char mcsSTRING8[8];
typedef char mcsSTRING12[12];
typedef char mcsSTRING16[16];
typedef char mcsSTRING20[20];
typedef char mcsSTRING32[32];
typedef char mcsSTRING48[48];
typedef char mcsSTRING64[64];
typedef char mcsSTRING80[80];
typedef char mcsSTRING128[128];
typedef char mcsSTRING256[256];
typedef char mcsSTRING512[512];
typedef char mcsSTRING1024[1024];
typedef char mcsSTRING2048[2048];
typedef char mcsSTRING16384[16384];
typedef char mcsSTRING65536[65536];

typedef char mcsPROCNAME[mcsPROCNAME_LEN + 1]; /* Process name          */
typedef char mcsENVNAME[mcsENVNAME_LEN + 1]; /* Environnement name    */
typedef char mcsMODULEID[mcsMODULEID_LEN + 1]; /* Software module name  */
typedef char mcsFILE_LINE[64]; /* File/line information */
typedef char mcsCMD[mcsCMD_LEN + 1]; /* Command name          */

#define mcsNULL_CMD  ""

/*
 *   Definition of logical
 */
typedef enum
{
    mcsFALSE = 0, /* False Logical */
    mcsTRUE = 1 /* True Logical  */
} mcsLOGICAL;

/*
 *   Definition of the routine completion status
 */
typedef enum
{
    mcsFAILURE = -1,
    mcsSUCCESS = 0
} mcsCOMPL_STAT; /* Completion status returned by subroutines */

/**
 * Definition of complex type
 */
typedef struct
{
    mcsDOUBLE re; /**<< real part */
    mcsDOUBLE im; /**<< imaginary part */
} mcsCOMPLEX;

/**
 * Definition of mutex type
 */
typedef pthread_mutex_t mcsMUTEX; /**< mutex type. */
/** mcsMUTEX static initializer with error checking */
#define MCS_MUTEX_STATIC_INITIALIZER PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP
/** mcsMUTEX static initializer supporting recursive lock/unlock (requires _GNU_SOURCE in CFLAGS) */
#define MCS_RECURSIVE_MUTEX_INITIALIZER PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP

/*
 * Public functions
 */
mcsCOMPL_STAT mcsInit(const mcsPROCNAME procName);
const char *mcsGetProcName();
const char *mcsGetEnvName();
void mcsExit();

mcsCOMPL_STAT mcsMutexLock(mcsMUTEX* mutex);
mcsCOMPL_STAT mcsMutexUnlock(mcsMUTEX* mutex);

mcsCOMPL_STAT mcsLockGdomeMutex(void);
mcsCOMPL_STAT mcsUnlockGdomeMutex(void);

mcsUINT32 mcsGetThreadId();
void mcsGetThreadName(mcsSTRING16* name);
void mcsSetThreadInfo(mcsUINT32 id, const mcsSTRING16 name);

mcsCOMPL_STAT mcsGetEnv_r(const char *name, char *buf, const int buflen);

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

#define mcsStrError(errno, buffer)  strerror_r(errno, buffer, sizeof(buffer) - 1)

#define IS_NULL(value)      ((value) == NULL)
#define IS_NOT_NULL(value)  ((value) != NULL)

#define IS_FALSE(value)     ((value) == mcsFALSE)
#define IS_TRUE(value)      ((value) == mcsTRUE)

#define IS_STR_EMPTY(value) (IS_NULL(value) || (strlen((value)) == 0))

/**
 * Useful macro to return mcsSUCCESS if the given status is mcsFAILURE
 */
#define SUCCESS(status)     \
if (status == mcsFAILURE)   \
{                           \
    return mcsSUCCESS;      \
}

/**
 * Useful macro to execute code in doSuccess arg and return mcsSUCCESS
 * if the given status is mcsFAILURE
 */
#define SUCCESS_DO(status, doSuccess)   \
if (status == mcsFAILURE)               \
{                                       \
    { doSuccess; }                      \
    return mcsSUCCESS;                  \
}

/**
 * Useful macro to execute code in doSuccess arg and return mcsSUCCESS
 * if the given status is mcsFALSE
 */
#define SUCCESS_FALSE_DO(status, doSuccess) \
if (status == mcsFALSE)                     \
{                                           \
    { doSuccess; }                          \
    return mcsSUCCESS;                      \
}

/**
 * Useful macro to return mcsSUCCESS if the given condition is true
 */
#define SUCCESS_COND(condition) \
if (condition)                  \
{                               \
    return mcsSUCCESS;          \
}

/**
 * Useful macro to execute code in doSuccess arg and return mcsSUCCESS
 * if the given condition is true
 */
#define SUCCESS_COND_DO(condition, doSuccess)   \
if (condition)                                  \
{                                               \
    { doSuccess; }                              \
    return mcsSUCCESS;                          \
}

/**
 * Useful macro to return mcsFAILURE if the given status is mcsFAILURE
 */
#define FAIL(status)        \
if (status == mcsFAILURE)   \
{                           \
    return mcsFAILURE;      \
}

/**
 * Useful macro to execute code in doFail arg and return mcsFAILURE
 * if the given status is mcsFAILURE
 */
#define FAIL_DO(status, doFail) \
if (status == mcsFAILURE)       \
{                               \
    { doFail; }                 \
    return mcsFAILURE;          \
}

/**
 * Useful macro to return mcsFAILURE if the given status is mcsFALSE
 */
#define FAIL_FALSE(status)  \
if (status == mcsFALSE)     \
{                           \
    return mcsFAILURE;      \
}

/**
 * Useful macro to execute code in doFail arg and return mcsFAILURE
 * if the given status is mcsFALSE
 */
#define FAIL_FALSE_DO(status, doFail)   \
if (status == mcsFALSE)                 \
{                                       \
    { doFail; }                         \
    return mcsFAILURE;                  \
}

/**
 * Useful macro to return mcsFAILURE if the given value is NULL
 */
#define FAIL_NULL(value)    \
if (value == NULL)          \
{                           \
    return mcsFAILURE;      \
}

/**
 * Useful macro to execute code in doFail arg and return mcsFAILURE
 * if the given value is NULL
 */
#define FAIL_NULL_DO(value, doFail) \
if (value == NULL)                  \
{                                   \
    { doFail; }                     \
    return mcsFAILURE;              \
}

/**
 * Useful macro to return mcsFAILURE if the given condition is true
 */
#define FAIL_COND(condition)    \
if (condition)                  \
{                               \
    return mcsFAILURE;          \
}

/**
 * Useful macro to execute code in doFail arg and return mcsFAILURE
 * if the given condition is true
 */
#define FAIL_COND_DO(condition, doFail) \
if (condition)                          \
{                                       \
    { doFail; }                         \
    return mcsFAILURE;                  \
}

/**
 * Useful macro to return NULL
 * if the given status is mcsFAILURE
 */
#define NULL_(status) \
if (status == mcsFAILURE)       \
{                               \
    return NULL;                \
}

/**
 * Useful macro to execute code in doFail arg and return NULL
 * if the given status is mcsFAILURE
 */
#define NULL_DO(status, doFail) \
if (status == mcsFAILURE)       \
{                               \
    { doFail; }                 \
    return NULL;                \
}

#ifdef __cplusplus
}
#endif

#endif /*!MCS_H*/
