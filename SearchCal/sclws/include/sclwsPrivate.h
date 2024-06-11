#ifndef sclwsPrivate_H
#define sclwsPrivate_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * sclws private header file.
 */

/* Server port number configuration */
#define SCLWS_PORTNUMBER_ENVVAR_NAME "SCLWS_PORT_NB"

/* Retrieve current server port */
mcsUINT16 sclwsGetServerPortNumber(void);

/**
 * Free server instances (GC like)
 * \param forceCleanup flag to force cleanup
 * \return mcsTRUE if any resource was freed
 */
mcsLOGICAL sclwsFreeServerList(const bool forceCleanup);

/**
 * Get thread statistics
 */
void sclwsThreadStats(mcsUINT32 *threadCreated, mcsUINT32 *threadJoined);

/**
 * GetCal statistics
 */
void sclwsGetCalStats(mcsUINT32 *serverCreated, mcsUINT32 *serverDeleted, mcsUINT32 *serverCancelled, mcsUINT32 *serverFailed);

/**
 * GetStar statistics
 */
void sclwsGetStarStats(mcsUINT32 *serverCreated, mcsUINT32 *serverDeleted, mcsUINT32 *serverFailed);

/*
 * Constants definition
 */

/* Module name */
#define MODULE_ID "sclws"

/** Usefull macro to return error when executing SOAP service. The error message
 * is get from error stack */
#define sclwsDefineSoapError(soapContext)                            \
{                                                                    \
    soap_fault(soapContext);                                         \
    char* sclwsSoapErrMsg = soap_strdup(soapContext, errUserGet());  \
    soapContext->fault->faultstring = sclwsSoapErrMsg;               \
    errCloseStack();                                                 \
}

/** Usefull macro to return error when executing SOAP service. The error message
 * is get from error stack */
#define sclwsReturnSoapError(soapContext)                            \
{                                                                    \
    sclwsDefineSoapError(soapContext);                               \
    return SOAP_ERR;                                                 \
}

#endif /*!sclwsPrivate_H*/

/*___oOo___*/
