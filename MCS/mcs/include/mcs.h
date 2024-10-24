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
typedef float          mcsFLOAT;   /* 32 bits floating point numbers */

#define mcsLEN4     4
#define mcsLEN8     8

#define mcsLEN12    12
#define mcsLEN16    16
#define mcsLEN32    32
#define mcsLEN48    48
#define mcsLEN64    64

#define mcsLEN128   128
#define mcsLEN256   256
#define mcsLEN512   512

#define mcsLEN1024  1024
#define mcsLEN2048  2048

#define mcsLEN16384 16384
#define mcsLEN65536 65536


typedef char mcsSTRING4[mcsLEN4];
typedef char mcsSTRING8[mcsLEN8];

typedef char mcsSTRING12[mcsLEN12];
typedef char mcsSTRING16[mcsLEN16];
typedef char mcsSTRING32[mcsLEN32];
typedef char mcsSTRING48[mcsLEN48];
typedef char mcsSTRING64[mcsLEN64];

typedef char mcsSTRING128[mcsLEN128];
typedef char mcsSTRING256[mcsLEN256];
typedef char mcsSTRING512[mcsLEN512];

typedef char mcsSTRING1024[mcsLEN1024];
typedef char mcsSTRING2048[mcsLEN2048];

typedef char mcsSTRING16384[mcsLEN16384];
typedef char mcsSTRING65536[mcsLEN65536];


/*****************************************************************************************
 *                           MCS  Constants                                              *
 *****************************************************************************************/
#define mcsCMD_LEN          (mcsLEN16)    /* max. length of a command name         */
#define mcsENVNAME_LEN      (mcsLEN32)    /* max. length of an environnement value */
#define mcsFILE_LINE_LEN    (mcsLEN1024)    /* max. length of a module name          */
#define mcsMODULEID_LEN     (mcsLEN16)    /* max. length of a module name          */
#define mcsPROCNAME_LEN     (mcsLEN32)    /* max. length of a process name         */

#define mcsUNKNOWN_PROC "unknown"   /* name used for unknown processes  */
#define mcsUNKNOWN_ENV  "default"   /* name used for unknown environment*/

typedef const char* mcsCMD;       /* (mcsSTRING16)   Command name          */
typedef const char* mcsENVNAME;   /* (mcsSTRING32)   Environnement name    */
typedef const char* mcsFILE_LINE; /* (mcsSTRING1024) File/line information */
typedef const char* mcsMODULEID;  /* (mcsSTRING16)   Software module name  */
typedef const char* mcsPROCNAME;  /* (mcsSTRING32)   Process name          */

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
mcsCOMPL_STAT mcsInit(const char* procName);
const char *mcsGetProcName();
const char *mcsGetEnvName();
void mcsExit();

mcsCOMPL_STAT mcsMutexLock(mcsMUTEX* mutex);
mcsCOMPL_STAT mcsMutexUnlock(mcsMUTEX* mutex);

mcsCOMPL_STAT mcsLockGdomeMutex(void);
mcsCOMPL_STAT mcsUnlockGdomeMutex(void);

mcsUINT32 mcsGetThreadId();
void mcsGetThreadName(mcsSTRING16* name);
void mcsSetThreadInfo(mcsUINT32 id, const char* name);

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

#define mcsStrError(errno, buffer)  strerror_r(errno, buffer, sizeof (buffer) - 1)

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
