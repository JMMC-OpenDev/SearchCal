#ifndef sclwsPrivate_H
#define sclwsPrivate_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclwsPrivate.h,v 1.2 2007-10-31 11:44:22 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2006/12/21 15:19:19  ccmgr
 * Fix directory structure and add additional files
 *
 ******************************************************************************/

/**
 * @file
 * sclws private header file.
 */

/*
 * Constants definition
 */

/* Module name */
#define MODULE_ID "sclws"

/** Usefull macro to return error when executing SOAP service. The error message
 * is get from error stack */
#define sclwsReturnSoapError()                                       \
    soap_fault(p_soap);                                              \
    strncpy(sclwsSoapErrMsg, errUserGet(), sizeof(sclwsSoapErrMsg)); \
    p_soap->fault->faultstring = sclwsSoapErrMsg;                    \
    errCloseStack();                                                 \
    return SOAP_ERR;

#endif /*!sclwsPrivate_H*/

/*___oOo___*/
