#ifndef misc_H
#define misc_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: misc.h,v 1.4 2004-06-17 15:17:58 lafrasse Exp $"
*
* who       when		 what
* --------  -----------	 -------------------------------------------------------
* gzins     16-Jun-2004  created
*
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
char *miscGetExtension(char *fullPath);

void miscGetUtcTimeStr(mcsBYTES32 localTime, mcsINT32 precision);
void miscGetLocalTimeStr(mcsBYTES32 localTime, mcsINT32 precision);

void miscStripQuotes(char *string);
void miscStrToUpper(char *string);

#ifdef __cplusplus
}
#endif


#endif /*!misc_H*/


/*___oOo___*/
