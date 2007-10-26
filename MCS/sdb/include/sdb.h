#ifndef sdb_H
#define sdb_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sdb.h,v 1.5 2007-10-26 13:25:26 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.4  2006/12/21 15:03:08  lafrasse
 * Moved from static-based design to instance-based design.
 *
 * Revision 1.3  2005/12/22 14:10:35  lafrasse
 * Added a way to release all the created semaphores used by sdbENTRY
 *
 * Revision 1.2  2005/12/20 13:52:34  lafrasse
 * Added preliminary support for INTRA-process action log
 *
 * Revision 1.1  2005/06/02 13:09:28  sccmgr
 * Fix directory structure and add additional files
 *
 ******************************************************************************/

/**
 * @file
 * sdb general header file.
 */


/*
 * MCS headers
 */
#include "log.h"
 

/*
 * Local headers
 */
#include "sdbENTRY.h"
#include "sdbSYNC_ENTRY.h"
 

#endif /*!sdb_H*/

/*___oOo___*/
