#ifndef miscDynStr_H
#define miscDynStr_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: miscDynStr.h,v 1.3 2004-07-23 14:29:59 lafrasse Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* lafrasse  13-Jul-2004  Created
* lafrasse  22-Jul-2004  Removed a 'endcode' doxygen tag in excess
* lafrasse  23-Jul-2004  Added miscDynStrGetStringFromTo parameter refinments
*
*
*******************************************************************************/

/**
 * \file
 * This header contains all the miscDynStr functions declarations.
 *
 * Those function declarations are isolated of the misc.h declarations due to
 * their number.
 *
 * \n \b Code \b Example:\n
 * \n A quick'n dirty main using a Dynamic Buffer.
 * \code
 * #include "miscDynStr.h"
 *
 * int main (int argc, char *argv[])
 * {
 *     miscDYN_BUF dynBuf;
 *     char *tmp = "bytes to";
 *     miscDynStrAppendString(&dynBuf, tmp);
 *     tmp = " append...";
 *     miscDynStrAppendString(&dynBuf, tmp);
 *     printf("DynBuf contains \"%s\".\n", miscDynBufGetBufferPointer(&dynBuf));
 *     miscDynBufDestroy(&dynBuf);
 *     exit (EXIT_SUCCESS);
 * }
 * \endcode
 */



/* 
 * Local Headers
 */
#include "miscDynBuf.h"


/* The following piece of code alternates the linkage type to C for all 
functions declared within the braces, which is necessary to use the 
functions in C++ code.
*/
#ifdef __cplusplus
extern "C" {
#endif



/*
 * Pubic functions declaration
 */

mcsCOMPL_STAT miscDynStrGetStringFromTo     (miscDYN_BUF       *dynBuf,
                                             char              *str,
                                             const mcsUINT32   from,
                                             const mcsUINT32   to);

mcsCOMPL_STAT miscDynStrReplaceStringFromTo (miscDYN_BUF       *dynBuf,
                                             char              *str,
                                             const mcsUINT32   from,
                                             const mcsUINT32   to);

mcsCOMPL_STAT miscDynStrAppendString        (miscDYN_BUF       *dynBuf,
                                             char              *str);

mcsCOMPL_STAT miscDynStrInsertStringAt      (miscDYN_BUF       *dynBuf,
                                             char              *str,
                                             const mcsUINT32   position);

#ifdef __cplusplus
}
#endif

#endif /*!miscDynStr_H*/

/*___oOo___*/
