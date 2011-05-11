#ifndef evhStates_H
#define evhStates_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Declaration of state/sub-state codes and names
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/*
 * MCS header
 */
#include "mcs.h"


/*
 * Class declaration
 */

/**
 * This include file contains standard definitions for state/sub-state codes and
 * names.
 */

/**
 * State codes
 */
#define    evhSTATE_UNKNOWN    0
#define    evhSTATE_OFF        1
#define    evhSTATE_STANDBY    2
#define    evhSTATE_ONLINE     3

/**
 * Sub-state codes
 */
#define    evhSUBSTATE_UNKNOWN 0
#define    evhSUBSTATE_ERROR   1
#define    evhSUBSTATE_IDLE    2
#define    evhSUBSTATE_WAITING 3
#define    evhSUBSTATE_BUSY    4

/**
 * State names
 */
#define    evhSTATE_STR_UNKNOWN    "UNKNOWN"
#define    evhSTATE_STR_OFF        "OFF"
#define    evhSTATE_STR_STANDBY    "STANDBY"
#define    evhSTATE_STR_ONLINE     "ONLINE"

/**
 * Sub-state names
 */
#define    evhSUBSTATE_STR_UNKNOWN "UNKNOWN"
#define    evhSUBSTATE_STR_ERROR   "ERROR"
#define    evhSUBSTATE_STR_IDLE    "IDLE"
#define    evhSUBSTATE_STR_WAITING "WAITING"
#define    evhSUBSTATE_STR_BUSY    "BUSY"

#endif /*!evhStates_H*/

/*___oOo___*/
