#ifndef misc_H
#define misc_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: misc.h,v 1.7 2004-06-22 07:54:12 gzins Exp $"
*
* who       when		 what
* --------  -----------	 -------------------------------------------------------
* gzins     16-Jun-2004  created
* lafrasse  17-Jun-2004  added miscGetLocalTimeStr
*                        added miscStripQuotes
*                        added miscStrToUpper
* lafrasse  18-Jun-2004  added miscGetExtension
*                        added miscYankExtension
*
*******************************************************************************/
 
/* The following piece of code alternates the linkage type to C for all 
functions declared within the braces, which is necessary to use the 
functions in C++-code.
*/

#ifdef __cplusplus
extern C {
#endif

/* Public functions */
char *miscGetFileName(char *fullPath);
char *miscGetExtension(char *fileName);
void miscYankExtension(char *fileName, char *extension);

void miscGetUtcTimeStr(mcsBYTES32 localTime, mcsINT32 precision);
void miscGetLocalTimeStr(mcsBYTES32 localTime, mcsINT32 precision);

void miscStripQuotes(char *string);
void miscStrToUpper(char *string);

#ifdef __cplusplus
}
#endif


#endif /*!misc_H*/


/*___oOo___*/
