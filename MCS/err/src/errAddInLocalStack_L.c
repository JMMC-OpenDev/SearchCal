/*******************************************************************************
* JMMC project
*
* History
* -------
* $Log: not supported by cvs2svn $
* Revision 1.9  2005/01/24 14:49:18  gzins
* Used CVS log as modification history
*
* gzins     23-Nov-2004  removed useless warning messages related to undefined
*                        INTROOT and MCSROOT
* gzins     17-Jun-2004  completed implementation
* berezne   02-Jun-2004  created
*
*-----------------------------------------------------------------------------*/

static char *rcsId="@(#) $Id: errAddInLocalStack_L.c,v 1.10 2005-01-27 14:08:33 gzins Exp $"; 
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
#include <libxml/parser.h>

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
errERROR_STACK errGlobalStack;

/*
 * Declaration of local functions
 */
static char *errGetErrProp(const char *moduleId, 
                           int errorId, const char * propName);

/*
 * Definition of local functions
 */
static char *errGetErrProp(const char *moduleId, 
                           int errorId, const char * propName){
    logExtDbg("errGetErrProp()"); 

    GdomeDOMImplementation *domimpl;
    GdomeDocument *doc;
    GdomeElement *root, *el, *elerr;
    GdomeNodeList *childs, *errs, *texts;
    GdomeException exc;
    GdomeDOMString *name, *value;
    unsigned long i, nbChilds, nbTags;
    mcsINT32 id;
    mcsINT32 node;
    static mcsSTRING256 propValue;
    mcsSTRING256 errFileName;
    char *envVar;
    mcsLOGICAL errDefFileFound;
    struct stat statBuf;
    char *retVal = NULL;

    /* Get a DOMImplementation reference */
    domimpl = gdome_di_mkref ();

    /* Look for the error definition file */
    errDefFileFound= mcsFALSE;
    /* In ../errors */
    /* Create error file name */
    sprintf(errFileName, "../errors/%sErrors.xml", moduleId);

    /* Test if file exists */
    if (stat(errFileName, &statBuf) == 0)
    {
        errDefFileFound = mcsTRUE;
    }

    /* In $INTROOT */
    /* If not found in $INTROOT, look in $MCSROOT */
    if (errDefFileFound == mcsFALSE)
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
                errDefFileFound = mcsTRUE;
            }
	}
    }

    /* If not found in $INTROOT, look in $MCSROOT */
    if (errDefFileFound == mcsFALSE)
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
                errDefFileFound = mcsTRUE;
	    }
	}
    }

    /* If error definition file has not been found */
    if (errDefFileFound == mcsFALSE)
    {
        logWarning ("Error definition file '%sErrors.xml' can not be found",
                    moduleId);
        return NULL;
    }

    logDebug("Used error definition file '%s'", errFileName);

    /* Load a new document from a file */
    doc = gdome_di_createDocFromURI(domimpl, errFileName, GDOME_LOAD_PARSING,
                                    &exc);
    if (doc == NULL)
    {
        logWarning ("Illegal format encountered for error definition file "
                    "'%.100s'. DOMImplementation.createDocFromURI() failed "
                    "with exception #%d", errFileName, exc);
        gdome_doc_unref (doc, &exc);
        gdome_di_unref (domimpl, &exc);
        xmlCleanupParser();
        return NULL;
    }

    /* Get reference to the root element of the document */
    root = gdome_doc_documentElement (doc, &exc);
    if (root == NULL) {
        logWarning ("Illegal format encountered for error definition file "
                    "'%.100s'. Document.documentElement() failed "
                    "with exception #%d", errFileName, exc);
        gdome_el_unref(root, &exc);            
        gdome_doc_unref (doc, &exc);
        gdome_di_unref (domimpl, &exc);
        xmlCleanupParser();        
        return NULL;
    }

    /* Get the reference to the childrens NodeList of the root element */
    childs = gdome_el_childNodes (root, &exc);
    if (childs == NULL)
    {
        logWarning ("Illegal format encountered for error definition file "
                    "'%.100s'. Element.childNodes() failed "
                    "with exception #%d", errFileName, exc);
        goto errCond;
    }

    /* Search the attribute id="**" in the child elements */
    nbChilds = gdome_nl_length (childs, &exc);
    node=0;
    for (i = 0; i < nbChilds; i++)
    {
        el = (GdomeElement *)gdome_nl_item (childs, i, &exc);
        if (el == NULL)
        {
            logWarning ("Illegal format encountered for error definition file "
                        "'%.100s'. NodeList.item(%d) failed "
                        "with exception #%d", errFileName, (int)i, exc);
            gdome_el_unref(el, &exc);
            goto errCond;
        }
        if (gdome_el_nodeType (el, &exc) == GDOME_ELEMENT_NODE)
        {
            node++;
            name = gdome_str_mkref ("id");
            value = gdome_el_getAttribute(el, name, &exc);
            if (exc)
            {
                logWarning ("Illegal format encountered for error definition "
                            "file '%.100s'. Element.getAttribute(node=#%d, "
                            "name=%s) failed with exception #%d", 
                            errFileName, (int)node, name, exc);
                gdome_str_unref(name);
                gdome_str_unref(value);    
                gdome_el_unref(el, &exc);
                goto errCond;
            }
            if (sscanf(value->str, "%d", &id) != 1)
            {
                logWarning ("Illegal format encountered for error definition "
                            "file '%.100s'. Invalid error identifier for "
                            "node #%d", errFileName, (int)node);
                gdome_str_unref(name);                
                gdome_str_unref(value);
                gdome_el_unref(el, &exc);
                goto errCond;  
            }
            if (errorId == id)
            {
                gdome_str_unref(name);                
                name = gdome_str_mkref (propName);
                errs = gdome_el_getElementsByTagName (el, name, &exc);
                if (errs == NULL)
                {
                    logWarning ("Illegal format encountered for error "
                                "definition file '%.100s'. "
                                "Element.childNodes(node=#%d, "
                                "name=%s) failed with exception #%d", 
                                errFileName, (int)node, name, exc);
                    gdome_nl_unref(errs, &exc);
                    gdome_str_unref(value);    
                    gdome_str_unref(name);
                    gdome_el_unref(el, &exc);
                    goto errCond;            
                }
                /* Check number of tags for the node */
                nbTags = gdome_nl_length (errs, &exc);
                if (nbTags == 0)
                {
                    logWarning ("Illegal format encountered for error "
                                "definition file '%.100s'. "
                                "Tag '%s' not found for node #%d", 
                                errFileName, propName, (int)node);
                    gdome_nl_unref(errs, &exc);
                    gdome_str_unref(value); 
                    gdome_str_unref(name);
                    gdome_el_unref(el, &exc);
                    goto errCond;         
                }
                if (nbTags != 1)
                {
                    logWarning ("Illegal format encountered for error "
                                "definition file '%.100s'. "
                                "Duplicated tag '%s' for node #%d", 
                                errFileName, propName, (int)node);
                    gdome_nl_unref(errs, &exc);
                    gdome_str_unref(value); 
                    gdome_str_unref(name);
                    gdome_el_unref(el, &exc);
                    goto errCond;         
                }

                /* Get none value */
                elerr = (GdomeElement *)gdome_nl_item (errs, 0, &exc);
                texts = gdome_el_childNodes (elerr, &exc);
                if (texts == NULL)
                {
                    logWarning ("Illegal format encountered for error "
                                "definition file '%.100s'. "
                                "Element.childNodes(node=#%d, "
                                "name=%s) failed with exception #%d", 
                                errFileName, (int)node, propName, exc);
                    gdome_nl_unref(texts, &exc);    
                    gdome_el_unref(elerr, &exc);
                    gdome_nl_unref(errs, &exc);
                    gdome_str_unref(value); 
                    gdome_str_unref(name);    
                    gdome_el_unref(el, &exc);
                    goto errCond;         
                }
                /*This element must contain one child : TextElement*/
                gdome_el_unref(elerr, &exc);
                elerr = (GdomeElement *)gdome_nl_item (texts, 0, &exc);
                gdome_str_unref(value); 
                value = gdome_el_nodeValue(elerr, &exc);
                if (value == NULL)
                {
                    logWarning ("Illegal format encountered for error "
                                "definition file '%.100s'. "
                                "Element.nodeValue(node=#%d, "
                                "name=%s) failed with exception #%d", 
                                errFileName, (int)node, propName, exc);
                    gdome_nl_unref(texts, &exc);    
                    gdome_el_unref(elerr, &exc);
                    gdome_nl_unref(errs, &exc);
                    gdome_str_unref(value); 
                    gdome_str_unref(name);
                    gdome_el_unref(el, &exc);
                    goto errCond;         
                }
                
                /* Check node value length */
                if (strlen(value->str) >= sizeof(mcsSTRING256))
                {
                    logWarning ("Error in error definition file '%.100s'. "
                                "Size of property value '%s' of errorId #%d "
                                "too long; max=%d, current=%d",
                                propName, errorId, sizeof(mcsSTRING256), 
                                strlen(value->str));
                    gdome_nl_unref(texts, &exc);    
                    gdome_el_unref(elerr, &exc);
                    gdome_nl_unref(errs, &exc);
                    gdome_str_unref(value); 
                    gdome_str_unref(name);
                    gdome_el_unref(el, &exc);
                    goto errCond;         
                }

                strncpy(propValue, value->str, sizeof(mcsSTRING256)-1);
                retVal = propValue;

                gdome_nl_unref(texts, &exc);                 
                gdome_el_unref(elerr, &exc);
                gdome_nl_unref(errs, &exc);
            }
            gdome_str_unref(value); 
            gdome_str_unref(name);
        }
        gdome_el_unref (el, &exc);
    }

    /* Check the error property has bben found */
    if (retVal == NULL)
    {
        logWarning ("Definition of errorId #%d not found in error "
                    "definition file '%s'",
                     errorId, errFileName);
    }
    
    /* Free the document structure and the DOMImplementation */
