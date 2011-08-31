/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Definition of errAddInLocalStack function.
 */

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
#include "pthread.h"

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

/** thread local storage key for error stack */
static pthread_key_t tlsKey_errStack;
/** flag to indicate that the thread local storage is initialized */
static mcsLOGICAL errInitialized = mcsFALSE;

/*
 * Declaration of local functions
 */
static mcsCOMPL_STAT errGetErrProp(const char* moduleId, 
                          int errorId, const char* propName,
                          mcsSTRING256* propValue);

/*
 * Definition of local functions
 */
/**
 * Get property of the given error.
 *
 * This function returns the requested property, as defined in error definition
 * file (e.g. errSeverity or errFormat), of an error specified by \em errorId. 
 *
 * \param moduleId module identifier
 * \param errorId error number
 * \param propName name of property
 *
 * \return property value or NULL if not found. 
 */
static mcsCOMPL_STAT errGetErrProp(const char* moduleId, 
                          int errorId, const char* propName,
                          mcsSTRING256* propValue)
{

    GdomeDOMImplementation* domimpl;
    GdomeDocument* doc = NULL;
    GdomeElement *root, *el, *elerr;
    GdomeNodeList *childs, *errs, *texts;
    GdomeException exc;
    GdomeDOMString *name, *value;
    unsigned long i, nbChilds, nbTags;
    mcsINT32 id;
    mcsINT32 node;
    mcsSTRING256 errFileName;
    mcsSTRING256 envVar;
    mcsLOGICAL errDefFileFound;
    struct stat statBuf;
    mcsCOMPL_STAT result = mcsFAILURE;
    
    /* reset result */
    *propValue[0] = '\0';

    /* Look for the error definition file */
    errDefFileFound = mcsFALSE;
    
    /* In ../errors */
    /* Create error file name */
    snprintf(errFileName, sizeof(errFileName) - 1, "../errors/%sErrors.xml", moduleId);

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
        if (mcsGetEnv_r("INTROOT", envVar, sizeof(envVar)) == mcsSUCCESS) 
        {
            /* Create error file name */
            snprintf(errFileName, sizeof(errFileName) - 1, "%s/errors/%sErrors.xml", envVar, moduleId);

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
        if (mcsGetEnv_r("MCSROOT", envVar, sizeof(envVar)) == mcsSUCCESS) 
        {
            /* Create error file name */
            snprintf(errFileName, sizeof(errFileName) - 1, "%s/errors/%sErrors.xml", envVar, moduleId);

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
        logWarning ("Error definition file '%sErrors.xml' can not be found", moduleId);
        return mcsFAILURE;
    }

    logTrace("Used error definition file '%s'", errFileName);

    mcsLockGdomeMutex();

    /* Get a DOMImplementation reference */
    domimpl = gdome_di_mkref();
    
    /* Load a new document from a file */
    doc = gdome_di_createDocFromURI(domimpl, errFileName, GDOME_LOAD_PARSING, &exc);
    if (doc == NULL)
    {
        logWarning ("Illegal format encountered for error definition file "
                    "'%.100s'. DOMImplementation.createDocFromURI() failed "
                    "with exception #%d", errFileName, exc);
        
        gdome_doc_unref(doc, &exc);
        gdome_di_unref(domimpl, &exc);
        
        mcsUnlockGdomeMutex();
        
        return mcsFAILURE;
    }

    /* Get reference to the root element of the document */
    root = gdome_doc_documentElement(doc, &exc);
    
    if (root == NULL) {
        logWarning ("Illegal format encountered for error definition file "
                    "'%.100s'. Document.documentElement() failed "
                    "with exception #%d", errFileName, exc);
        
        gdome_el_unref(root, &exc);            
        gdome_doc_unref(doc, &exc);
        gdome_di_unref(domimpl, &exc);
        
        mcsUnlockGdomeMutex();

        return mcsFAILURE;
    }

    /* Get the reference to the childrens NodeList of the root element */
    childs = gdome_el_childNodes(root, &exc);
    
    if (childs == NULL)
    {
        logWarning ("Illegal format encountered for error definition file "
                    "'%.100s'. Element.childNodes() failed "
                    "with exception #%d", errFileName, exc);
        goto errCond;
    }

    /* Search the attribute id="**" in the child elements */
    nbChilds = gdome_nl_length(childs, &exc);
    node = 0;
    
    for (i = 0; i < nbChilds; i++)
    {
        el = (GdomeElement*)gdome_nl_item(childs, i, &exc);
        if (el == NULL)
        {
            logWarning ("Illegal format encountered for error definition file "
                        "'%.100s'. NodeList.item(%d) failed "
                        "with exception #%d", errFileName, (int)i, exc);
            
            gdome_el_unref(el, &exc);
            goto errCond;
        }
        
        if (gdome_el_nodeType(el, &exc) == GDOME_ELEMENT_NODE)
        {
            node++;
            name  = gdome_str_mkref("id");
            value = gdome_el_getAttribute(el, name, &exc);
            
            if (exc)
            {
                logWarning ("Illegal format encountered for error definition "
                            "file '%.100s'. Element.getAttribute(node=#%d, "
                            "name=%s) failed with exception #%d", 
                            errFileName, (int)node, name, exc);
                
                gdome_str_unref(value);    
                gdome_str_unref(name);
                gdome_el_unref(el, &exc);
                goto errCond;
            }

            gdome_str_unref(name);
            
            if (sscanf(value->str, "%d", &id) != 1)
            {
                logWarning ("Illegal format encountered for error definition "
                            "file '%.100s'. Invalid error identifier for "
                            "node #%d", errFileName, (int)node);
                
                gdome_str_unref(value);
                gdome_el_unref(el, &exc);
                goto errCond;  
            }
            
            gdome_str_unref(value);
            
            if (errorId == id)
            {
                name = gdome_str_mkref(propName);
                errs = gdome_el_getElementsByTagName(el, name, &exc);
                
                if (errs == NULL)
                {
                    logWarning ("Illegal format encountered for error "
                                "definition file '%.100s'. "
                                "Element.childNodes(node=#%d, "
                                "name=%s) failed with exception #%d", 
                                errFileName, (int)node, name, exc);
                    
                    gdome_nl_unref(errs, &exc);
                    gdome_str_unref(name);
                    gdome_el_unref(el, &exc);
                    goto errCond;            
                }

                gdome_str_unref(name);
                
                /* Check number of tags for the node */
                nbTags = gdome_nl_length(errs, &exc);
                if (nbTags == 0)
                {
                    logWarning ("Illegal format encountered for error "
                                "definition file '%.100s'. "
                                "Tag '%s' not found for node #%d", 
                                errFileName, propName, (int)node);
                    
                    gdome_nl_unref(errs, &exc);
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
                    gdome_el_unref(el, &exc);
                    goto errCond;         
                }

                /* Get node value */
                elerr = (GdomeElement*)gdome_nl_item(errs, 0, &exc);
                
                gdome_nl_unref(errs, &exc);
                
                texts = gdome_el_childNodes(elerr, &exc);
                if (texts == NULL)
                {
                    logWarning ("Illegal format encountered for error "
                                "definition file '%.100s'. "
                                "Element.childNodes(node=#%d, "
                                "name=%s) failed with exception #%d", 
                                errFileName, (int)node, propName, exc);
                    
                    gdome_nl_unref(texts, &exc);    
                    gdome_el_unref(elerr, &exc);
                    gdome_el_unref(el, &exc);
                    goto errCond;         
                }

                gdome_el_unref(elerr, &exc);
                
                /*This element must contain one child : TextElement*/
                elerr = (GdomeElement*)gdome_nl_item(texts, 0, &exc);

                gdome_nl_unref(texts, &exc);    
                
                value = gdome_el_nodeValue(elerr, &exc);
                
                if (value == NULL)
                {
                    logWarning ("Illegal format encountered for error "
                                "definition file '%.100s'. "
                                "Element.nodeValue(node=#%d, "
                                "name=%s) failed with exception #%d", 
                                errFileName, (int)node, propName, exc);
                    
                    gdome_str_unref(value); 
                    gdome_el_unref(elerr, &exc);
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
                    
                    gdome_str_unref(value); 
                    gdome_el_unref(elerr, &exc);
                    gdome_el_unref(el, &exc);
                    goto errCond;         
                }

                strncpy(*propValue, value->str, sizeof(mcsSTRING256) - 1);

                gdome_str_unref(value); 
                gdome_el_unref(elerr, &exc);
                gdome_el_unref(el, &exc);
                
                result = mcsSUCCESS;
                
                // exit quickly
                goto errCond;
            }
        }
        gdome_el_unref(el, &exc);
        
    } // childs

    /* Check the error property has been found */
    if (result == mcsFAILURE)
    {
        logWarning ("Definition of errorId #%d not found in error "
                    "definition file '%s'", errorId, errFileName);
    }
    
    /* Free the document structure and the DOMImplementation */
errCond:
    gdome_nl_unref(childs, &exc);            
    gdome_el_unref(root, &exc);
    gdome_doc_unref(doc, &exc);
    gdome_di_unref(domimpl, &exc);
    
    mcsUnlockGdomeMutex();
    
    return result;
}

/**
 * Add a new error to the error stack
 *
 * \param error error structure.
 * \param moduleId module identifier
 * \param fileLine file name and line number from where the error has been added
 * \param errorId error number
 * \param isErrUser specify whether the error message is intended or not to the 
 * end-user.
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
    mcsSTRING256 errName;
    mcsSTRING256 runTimePar;
    mcsSTRING256 propValue;

    logTrace("errAddInLocalStack_v()");
    
    if (error == NULL)
    {
        return mcsFAILURE;
    }

    /* If error stack is not initialised, do it */
    if (error->stackInit == mcsFALSE)
    {
        errResetLocalStack(error);
    } 

    /* Get error severity */
    if (errGetErrProp(moduleId, errorId, "errSeverity", &propValue) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    severity = (char)toupper((int)propValue[0]);

    /* Get error format */
    if (errGetErrProp(moduleId, errorId, "errFormat", &format) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    /* Get error format */
    if (errGetErrProp(moduleId, errorId, "errName", &propValue) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    snprintf(errName, sizeof(errName) - 1, "%s_ERR_%s", moduleId, propValue);

    /* Get the current UTC date/time */
    logGetTimeStamp(timeStamp);

    /* Fill the error message */
    vsnprintf(runTimePar, sizeof(mcsSTRING256) - 1, format, argPtr);
    
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
 * end-user.
 * \param ... (optional) argument list associated to the error 
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

    logTrace("errAddInLocalStack()");

    va_start(argPtr, isErrUser);
    status = errAddInLocalStack_v(error, moduleId, 
                                  fileLine, errorId, isErrUser, argPtr);
    va_end(argPtr);

    return (status);
}


/**
 * Get the error stack relative to the current pthread
 * \param error returned error stack pointer
 * \return mcsSUCCESS on successfull completion, mcsFAILURE otherwise.
 */
errERROR_STACK* errGetThreadStack()
{
    logDebug("errGetThreadStack : get error stack for thread %d", pthread_self());
    
    if (errInitialized == mcsFALSE)
    {
        /* Useful for mono thread executables */
        if (errInit() == mcsFAILURE) {
            return NULL;
        }
    }

    void* global;
    errERROR_STACK* errStack;
    
    global = pthread_getspecific(tlsKey_errStack);
    
    if (global == NULL)
    {
        /* first time - create the error stack */
        errStack = (errERROR_STACK*)malloc(sizeof(errERROR_STACK));
        
        /* explicitely indicate that this error stack is not intialized */
        errStack->stackInit = mcsFALSE;
        
        pthread_setspecific(tlsKey_errStack, errStack);
    } 
    else
    {
        errStack = (errERROR_STACK*)global;
    }

    logDebug("errGetThreadStack : return %p", errStack);
    
    return errStack;
}


/**
 * Thread local key destructor
 * @param value value to free
 */
static void tlsErrStackDestructor(void* value)
{
    logDebug("tlsErrStackDestructor : %p", value);

    errERROR_STACK* errStack;
    errStack = (errERROR_STACK*)value;
    
    /* logs any error and reset global stack */
    errCloseLocalStack(errStack);
    
    /* free values */
    free(value);
    pthread_setspecific(tlsKey_errStack, NULL);
}


/**
 * Initialize the thread local storage for error stacks
 */
mcsCOMPL_STAT errInit(void)
{
    logDebug("errInit");
    
    const int rc = pthread_key_create(&tlsKey_errStack, tlsErrStackDestructor);
    if (rc != 0) {
        return mcsFAILURE;
    }
    
    errInitialized = mcsTRUE;
    
    return mcsSUCCESS;
}


/**
 * Destroy the thread local storage for error stacks
 */
mcsCOMPL_STAT errExit(void)
{
    logDebug("errExit");

    /* Get and free main error stack */
    errERROR_STACK *error = errGetThreadStack();
    tlsErrStackDestructor(error);
    
    pthread_key_delete(tlsKey_errStack);
    
    errInitialized = mcsFALSE;
    
    return mcsSUCCESS;
}


/*___oOo___*/

