/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * vobsPARSER class definition.
 */

/* 
 * System Headers 
 */
#include <iostream>
using namespace std;
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <string.h>
#include <libxml/parser.h>
/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"
#include "misc.h"

/*
 * Local Headers 
 */
#include "vobsPARSER.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"

/*
 * Local Functions
 */
// Class constructor 
vobsPARSER::vobsPARSER()
{
}

// Class destructor 
vobsPARSER::~vobsPARSER()
{
}

/**
 * Load XML document from URI and parse it to extract the list of star.
 * 
 * This method loads the XML document from the URI, check it is  well formed
 * and parses it to extract the list of stars contained in the CDATA section.
 * The extracted star are stored in the list \em starList given as parameter.
 *
 * @param uri URI from where XML document has to be loaded.
 * @param catalogName catalog name from where XML document has been got.
 * @param starList list where star has to be put.
 * @param logFileName file to save the result of the asking
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned
 */
mcsCOMPL_STAT vobsPARSER::Parse(const char *uri,
                                const char *data,
                                const char *catalogName,
                                vobsSTAR_LIST &starList,
                                const char *logFileName)
{    
    logDebug("vobsPARSER::Parse() - catalog '%s'", catalogName);	

    // Clear the output star list anyway (even if the query fails):
    starList.Clear();

    // Load a new document from the URI
    logTest("Get XML document from '%s' with POST data '%s'", uri, data);

    // Create a misco buffer to store CDS results
    miscoDYN_BUF completeReturnBuffer;

    // Query the CDS
    if (miscPerformHttpPost(uri, data, completeReturnBuffer.GetInternalMiscDYN_BUF(), vobsTIME_OUT) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    mcsUINT32 storedBytesNb = 0;
    completeReturnBuffer.GetNbStoredBytes(&storedBytesNb);
    
    logTest("Parsing XML document (%d bytes)", storedBytesNb);
    
    mcsLockGdomeMutex();
    
    // Get a DOMImplementation reference
    GdomeDOMImplementation* domimpl = gdome_di_mkref();

    GdomeException exc;
    
    // XML parsing of the CDS answer
    GdomeDocument* doc = gdome_di_createDocFromMemory(domimpl, completeReturnBuffer.GetBuffer(), GDOME_LOAD_PARSING, &exc);
    
    if (doc == NULL) 
    {
        errAdd(vobsERR_GDOME_CALL, "gdome_di_createDocFromURI", exc);
        // free gdome object
        gdome_doc_unref(doc, &exc);
        gdome_di_unref(domimpl, &exc);
        
        mcsUnlockGdomeMutex();
        
        return mcsFAILURE;
    }

    // Get reference to the root element of the document
    GdomeElement* root = gdome_doc_documentElement(doc, &exc);
    
    if (root == NULL)
    {
        errAdd(vobsERR_GDOME_CALL, "gdome_doc_documentElement", exc);
        // free gdome object
        gdome_el_unref(root, &exc);            
        gdome_doc_unref(doc, &exc);
        gdome_di_unref(domimpl, &exc);

        mcsUnlockGdomeMutex();
        
        return mcsFAILURE;
    }

    // Create the cData parser;
    vobsCDATA cData;

    // Set the catalog name
    cData.SetCatalogName(catalogName);
    
    // Begin the recursive traversal of the XML tree
    if (ParseXmlSubTree((GdomeNode *)root, &cData) == mcsFAILURE)
    {
        // free gdome object
        gdome_el_unref(root, &exc);            
        gdome_doc_unref(doc, &exc);
        gdome_di_unref(domimpl, &exc);

        mcsUnlockGdomeMutex();
        
        return mcsFAILURE;
    }
    
    // free gdome object
    gdome_el_unref(root, &exc);            
    gdome_doc_unref(doc, &exc);
    gdome_di_unref(domimpl, &exc);

    mcsUnlockGdomeMutex();

    // Print out CDATA description and Save xml file
    if ((logFileName != NULL) || doLog(logDEBUG))
    {
        mcsSTRING32 catalog;
        strcpy(catalog, catalogName);
        if (miscReplaceChrByChr(catalog, '/', '_') == mcsFAILURE)
        {
            return mcsFAILURE;
        }
        mcsSTRING256 xmlFileName;
        strcpy(xmlFileName, "$MCSDATA/tmp/");
        strcat(xmlFileName, catalog);
        strcat(xmlFileName, ".xml");

        char *resolvedPath;
        // Resolve path
        resolvedPath = miscResolvePath(xmlFileName);
        if (resolvedPath != NULL)
        {
            logTest("Save XML document to: %s", resolvedPath);
            
            if (completeReturnBuffer.SaveInASCIIFile(resolvedPath) == mcsFAILURE)
            {
                free(resolvedPath);
                return mcsFAILURE;
            }
            free(resolvedPath);
        }
        
        mcsUINT32 nbParams = cData.GetNbParams();

        logDebug("CDATA description");
        logDebug("   Number of lines to be skipped : %d", cData.GetNbLinesToSkip());
        logDebug("   Number of parameters in table : %d", nbParams);
        
        char *paramName;
        char *ucdName;
        for (unsigned int i = 0; i < nbParams; i++)
        {
            // Table header
            if (i == 0)
            {
                logDebug("   +----------+--------------+--------------------+");
                logDebug("   | Param #  | Name         | UCD1               |");
                logDebug("   +----------+--------------+--------------------+");
            }
            // Get the parameter name and UCD
            if (cData.GetNextParamDesc(&paramName, &ucdName, (mcsLOGICAL)(i == 0)) == mcsFAILURE)
            {
                return mcsFAILURE;
            }

            logDebug("   |      %3d | %12s | %18s |", i+1, paramName, ucdName);
            ++paramName;
            ++ucdName;

            // Table footer
            if (i == (nbParams - 1))
            {
                logDebug("   +----------+--------------+--------------------+");
            }
        }
    }

    mcsUINT32 nbLines = cData.GetNbLines();
    
    // If CDATA section has been found
    if (nbLines != 0)
    {
        // Save CDATA (if requested)
        if ((logFileName != NULL) && (miscIsSpaceStr(logFileName) == mcsFALSE))
        {
            logTest("Save CDATA to: %s", logFileName);
            
            if (cData.SaveInASCIIFile(logFileName) == mcsFAILURE) {
                errCloseStack();
            }
        }

        // Parse the CDATA section

        // Because the lines to be skipped have been removed when appending
        // lines, there is no more line to skip.
        cData.SetNbLinesToSkip(0);

        logTest("Extracting data from CDATA section (%d rows)", nbLines);
        
        vobsSTAR star;
        if (cData.Extract(star, starList) == mcsFAILURE)
        {
            // Clear the output star list when the query fails:
            starList.Clear();

            return mcsFAILURE;
        }
    }
    else 
    {
        logTest("No CDATA section in XML document.");
    }
    return mcsSUCCESS;
}

/**
 * Parse recursively XML document.
 *
 * This method recursively parses the XML document (i.e. node by node), and
 * extract informations related to the CDATA section which contains the star
 * table. These informations are the name of the parameters with the
 * corresponding UCD, the number lines to be skipped in CDATA section and the
 * pointer to the CDATA buffer (see vobsCDATA data structure).
 * Informations about parameters are given by 'FIELD' node; 'name' attribute for
 * the parameter name and 'UCD' for UCD, and the number of line to skip is given
 * by 'CSV' node and 'headlines' attribute.
 *
 * @param node  XML document node to be parsed. 
 * @param cData data structure where CDATA description has to be stored.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned
 * and an error is added to the error stack. The possible error is :
 * \li vobsERR_GDOME_CALL
 *
 * \todo
 * nblinetojump problem
 */
mcsCOMPL_STAT vobsPARSER::ParseXmlSubTree(GdomeNode *node, vobsCDATA *cData)
{
    GdomeException exc;

    // Get the node list containing all children of this node
    GdomeNodeList* nodeList = gdome_n_childNodes(node, &exc);
    
    if (exc != GDOME_NOEXCEPTION_ERR)
    {
        errAdd(vobsERR_GDOME_CALL, "gdome_n_childNodes", exc);
        // free gdome object
        gdome_nl_unref(nodeList, &exc);
        
        return mcsFAILURE;
    }
    
    // Get the number of children
    unsigned long nbChildren = gdome_nl_length(nodeList, &exc);
    
    if (exc != GDOME_NOEXCEPTION_ERR)
    {
        errAdd(vobsERR_GDOME_CALL, "gdome_nl_length", exc);
        // free gdome object
        gdome_nl_unref(nodeList, &exc);
        
        return mcsFAILURE;
    }

    // If there is no child; return
    if (nbChildren == 0)
    {
        // free gdome object
        gdome_nl_unref(nodeList, &exc);
        
        return mcsSUCCESS;
    }
    
    // For each child
    for (unsigned int i = 0; i < nbChildren; i++)
    {
        // Get the the child in the node list
        GdomeNode* child = gdome_nl_item(nodeList, i, &exc);
        
        if (child == NULL) 
        {
            errAdd(vobsERR_GDOME_CALL, "gdome_nl_item", exc);
            // free gdome object
            gdome_n_unref(child, &exc);
            gdome_nl_unref(nodeList, &exc);
            
            return mcsFAILURE;
        }
        
        // Get the child node type
        unsigned short nodeType = gdome_n_nodeType(child, &exc);

        // If it is the CDATA section
        if (nodeType == GDOME_CDATA_SECTION_NODE) 
        {
            /* Get CDATA */
            GdomeDOMString* cdataValue = gdome_cds_data(GDOME_CDS(child), &exc);
            
            if (exc != GDOME_NOEXCEPTION_ERR)
            {
                errAdd(vobsERR_GDOME_CALL, "gdome_cds_data", exc);
                // free gdome object
                gdome_str_unref(cdataValue);
                gdome_n_unref(child, &exc);
                gdome_nl_unref(nodeList, &exc);
                
                return mcsFAILURE;
            }
            else
            {
                if (cData->AppendLines(cdataValue->str, cData->GetNbLinesToSkip()) == mcsFAILURE)
                {
                    // free gdome object
                    gdome_str_unref(cdataValue);
                    gdome_n_unref(child, &exc);
                    gdome_nl_unref(nodeList, &exc);
                    
                    return mcsFAILURE;
                }
            }
            gdome_str_unref(cdataValue);
        }
        // If it is an element node, try to get information on attributes
        else if (nodeType == GDOME_ELEMENT_NODE) 
        {
            // Get the node name
            GdomeDOMString* nodeName = gdome_n_nodeName(child, &exc);
            if (exc != GDOME_NOEXCEPTION_ERR)
            {
                errAdd(vobsERR_GDOME_CALL, "gdome_n_nodeName", exc);
                // free gdome object
                gdome_str_unref(nodeName);
                gdome_n_unref(child, &exc);
                gdome_nl_unref(nodeList, &exc);
                return mcsFAILURE;
            }
            
            // Check if nodeName is FIELD or CSV:
            bool isField = false;
            bool isCsv  = false;
            
            isField = (strcmp(nodeName->str, "FIELD") == 0);
            
            if (!isField)
            {
                isCsv = (strcmp(nodeName->str, "CSV") == 0);
            }

            // free gdome object
            gdome_str_unref(nodeName);
            
            // Parse attributes then:
            if (isField || isCsv)
            {
                // Get the attributes list
                GdomeNamedNodeMap* attrList = gdome_n_attributes(child, &exc);
                if (exc != GDOME_NOEXCEPTION_ERR)
                {
                    errAdd(vobsERR_GDOME_CALL, "gdome_n_attributes", exc);
                    // free gdome object
                    gdome_nnm_unref(attrList, &exc);
                    gdome_n_unref(child, &exc);
                    gdome_nl_unref(nodeList, &exc);
                    return mcsFAILURE;
                }

                // Get the number of attributes
                unsigned long nbAttrs = gdome_nnm_length(attrList, &exc);
                if (exc != GDOME_NOEXCEPTION_ERR)
                {
                    errAdd(vobsERR_GDOME_CALL, "gdome_nnm_length", exc);
                    // free gdome object
                    gdome_nnm_unref(attrList, &exc);
                    gdome_n_unref(child, &exc);
                    gdome_nl_unref(nodeList, &exc);
                    return mcsFAILURE;
                }

                // For each attribute
                for (unsigned int j = 0; j < nbAttrs; j++)
                {
                    // Get the ith attribute in the node list
                    GdomeNode* attr = gdome_nnm_item(attrList, j, &exc);
                    if (exc != GDOME_NOEXCEPTION_ERR)
                    {
                        errAdd(vobsERR_GDOME_CALL, "gdome_nnm_item", exc);
                        // free gdome object
                        gdome_n_unref(attr, &exc);
                        gdome_nnm_unref(attrList, &exc);
                        gdome_n_unref(child, &exc);
                        gdome_nl_unref(nodeList, &exc);
                        return mcsFAILURE;
                    }
                    else
                    {
                        // Get the attribute name
                        GdomeDOMString* attrName = gdome_n_nodeName(attr, &exc);
                        if (exc != GDOME_NOEXCEPTION_ERR)
                        {
                            errAdd(vobsERR_GDOME_CALL, "gdome_n_nodeName", exc);
                            // free gdome object
                            gdome_str_unref(attrName);
                            gdome_n_unref(attr, &exc);
                            gdome_nnm_unref(attrList, &exc);
                            gdome_n_unref(child, &exc);
                            gdome_nl_unref(nodeList, &exc);
                            return mcsFAILURE;
                        }

                        // Check if attrName is name/ucd for nodeName = FIELD or headlines for nodeName = CSV:
                        bool isFieldName    = false;
                        bool isFieldUcd     = false;
                        bool isCsvHeadlines = false;

                        if (isField) {
                            isFieldName = (strcmp(attrName->str, "name")  == 0);
                            
                            if (!isFieldName)
                            {
                                isFieldUcd = (strcmp(attrName->str, "ucd")  == 0);
                            }
                                    
                        } else if (isCsv) {
                            isCsvHeadlines = (strcmp(attrName->str, "headlines")  == 0);
                        }

                        // free gdome objects
                        gdome_str_unref(attrName);                    
                        
                        if (isFieldName || isFieldUcd || isCsvHeadlines)
                        {
                            // Get the attribute value
                            GdomeDOMString* attrValue = gdome_n_nodeValue(attr, &exc);
                            if (exc != GDOME_NOEXCEPTION_ERR)
                            {
                                errAdd(vobsERR_GDOME_CALL, "gdome_n_nodeValue", exc);
                                // free gdome object
                                gdome_str_unref(attrValue);
                                gdome_n_unref(attr, &exc);
                                gdome_nnm_unref(attrList, &exc);
                                gdome_n_unref(child, &exc);
                                gdome_nl_unref(nodeList, &exc);
                                return mcsFAILURE;
                            }

                            // If it is the name a parameter of CDATA 
                            if (isFieldName)
                            {
                                cData->AddParamName(attrValue->str); 
                            } 
                            else
                            // If it is the UCD name of the corresponding parameter
                            if (isFieldUcd)
                            {
                                cData->AddUcdName(attrValue->str); 
                            }
                            else
                            // If it is the number of lines to be skipped
                            // before accessing to data in CDATA table 
                            // NOTE: Skip one line more than the value given by
                            // CDS because the CDATA buffer always contains
                            // an empty line at first.
                            if (isCsvHeadlines)
                            {
                                cData->SetNbLinesToSkip(atoi(attrValue->str) + 1); 
                            }

                            // free gdome objects
                            gdome_str_unref(attrValue);                    
                        }
                    }
                    // free gdome object
                    gdome_n_unref(attr, &exc);                
                    
                } // For attr
                
                // free gdome object
                gdome_nnm_unref(attrList, &exc);            
            }
            
            // If there are children nodes, parse corresponding XML sub-tree
            if (gdome_n_hasChildNodes(child, &exc))
            {
                if (ParseXmlSubTree(child, cData) == mcsFAILURE)
                {
                    // free gdome object
                    gdome_n_unref(child, &exc);
                    gdome_nl_unref(nodeList, &exc);
                    return mcsFAILURE;
                }
            }
        }

        // free gdome object
        gdome_n_unref(child, &exc);        
    }

    // free gdome object
    gdome_nl_unref(nodeList, &exc);
    
    return mcsSUCCESS;
}

/*___oOo___*/
