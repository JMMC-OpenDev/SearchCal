#ifndef sclsvrVersion_H
#define sclsvrVersion_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrVersion.h,v 1.3 2005-03-10 15:13:05 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2005/03/08 14:38:50  swmgr
 * Set version to 3.0
 *
 * Revision 1.1  2005/03/06 20:26:55  gzins
 * Created
 *
 ******************************************************************************/

/**
 * \file
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
#define sclsvrVERSION "3.1"

#ifdef __cplusplus
}
#endif


#endif /*!sclsvrVersion_H*/

/*___oOo___*/
