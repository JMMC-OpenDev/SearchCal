/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsPARSER.cpp,v 1.31 2008-04-10 14:19:25 lafrasse Exp $"
*
* History
* -------
* $Log: not supported by cvs2svn $
* Revision 1.30  2008/03/10 07:50:22  lafrasse
* Minor modifications on comments and log traces.
*
* Revision 1.29  2007/10/31 11:16:17  gzins
* Increased size of logged message related to HTTP request
*
* Revision 1.28  2007/06/27 20:41:47  gzins
* Limited length of logged string causing segmentation fault
*
* Revision 1.27  2006/08/22 14:42:50  gzins
* Used 'ucd1' instead of 'ucd' which is now used for UCD1+
*
* Revision 1.26  2006/03/03 15:03:27  scetre
* Changed rcsId to rcsId __attribute__ ((unused))
*
* Revision 1.25  2005/11/16 10:47:55  scetre
* Updated documentation
*
* Revision 1.24  2005/11/16 10:47:54  scetre
* Updated documentation
*
* Revision 1.23  2005/09/19 10:42:03  scetre
* Add Save of the xml file
*
* Revision 1.22  2005/09/13 11:52:04  scetre
* Load xml file from memory after using msgSOCKET instead of loading xml file directly with gdome methods
*
* Revision 1.21  2005/06/01 14:16:55  scetre
* Changed logExtDbg to logTrace
*
* Revision 1.20  2005/04/14 14:39:03  scetre
* Updated documentation.
* added test on method return.
*
* Revision 1.19  2005/02/13 15:23:20  gzins
* Fixed modification history
*
* Revision 1.18  2005/02/13 15:16:07  gzins
* Removed ParseCDATA and GetPropertyId. Moved to vobsCDATA class
*
* Revision 1.17  2005/02/11 10:41:55  gzins
* Updated to correctly handle the number of lines to be skipped.
* Fixed bug when scanning UCD/parameter to add star properties
*
* Revision 1.16  2005/02/11 10:34:49  gzins
* Added GetPropertyId() method
* Improved parsing of CDATA
*
* Revision 1.15  2005/02/10 10:46:42  gzins
* Changed column name to parameter name
*
* Revision 1.14  2005/02/10 08:07:38  scetre
* changed parser and hd, hip, dm number id in order to get all of them even if
* they have the same UCD
*
* Revision 1.13  2005/02/10 06:28:29  gzins
* Fixed bug related to ERRO UCD handling
*
* Revision 1.12  2005/02/08 21:34:05  gzins
* Handled ERROR UCDs by renaming them; i.e adding previous UCD as prefix
*
* Revision 1.11  2005/02/08 20:38:37  gzins
* Added name of catalog from where data is coming from
* Changed some parameter types from char* to const char*
*
* Revision 1.10  2005/02/07 09:47:08  gzins
* Renamed vobsCDATA method to be compliant with programming standards; method
* name starts with capital
*
* Revision 1.9  2005/02/04 15:25:40  gzins
* Minor change in log message
*
* Revision 1.8  2005/02/04 07:40:53  gzins
* Limited number of logged messages for test
*
* Revision 1.7  2005/01/27 13:45:30  scetre
* Add functionnality to the parser to parse II/225 return
*
* Revision 1.6  2005/01/26 08:11:28  scetre
* change history
*
* scetre    06-Jul-2004  Created
* gzins     09-Dec-2004  Fixed cast problem with nez mcsLOGICAL enumerate
*
******************************************************************************/

