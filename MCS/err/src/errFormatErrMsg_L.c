/*******************************************************************************
* JMMC project
*
* who       when		 what
* --------  -----------	 -------------------------------------------------------
* berezne   02-Jun-2004  created
* gzins     17-Jun-2004  completed implementation
*
*-----------------------------------------------------------------------------*/

static char *rcsId="@(#) $Id: errFormatErrMsg_L.c,v 1.2 2004-08-23 13:35:49 gzins Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

/* 
 * System Headers
 */

#include <string.h>
#include <ctype.h>

#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <sys/stat.h>
#include <unistd.h>

#include <libgdome/gdome.h>

#include <stdlib.h>
/*
 * Common Software Headers
 */
#include "mcs.h"
#include "log.h"

/*
 * Local Headers
 */
#include "err.h"
#include "errPrivate.h"

/* Global variable */
errERROR errGlobalStack;

/*
 * Declaration of local functions
 */
static char *errGetErrProp (const char *moduleId,
                            mcsINT32 errorId, 
                            const char *propName);

static char *errGetErrProp(const char *moduleId, int id, const char * propName)
{
    logExtDbg("errGetErrProp()"); 

    GdomeDOMImplementation *domimpl;
    GdomeDocument *doc;
    GdomeElement *root, *el, *elerr;
    GdomeNodeList *childs, *errs, *texts;
    GdomeException exc;
    GdomeDOMString *name, *value;
    unsigned long i, j, nbChilds, nbErrs;
    mcsSTRING16 idSearch;
    static mcsSTRING256 propValue;
    mcsSTRING256 errFileName;
    char *envVar;
    mcsLOGICAL errFileFound;
    struct stat statBuf;

    sprintf(idSearch, "%d", id);

    /* Get a DOMImplementation reference */
    domimpl = gdome_di_mkref ();

    /* Look for the error definition file */
    errFileFound= mcsFALSE;
    /* In ../errors */
    /* Create error file name */
    sprintf(errFileName, "../errors/%sErrors.xml", moduleId);

    /* Test if file exists */
    if (stat(errFileName, &statBuf) == 0)
    {
        errFileFound = mcsTRUE;
    }

    /* In $INTROOT */
    /* If not found in $INTROOT, look in $MCSROOT */
    if (errFileFound == mcsFALSE)
    {
        /* Get INTROOT environment variable value */
        envVar = getenv("INTROOT");
        if (envVar != NULL)
        {
            /* Create error file name */
            sprintf(errFileName, "%s/errors/%sErrors.xml", envVar, moduleId);

            /* Test if file exists */
            if (stat(errFileName, &statBuf) == 0)
            {
                errFileFound = mcsTRUE;
            }
        }
        else
        {
            logWarning ("Environment variable INTROOT not defined");
        }
    }
    
    /* If not found in $INTROOT, look in $MCSROOT */
    if (errFileFound == mcsFALSE)
    {
        /* Get MCSROOT environment variable value */
        envVar = getenv("MCSROOT");
        if (envVar != NULL)
        {
            struct stat statBuf;

            /* Create error file name */
            sprintf(errFileName, "%s/errors/%sErrors.xml", envVar, moduleId);

            /* Test if file exists */
            if (stat(errFileName, &statBuf) == 0)
            {
                errFileFound = 0;
            }
        }
        else
        {
            logWarning ("Environment variable MCSROOT not defined");
        }

    }
    
    /* If error definition file has not been found */
    if (errFileFound == mcsFALSE)
    {
        logWarning ("Error definition file '%sErrors.xml' can not be found",
                    moduleId);
        return NULL;
    }

    /* Load a new document from a file */
    doc = gdome_di_createDocFromURI(domimpl, errFileName, GDOME_LOAD_PARSING,
                                    &exc);
    if (doc == NULL)
    {
        logWarning ("DOMImplementation.createDocFromURI: failed. Exception #%d",
                    exc);
        return NULL;
    }

    /* Get reference to the root element of the document */
    root = gdome_doc_documentElement (doc, &exc);
    if (root == NULL) {
        logWarning ("Document.documentElement: NULL. Exception #%d\n", exc);
        return NULL;
    }

    /* Get the reference to the childrens NodeList of the root element */
    childs = gdome_el_childNodes (root, &exc);
    if (childs == NULL)
    {
        logWarning ("Element.childNodes: NULL. Exception #%d\n", exc);
        return NULL;
    }

    /* Search the attribute id="**" in the child elements
     * When it is found, le value of NODEx1 is print to the screen */
    nbChilds = gdome_nl_length (childs, &exc);

    for (i = 0; i < nbChilds; i++)
    {
        el = (GdomeElement *)gdome_nl_item (childs, i, &exc);
        if (el == NULL)
        {
            logWarning ("NodeList.item(%d): NULL. Exception #%d", (int)i, exc);
            return NULL;
        }
        if (gdome_el_nodeType (el, &exc) == GDOME_ELEMENT_NODE)
        {
            name = gdome_str_mkref ("id");
            value = gdome_el_getAttribute(el, name, &exc);
            if (exc)
            {
                logWarning ("Element.getAttribute: failed. Exception #%d", exc);
                return NULL;
            }
            if (strcmp(value->str, idSearch)==0)
            {
                name = gdome_str_mkref (propName);
                errs = gdome_el_getElementsByTagName (el, name, &exc);
                if (errs == NULL)
                {
                    logWarning ("Element.childNodes: NULL. Exception #%d", exc);
                    return NULL;
                }
                nbErrs = gdome_nl_length (errs, &exc);
                for (j = 0; j < nbErrs; j++)
                {
                    elerr = (GdomeElement *)gdome_nl_item (errs, j, &exc);
                    texts = gdome_el_childNodes (elerr, &exc);
                    if (texts == NULL)
                    {
                        logWarning ("Element.childNodes: NULL. Exception #%d",
                                    exc);
                        return NULL;
                    }
                    /*This element must contain one child : TextElement*/
                    elerr = (GdomeElement *)gdome_nl_item (texts, j, &exc);
                    value = gdome_el_nodeValue(elerr, &exc);
                    if (value == NULL)
                    {
                        logWarning ("Element.nodeValue: NULL. Exception #%d",
                                    exc);
                        return NULL;
                    }
                    /* Check property value length */
                    if (strlen(value->str) >= sizeof(mcsSTRING256))
                    {
                        logWarning ("Size of property value too long");
                        return NULL;
                    }

                    strncpy(propValue, value->str, sizeof(mcsSTRING256)-1);
                }
            }
        }
        gdome_el_unref (el, &exc);
    }

    /* Free the document structure and the DOMImplementation */
    gdome_di_freeDoc (domimpl, doc, &exc);
    gdome_di_unref (domimpl, &exc);

    /* Return property value */
    return propValue;
}

