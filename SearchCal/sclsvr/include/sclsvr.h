#ifndef sclsvr_H
#define sclsvr_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvr.h,v 1.9 2005-10-26 11:27:24 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.8  2005/06/20 14:41:25  scetre
 * Removed include of removed filter
 *
 * Revision 1.7  2005/06/01 14:18:54  scetre
 * Added filters and filter list objects.
 * Changed logExtDbg to logTrace
 *
 * Revision 1.6  2005/03/06 20:27:33  gzins
 * Added sclsvrVersion.h
 *
 * Revision 1.5  2005/02/07 15:00:03  gzins
 * Added CVS log as modification history
 *
 * scetre    15-Sep-2004  Created
 *
 ******************************************************************************/

/**
 * @file
 * Header file of the sclsvr module.
 */


#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif


/*
 * Local header
 */
#include "sclsvrCALIBRATOR.h"
#include "sclsvrCALIBRATOR_LIST.h"
#include "sclsvrErrors.h"
#include "sclsvrGETCAL_CMD.h"
#include "sclsvrGETSTAR_CMD.h"
#include "sclsvrSERVER.h"
#include "sclsvrVersion.h"
#include "sclsvrVISIBILITY_FILTER.h"


#endif /*!sclsvr_H*/

/*___oOo___*/
