#ifndef sdb_H
#define sdb_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sdb.h,v 1.3 2005-12-22 14:10:35 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
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
 

/*
 * Convenience macros
 */
#define sdbInitAction() \
        (sdbENTRY::Init())

#define sdbDestroyAction() \
        (sdbENTRY::Destroy())

#define sdbWaitAction(message, lastMessage) \
        (sdbENTRY::Wait(message, lastMessage))
 
#define sdbWriteAction(message, lastMessage) \
        (sdbENTRY::Write(message, lastMessage))


#endif /*!sdb_H*/

/*___oOo___*/