/**
 * Format the error message
 *
 * This function retreives in the error definition file of the module, the
 * error format and then format the error message accordingly
 * 
 * \param moduleId module identifier
 * \param errorId error number
 */
mcsCOMPL_STAT errFormatErrMsg(const mcsMODULEID moduleId,
                              mcsINT32          errorId,
                              mcsBYTES256       errMsg,
                              va_list           ap)
{
    char         *propValue;
    mcsSTRING256 format;

    /* Get error format */
    propValue = errGetErrProp(moduleId, errorId, "errFormat");
    if (propValue == NULL)
    {
        return FAILURE;
    }
    strcpy(format, propValue);

    /* Fill the error message */
    va_start(argPtr,errorId);
    vsprintf(runTimePar, format, argPtr);
    va_end(argPtr);

    return (SUCCESS);
}
/**
 * Add a new error to the error stack
 *
 * \param error error structure.
 * \param moduleId module identifier
 * \param errorId error number
 * \param fileLine file name and line number from where the error has been added
 */
mcsCOMPL_STAT errAddInLocalStack(errERROR          *error,
                                 const mcsMODULEID moduleId,
                                 const char        *fileLine,
                                 mcsINT32          errorId, ...)
{
    va_list      argPtr;
    mcsSTRING64  timeStamp;
    char         severity;
    mcsSTRING256 format;
    mcsSTRING256 runTimePar;
    char         *propValue;

    logExtDbg("errAddInLocalStack()");

    /* If error stack is not initialised, do it */
    if (error->thisPtr != error)
    {
        errResetLocalStack(error);
    } 

    /* Get error severity */
    propValue = errGetErrProp(moduleId, errorId, "errSeverity");
    if (propValue == NULL)
    {
        return FAILURE;
    }
    severity = (char)toupper((int)propValue[0]);

    /* Get error format */
    propValue = errGetErrProp(moduleId, errorId, "errFormat");
    if (propValue == NULL)
    {
        return FAILURE;
    }
    strcpy(format, propValue);

    /* Get the current UTC date/time */
    logGetTimeStamp(timeStamp);


    /* Fill the error message */
    va_start(argPtr,errorId);
    vsprintf(runTimePar, format, argPtr);
    va_end(argPtr);
    
    /* Add error to the stack */
    return (errPushInLocalStack(error, timeStamp, mcsGetProcName(), moduleId,
                                fileLine, errorId, severity, runTimePar));
}


/*___oOo___*/

