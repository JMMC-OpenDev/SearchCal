#ifndef sclsvrVersion_H
#define sclsvrVersion_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrVersion.h,v 1.9 2006-02-22 11:22:16 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.8  2006/02/20 16:33:47  scetre
 * Changed version 3.3 to 3.3.1
 *
 * Revision 1.7  2006/02/20 10:19:50  scetre
 * Changed revision number from 3.3-beta to 3.3
 *
 * Revision 1.6  2006/01/26 13:01:45  scetre
 * Changed version number from 3.2 to 3.3-beta
 *
 * Revision 1.5  2005/10/26 11:27:24  lafrasse
 * Code review
 *
 * Revision 1.4  2005/09/13 11:08:52  scetre
 * Changed version from 3.1 to 3.2
 *
 * Revision 1.3  2005/03/10 15:13:05  scetre
 * changed version 3.0 -> 3.1
 *
 * Revision 1.2  2005/03/08 14:38:50  swmgr
 * Set version to 3.0
 *
 * Revision 1.1  2005/03/06 20:26:55  gzins
 * Created
 *
 ******************************************************************************/

/**
 * @file
 * Definition of the software version.
 */

/* The following piece of code alternates the linkage type to C for all 
functions declared within the braces, which is necessary to use the 
functions in C++-code.
*/
#ifdef __cplusplus
extern "C" {
#endif

// Software version 
#define sclsvrVERSION "3.3.2"

#ifdef __cplusplus
}
#endif


#endif /*!sclsvrVersion_H*/

/*___oOo___*/
