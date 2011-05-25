#ifndef sclwsPrivate_H
#define sclwsPrivate_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
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
    soap_fault(soapContext);                                         \
    strncpy(sclwsSoapErrMsg, errUserGet(), sizeof(sclwsSoapErrMsg)); \
    soapContext->fault->faultstring = sclwsSoapErrMsg;               \
    errCloseStack();                                                 \
    return SOAP_ERR;

#endif /*!sclwsPrivate_H*/

/*___oOo___*/