static char *rcsId __attribute__ ((unused)) ="@(#) $Id: vobsPARSER.cpp,v 1.31 2008-04-10 14:19:25 lafrasse Exp $"; 

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
#include "msg.h"

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
                                const char *catalogName,
                                vobsSTAR_LIST &starList,
                                const char *logFileName)
{    
    logTrace("vobsPARSER::MainParser()");	
    GdomeDOMImplementation *domimpl;
    GdomeDocument          *doc;
    GdomeElement           *root;
    GdomeException         exc;
    
    vobsCDATA cData;

    // Set the catalog name
    cData.SetCatalogName(catalogName);

    // Get a DOMImplementation reference
    domimpl = gdome_di_mkref ();

    // Load a new document from the URI
    logTest("Get XML document from '%.4096s'", uri);

    // Create a misco buffer to store CDS results
    miscoDYN_BUF completeReturnBuffer;

    // Temporary 10MByte buffer
    char returnString[10 * 1024 * 1024];

    // Query the CDS
    logTrace("URI = %s", uri);
    if (miscPerformHttpGet(uri, returnString, sizeof(returnString), vobsTIME_OUT) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    completeReturnBuffer.AppendString(returnString);
    
    // XML parsing of the CDS answer
    doc = gdome_di_createDocFromMemory(domimpl,
                                       completeReturnBuffer.GetBuffer(),
                                       GDOME_LOAD_PARSING, &exc);
    if (doc == NULL) 
    {
        errAdd(vobsERR_GDOME_CALL, "gdome_di_createDocFromURI", exc);
        // free gdome object
        gdome_doc_unref (doc, &exc);
        gdome_di_unref (domimpl, &exc);
        xmlCleanupParser();
        return mcsFAILURE;
    }

    // Get reference to the root element of the document
    root = gdome_doc_documentElement (doc, &exc);
    if (root == NULL)
    {
        errAdd(vobsERR_GDOME_CALL, "gdome_doc_documentElement", exc);
        // free gdome object
        gdome_el_unref(root, &exc);            
        gdome_doc_unref (doc, &exc);
        gdome_di_unref (domimpl, &exc);
        xmlCleanupParser();
        return mcsFAILURE;
    }

    // Begin the recursif look of the tree
    if (ParseXmlSubTree((GdomeNode *)root, &cData) == mcsFAILURE)
    {
        // free gdome object
        gdome_el_unref(root, &exc);            
        gdome_doc_unref (doc, &exc);
        gdome_di_unref (domimpl, &exc);
        xmlCleanupParser();
        return mcsFAILURE;
    }

    // Print out CDATA description and Save xml file
    if (logGetStdoutLogLevel() >= logDEBUG)
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

        const char   *resolvedPath;
        // Resolve path
        resolvedPath = miscResolvePath(xmlFileName);
        if (resolvedPath != NULL)
        {
            if (completeReturnBuffer.SaveInFile(resolvedPath) == mcsFAILURE)
            {
                return mcsFAILURE;
            }
        }

        logDebug("CDATA description");
        logDebug("   Number of lines to be skipped : %d", 
                cData.GetNbLinesToSkip());
        logDebug("   Number of parameters in table : %d", cData.GetNbParams());
        char *paramName;
        char *ucdName;
        for (unsigned int i = 0; i < cData.GetNbParams(); i++)
        {
            // Table header
            if (i == 0)
            {
                logDebug("   +----------+--------------+--------------------+");
                logDebug("   | Param #  | Name         | UCD1               |");
                logDebug("   +----------+--------------+--------------------+");
            }
            // Get the parameter name and UCD
            if (cData.GetNextParamDesc(&paramName, &ucdName,
                                       (mcsLOGICAL)(i==0)) == mcsFAILURE)
            {
                return mcsFAILURE;
            }

            logDebug("   |      %3d | %12s | %18s |",
                    i+1, paramName, ucdName);
            ++paramName;
            ++ucdName;

            // Table footer
            if (i == (cData.GetNbParams() -1))
            {
                logDebug("   +----------+--------------+--------------------+");
            }
        }
    }

    // If CDATA section has been found
    if (cData.GetNbLines() != 0)
    {
        // Save CDATA (if requested)
        if ((logFileName != NULL) && (miscIsSpaceStr(logFileName) == mcsFALSE))
        {
            cData.SaveInFile(logFileName);
            errCloseStack();
        }

        // Parse the CDATA section
        starList.Clear();
        vobsSTAR star;

        // Because the lines to be skipped have been removed when appending
        // lines, there is no more line to skip.
        cData.SetNbLinesToSkip(0);
        if (cData.Extract(star, starList) == mcsFAILURE)
        {
            // free gdome object
            gdome_el_unref(root, &exc);            
            gdome_doc_unref (doc, &exc);
            gdome_di_unref (domimpl, &exc);
            xmlCleanupParser();
            starList.Clear();
            return mcsFAILURE;
        }
    }

    // Free the document structure and the DOMImplementation
    gdome_el_unref(root, &exc);            
    gdome_doc_unref (doc, &exc);
    gdome_di_unref (domimpl, &exc);
    xmlCleanupParser();
      
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
mcsCOMPL_STAT vobsPARSER::ParseXmlSubTree(GdomeNode *node,
                                          vobsCDATA *cData)
{
    GdomeException exc;
    GdomeNodeList *nodeList;
    unsigned long nbChildren;
    GdomeDOMString *nodeName;
    GdomeNode *child;
    GdomeNamedNodeMap *attrList;
    unsigned long nbAttrs;
    GdomeDOMString *attrName;
    GdomeDOMString *attrValue;
    GdomeNode *attr;

    logTrace("vobsPARSER:ParseXmlSubTree()");

    // Get the node list containing all children of this node
    nodeList = gdome_n_childNodes (node, &exc);
    if (exc != GDOME_NOEXCEPTION_ERR)
    {
        errAdd(vobsERR_GDOME_CALL, "gdome_n_childNodes", exc);
        // free gdome object
        gdome_nl_unref(nodeList, &exc);
        return mcsFAILURE;
    }

    // Get the number of children
    nbChildren = gdome_nl_length (nodeList, &exc);
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
        child = gdome_nl_item (nodeList, i, &exc);
        if (child == NULL) 
        {
            errAdd(vobsERR_GDOME_CALL, "gdome_nl_item", exc);
            // free gdome object
            gdome_n_unref(child, &exc);
            gdome_nl_unref(nodeList, &exc);
            return mcsFAILURE;
        }

        // If it is the CDATA section
        if (gdome_n_nodeType (child, &exc) == GDOME_CDATA_SECTION_NODE) 
        {
            /* Get CDATA */
            nodeName = gdome_cds_data(GDOME_CDS(child), &exc);
            if (exc != GDOME_NOEXCEPTION_ERR)
            {
                errAdd(vobsERR_GDOME_CALL, "gdome_cds_data", exc);
                // free gdome object
                gdome_str_unref(nodeName);
                gdome_n_unref(child, &exc);
                gdome_nl_unref(nodeList, &exc);
                return mcsFAILURE;
            }
            else
            {
                if (cData->AppendLines(nodeName->str,
                                       cData->GetNbLinesToSkip()) == mcsFAILURE)
                {
                    // free gdome object
                    gdome_str_unref(nodeName);
                    gdome_n_unref(child, &exc);
                    gdome_nl_unref(nodeList, &exc);
                    return mcsFAILURE;
                }
            }
            gdome_str_unref(nodeName);
        }
        // If it is an element node, try to get information on attributes
        else if (gdome_n_nodeType (child, &exc) == GDOME_ELEMENT_NODE) 
        {
            // Get the node name
            nodeName = gdome_n_nodeName(child, &exc);
            if (exc != GDOME_NOEXCEPTION_ERR)
            {
                errAdd(vobsERR_GDOME_CALL, "gdome_n_nodeName", exc);
                // free gdome object
                gdome_str_unref(nodeName);
                gdome_n_unref(child, &exc);
                gdome_nl_unref(nodeList, &exc);
                return mcsFAILURE;
            }
            logDebug("Parsing node %s...", nodeName->str);

            // Get the attributes list
            attrList = gdome_n_attributes(child, &exc);
            if (exc != GDOME_NOEXCEPTION_ERR)
            {
                errAdd(vobsERR_GDOME_CALL, "gdome_n_attributes", exc);
                // free gdome object
                gdome_nnm_unref(attrList, &exc);
                gdome_str_unref(nodeName);
                gdome_n_unref(child, &exc);
                gdome_nl_unref(nodeList, &exc);
                return mcsFAILURE;
            }

            // Get the number of attributes
            nbAttrs = gdome_nnm_length (attrList, &exc);
            if (exc != GDOME_NOEXCEPTION_ERR)
            {
                errAdd(vobsERR_GDOME_CALL, "gdome_nnm_length", exc);
                // free gdome object
                gdome_nnm_unref(attrList, &exc);
                gdome_str_unref(nodeName);
                gdome_n_unref(child, &exc);
                gdome_nl_unref(nodeList, &exc);
                return mcsFAILURE;
            }

            // For each attribute
            for (unsigned int j=0; j < nbAttrs; j++)
            {
                // Get the ith attribute in the node list
                attr = gdome_nnm_item(attrList, j, &exc);
                if (exc != GDOME_NOEXCEPTION_ERR)
                {
                    errAdd(vobsERR_GDOME_CALL, "gdome_nnm_item", exc);
                    // free gdome object
                    gdome_n_unref(attr, &exc);
                    gdome_nnm_unref(attrList, &exc);
                    gdome_str_unref(nodeName);
                    gdome_n_unref(child, &exc);
                    gdome_nl_unref(nodeList, &exc);
                    return mcsFAILURE;
                }
                else
                {
                    // Get the attribute name
                    attrName = gdome_n_nodeName(attr, &exc);
                    if (exc != GDOME_NOEXCEPTION_ERR)
                    {
                        errAdd(vobsERR_GDOME_CALL, "gdome_n_nodeName", exc);
                        // free gdome object
                        gdome_str_unref(attrName);
                        gdome_n_unref(attr, &exc);
                        gdome_nnm_unref(attrList, &exc);
                        gdome_str_unref(nodeName);
                        gdome_n_unref(child, &exc);
                        gdome_nl_unref(nodeList, &exc);
                        return mcsFAILURE;
                    }

                    // Get the attribute name
                    attrValue = gdome_n_nodeValue(attr, &exc);
                    if (exc != GDOME_NOEXCEPTION_ERR)
                    {
                        errAdd(vobsERR_GDOME_CALL, "gdome_n_nodeValue", exc);
                        // free gdome object
                        gdome_str_unref(attrValue);
                        gdome_str_unref(attrName);
                        gdome_n_unref(attr, &exc);
                        gdome_nnm_unref(attrList, &exc);
                        gdome_str_unref(nodeName);
                        gdome_n_unref(child, &exc);
                        gdome_nl_unref(nodeList, &exc);
                        return mcsFAILURE;
                    }

                    // If it is the name a parameter of CDATA 
                    if ((strcmp(nodeName->str, "FIELD") == 0) &&
                        (strcmp(attrName->str, "name")  == 0))
                    {
                        cData->AddParamName(attrValue->str); 
                    }

                    // If it is the UCD name of the corresponding
                    // parameter
                    if ((strcmp(nodeName->str, "FIELD") == 0) &&
                        (strcmp(attrName->str, "ucd1")  == 0))
                    {
                        cData->AddUcdName(attrValue->str); 
                    }

                    // If it is the number of lines to be skipped
                    // before accessing to data in CDATA table 
                    // NOTE: Skip one line more than the value given by
                    // CDS because the CDATA buffer always contains
                    // an empty line at first.
                    if ((strcmp(nodeName->str, "CSV") == 0) &&
                        (strcmp(attrName->str, "headlines")  == 0))
                    {
                        cData->SetNbLinesToSkip(atoi(attrValue->str) + 1); 
                    }
                    // free gdome object
                    gdome_str_unref(attrValue);                    
                    gdome_str_unref(attrName);                    
                }
                // free gdome object
                gdome_n_unref(attr, &exc);                
            }
            // If there are children nodes, parse corresponding XML sub-tree
            if (gdome_n_hasChildNodes (child, &exc))
            {
                if (ParseXmlSubTree(child, cData) == mcsFAILURE)
                {
                    // free gdome object
                    gdome_str_unref(nodeName);
                    gdome_n_unref(child, &exc);
                    gdome_nl_unref(nodeList, &exc);
                    return mcsFAILURE;
                }
            }
            // free gdome object
            gdome_str_unref(nodeName);
            gdome_nnm_unref(attrList, &exc);            
        }
        // Free child instance
        if (exc != GDOME_NOEXCEPTION_ERR)
        {
            errAdd(vobsERR_GDOME_CALL, "gdome_n_unref", exc);
            // free gdome object
            gdome_nl_unref(nodeList, &exc);
            gdome_n_unref (child, &exc);
            return mcsFAILURE;
        }
        // free gdome object
        gdome_n_unref(child, &exc);        
    }

    // free gdome object
    gdome_nl_unref(nodeList, &exc);
    
    return mcsSUCCESS;
}

/*___oOo___*/