errCond:
    gdome_nl_unref(childs, &exc);            
    gdome_el_unref(root, &exc);
    gdome_doc_unref (doc, &exc);
    gdome_di_unref (domimpl, &exc);
    xmlCleanupParser();

    /* Return property value */
    return retVal;
}

/**
 * Add a new error to the error stack
 *
 * \param error error structure.
 * \param moduleId module identifier
 * \param fileLine file name and line number from where the error has been added
 * \param errorId error number
 * \param isErrUser specify whether the error message is intended or not to the 
 * \param argPtr (optional) argument list associated to the error 
 *
 * \return mcsSUCCESS on successfull completion, mcsFAILURE otherwise.
 */
mcsCOMPL_STAT errAddInLocalStack_v(errERROR_STACK    *error,
                                   const mcsMODULEID moduleId,
                                   const char        *fileLine,
                                   mcsINT32          errorId,
                                   mcsLOGICAL        isErrUser,
                                   va_list           argPtr)
{
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
        return mcsFAILURE;
    }
    severity = (char)toupper((int)propValue[0]);

    /* Get error format */
    propValue = errGetErrProp(moduleId, errorId, "errFormat");
    if (propValue == NULL)
    {
        return mcsFAILURE;
    }
    strcpy(format, propValue);

    /* Get the current UTC date/time */
    logGetTimeStamp(timeStamp);


    /* Fill the error message */
    vsprintf(runTimePar, format, argPtr);
    
    /* Add error to the stack */
    return (errPushInLocalStack(error, timeStamp, mcsGetProcName(), moduleId,
                                fileLine, errorId, isErrUser, 
                                severity, runTimePar));
}

/**
 * Add a new error to the error stack
 *
 * \param error error structure.
 * \param moduleId module identifier
 * \param fileLine file name and line number from where the error has been added
 * \param errorId error number
 * \param isErrUser specify whether the error message is intended or not to the 
 * \param argPtr (optional) argument list associated to the error 
 *
 * \return mcsSUCCESS on successfull completion, mcsFAILURE otherwise.
 */
mcsCOMPL_STAT errAddInLocalStack(errERROR_STACK    *error,
                                 const mcsMODULEID moduleId,
                                 const char        *fileLine,
                                 mcsINT32          errorId,
                                 mcsLOGICAL        isErrUser,
                                 ...)
{
    va_list       argPtr;
    mcsCOMPL_STAT status;

    logExtDbg("errAddInStack()");

    va_start(argPtr, isErrUser);
    status = errAddInLocalStack_v(error, moduleId, 
                                  fileLine, errorId, isErrUser, argPtr);
    va_end(argPtr);

    return (status);
}

/*___oOo___*/

