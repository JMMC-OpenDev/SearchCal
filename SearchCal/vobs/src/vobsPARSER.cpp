/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsPARSER.cpp,v 1.7 2005-01-27 13:45:30 scetre Exp $"
*
* History
* -------
* $Log: not supported by cvs2svn $
* Revision 1.6  2005/01/26 08:11:28  scetre
* change history
*
* scetre    06-Jul-2004  Created
* gzins     09-Dec-2004  Fixed cast problem with nez mcsLOGICAL enumerate
*
******************************************************************************/

static char *rcsId="@(#) $Id: vobsPARSER.cpp,v 1.7 2005-01-27 13:45:30 scetre Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

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
 * \param uri URI from where XML document has to be loaded.
 * \param starList list where star has to be put.
 * \param logFileName file to save the result of the asking
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned and
 * an error is added to the error stack. The possible errors are :
 * \li vobsERR_GDOME_CALL
 */
mcsCOMPL_STAT vobsPARSER::Parse(char *uri,
                                vobsSTAR_LIST &starList,
                                char* logFileName)
{    
    GdomeDOMImplementation *domimpl;
    GdomeDocument          *doc;
    GdomeElement           *root;
    GdomeException         exc;
    vobsSTAR_LIST starListToReturn;
    
    vobsCDATA cData;
    logExtDbg("vobsPARSER::MainParser()");	

    // Get a DOMImplementation reference
    domimpl = gdome_di_mkref ();

    // Load a new document from the URI
    logTest("Get XML document from '%s'", uri);
    doc = gdome_di_createDocFromURI(domimpl, uri, GDOME_LOAD_PARSING, &exc);
    if (doc == NULL) 
    {
        errAdd(vobsERR_GDOME_CALL, "gdome_di_createDocFromURI", exc);
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
        gdome_el_unref(root, &exc);            
        gdome_doc_unref (doc, &exc);
        gdome_di_unref (domimpl, &exc);
        xmlCleanupParser();
        return mcsFAILURE;
    }

    // Begin the recursif look of the tree
    if (ParseXmlSubTree((GdomeNode *)root, &cData) == mcsFAILURE)
    {
        gdome_el_unref(root, &exc);            
        gdome_doc_unref (doc, &exc);
        gdome_di_unref (domimpl, &exc);
        xmlCleanupParser();
        return mcsFAILURE;
    }

    // Print out CDATA description
    if (logGetStdoutLogLevel() >= logTEST)
    {
        logTest("CDATA description");
        logTest("    Number of lines to be skipped : %d", 
                cData.getNbLinesToSkip());
        logTest("    Number of columns in table    : %d", cData.getNbColumns());
        char *colName;
        char *ucdName;
        for (unsigned int i = 0; i < cData.getNbColumns(); i++)
        {
            // Table header
            if (i == 0)
            {
                logTest("    +----------+--------------+--------------------+");
                logTest("    | Column # | Name         | UCD                |");
                logTest("    +----------+--------------+--------------------+");
            }
            // Get the column name and UCD
            if (cData.getNextColDesc(&colName, 
                                     &ucdName, (mcsLOGICAL)(i==0)) == mcsFAILURE)
            {
                return mcsFAILURE;
            }

            logTest("    |      %3d | %12s | %18s |",
                    i+1, colName, ucdName);
            ++colName;
            ++ucdName;

            // Table footer
            if (i == (cData.getNbColumns() -1))
            {
                logTest("    +----------+--------------+--------------------+");
            }
        }
    }

    // If CDATA section has been found
    if (cData.getNbLines() != 0)
    {
        // Save CDATA (if requested)
        if (logFileName != NULL)
        {
            cData.save(logFileName);
        }

        // Parse the CDATA section
        if (ParseCData(&cData, starListToReturn) == mcsFAILURE)
        {
            gdome_el_unref(root, &exc);            
            gdome_doc_unref (doc, &exc);
            gdome_di_unref (domimpl, &exc);
            xmlCleanupParser();
            return mcsFAILURE;
        }
    }

    // Free the document structure and the DOMImplementation
    gdome_el_unref(root, &exc);            
    gdome_doc_unref (doc, &exc);
    gdome_di_unref (domimpl, &exc);
    xmlCleanupParser();
      
    starList.Clear();
    starList.Copy(starListToReturn); 
    
    return mcsSUCCESS;
}

/**
 * Parse recursively XML document.
 *
 * This method recursively parses the XML document (i.e. node by node), and
 * extract informations related to the CDATA section which contains the star
 * table. These informations are the name of the columns with the
 * corresponding UCD, the number lines to be skipped in CDATA section and the
 * pointer to the CDATA buffer (see vobsCDATA data structure).
 * Informations about columns are given by 'FIELD' node; 'name' attribute for
 * the column name and 'UCD' for UCD, and the number of line to skip is given
 * by 'CSV' node and 'headlines' attribute.
 *
 * \param node  XML document node to be parsed. 
 * \param cData data structure where CDATA description has to be stored.
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned and
 * an error is added to the error stack. The possible error is :
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

    logExtDbg("vobsPARSER:ParseXmlSubTree()");

    // Get the node list containing all children of this node
    nodeList = gdome_n_childNodes (node, &exc);
    if (exc != GDOME_NOEXCEPTION_ERR)
    {
        errAdd(vobsERR_GDOME_CALL, "gdome_n_childNodes", exc);
        gdome_nl_unref(nodeList, &exc);
        return mcsFAILURE;
    }

    // Get the number of children
    nbChildren = gdome_nl_length (nodeList, &exc);
    if (exc != GDOME_NOEXCEPTION_ERR)
    {
        errAdd(vobsERR_GDOME_CALL, "gdome_nl_length", exc);
        gdome_nl_unref(nodeList, &exc);
        return mcsFAILURE;
    }

    // If there is no child; return
    if (nbChildren == 0)
    {
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
                gdome_str_unref(nodeName);
                gdome_n_unref(child, &exc);
                gdome_nl_unref(nodeList, &exc);
                return mcsFAILURE;
            }
            else
            {
                if (cData->appendLines(nodeName->str) == mcsFAILURE)
                {
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
                gdome_str_unref(nodeName);
                gdome_n_unref(child, &exc);
                gdome_nl_unref(nodeList, &exc);
                return mcsFAILURE;
            }
            logTest("Parsing node %s...", nodeName->str);

            // Get the attributes list
            attrList = gdome_n_attributes(child, &exc);
            if (exc != GDOME_NOEXCEPTION_ERR)
            {
                errAdd(vobsERR_GDOME_CALL, "gdome_n_attributes", exc);
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
                        gdome_str_unref(attrValue);
                        gdome_str_unref(attrName);
                        gdome_n_unref(attr, &exc);
                        gdome_nnm_unref(attrList, &exc);
                        gdome_str_unref(nodeName);
                        gdome_n_unref(child, &exc);
                        gdome_nl_unref(nodeList, &exc);
                        return mcsFAILURE;
                    }

                    // If it is the name of the column table of CDATA 
                    if ((strcmp(nodeName->str, "FIELD") == 0) &&
                        (strcmp(attrName->str, "name")  == 0))
                    {
                        cData->addColName(attrValue->str); 
                    }

                    // If it is the UCD name of the corresponding
                    // column table  
                    if ((strcmp(nodeName->str, "FIELD") == 0) &&
                        (strcmp(attrName->str, "ucd")  == 0))
                    {
                        cData->addUcdName(attrValue->str); 
                    }

                    // If it is the number of lines to be skipped
                    // before accessing to data in CDATA table 
                    if ((strcmp(nodeName->str, "CSV") == 0) &&
                        (strcmp(attrName->str, "headlines")  == 0))
                    {
                        cData->setNbLinesToSkip(atoi(attrValue->str)); 
                    }
                    gdome_str_unref(attrValue);                    
                    gdome_str_unref(attrName);                    
                }
                gdome_n_unref(attr, &exc);                
            }
            // If there are children nodes, parse corresponding XML sub-tree
            if (gdome_n_hasChildNodes (child, &exc))
            {
                if (ParseXmlSubTree(child, cData) == mcsFAILURE)
                {
                    gdome_str_unref(nodeName);
                    gdome_n_unref(child, &exc);
                    gdome_nl_unref(nodeList, &exc);
                    return mcsFAILURE;
                }
            }
            gdome_str_unref(nodeName);
            gdome_nnm_unref(attrList, &exc);            
        }
        // Free child instance
        if (exc != GDOME_NOEXCEPTION_ERR)
        {
            errAdd(vobsERR_GDOME_CALL, "gdome_n_unref", exc);
            gdome_nl_unref(nodeList, &exc);
            gdome_n_unref (child, &exc);
            return mcsFAILURE;
        }
        gdome_n_unref(child, &exc);        
    }

    gdome_nl_unref(nodeList, &exc);
    
    return mcsSUCCESS;
}

/**
 * Parse the CDATA section of XML document.
 *
 * This method parses the CDATA section defined by the \em cData parameters.
 * The CDATA section contains a list of stars which are stored in a table
 * where each line corresponds to a star; i.e. all star properties are placed
 * in one line, and separated by the '\\t' character. The list of star
 * properties to be extracted is given by \em ucd.ucdName list.
 *
 * The first lines of the CDATA containing the description of the column table
 * has to be skipped. The number of lines to be skipped is given by \em
 * ucd.nbLineToJump 
 *
 * The found stars are put in the \em starList parameter. 
 * 
 * \param cData    data structure describing the CDATA section.
 * \param starList list where star has to be put.
 * 
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned and
 * an error is added to the error stack. The possible error is :
 * \li vobsERR_INVALID_CDATA_FORMAT
 */
mcsCOMPL_STAT vobsPARSER::ParseCData(vobsCDATA *cData,
                                     vobsSTAR_LIST &starList)
{
    logExtDbg("vobsPARSER::ParseCData()");  

    char *linePtr=NULL;

    // For each line in buffer, get the value for each defined UCD (value are
    // separated by '\t' character), store them in star object and add this
    // new star in the list.
    char *delimiters = "\t";
    do
    {
        // Get next line
        linePtr = cData->getNextLine(linePtr);
        
        if (linePtr != NULL)
        {
            char line[1024];
            int  nbUcd;
            char *colName;
            char *ucdName;
            char *ucdValue;
            vobsSTAR star;

            // Copy line into temporary buffer
            strcpy(line, linePtr);

            // Number of UCDs per line
            nbUcd = cData->getNbColumns();

            // Scan UCD list
            char *nextLinePtr;
            char *currLinePtr=line;

            // temporary variable to parse in case of II/225
            mcsSTRING256 lambda;
            mcsFLOAT lambdaFloat;
            mcsSTRING256 flux;
            strcpy(lambda, "");
            strcpy(flux, "");

            for (int j=0; j < nbUcd; j++)
            {
                // Get the column name and UCD
                if (cData->getNextColDesc(&colName, 
                                          &ucdName,
                                          (mcsLOGICAL)(j==0)) == mcsFAILURE)
                {
                    return mcsFAILURE;
                }

                // Get the UCD value
                ucdValue = strtok_r(currLinePtr, delimiters, &nextLinePtr);
                if (ucdValue == NULL)
                {
                    // End of line reached; stop UCD scan
                    break;
                }
                // If lambda is found, memorise it
                else if (strcmp(ucdName, vobsSTAR_INST_WAVELENGTH_VALUE) == 0)
                {
                    currLinePtr = nextLinePtr;
                    strcpy(lambda, ucdValue); 
                }
                // if flux is found, memorise it
                else if (strcmp(ucdName, vobsSTAR_PHOT_FLUX_IR_MISC) == 0)
                {
                    currLinePtr = nextLinePtr;
                    strcpy(flux, ucdValue);
                }
                else
                {
                    currLinePtr = nextLinePtr;

                    // Check if value if empty
                    if (miscIsSpaceStr(ucdValue) == mcsTRUE)
                    {
                        ucdValue = vobsSTAR_PROP_NOT_SET;
                    }

                    // Set star property
                    if (star.SetPropertyValue(ucdName, ucdValue) == mcsFAILURE)
                    {
                        // If ucd is not found, ignore error
                        if (errIsInStack(MODULE_ID, 
                                         vobsERR_INVALID_PROPERTY_ID)
                            == mcsTRUE)
                        {
                            errResetStack();
                        }
                        else
                        {
                            return mcsFAILURE;
                        }
                    }
                }
                // if wavelength and flux has been found and memorise
                if ((strcmp(lambda, "") != 0) && (strcmp(flux, "") != 0))
                {
                    // convert lambda from str to numerical value
                    if (sscanf(lambda, "%f" , &lambdaFloat) == 1)
                    {
                        logTest("we will compute a mag\n");
                        logTest("lbd = %f and flux = %s\n", lambdaFloat, flux);
                        // if the wvl is 1.25, the flux is mgJ
                        if (lambdaFloat == (mcsFLOAT)1.25)
                        {
                            logTest("J = %s\n", flux);
                            if (star.SetPropertyValue(vobsSTAR_PHOT_JHN_J,
                                                      flux) == mcsFAILURE)
                            {
                                // If ucd is not found, ignore error
                                if (errIsInStack(MODULE_ID, 
                                                 vobsERR_INVALID_PROPERTY_ID)
                                    == mcsTRUE)
                                {
                                    errResetStack();
                                }
                                else
                                {
                                    return mcsFAILURE;
                                }
                            }
                        }
                        // if wvl is 1.65, the flux is mgH
                        if (lambdaFloat == (mcsFLOAT)1.65)
                        {
                            logTest("H = %s\n", flux);
                            if (star.SetPropertyValue(vobsSTAR_PHOT_JHN_H,
                                                      flux) == mcsFAILURE)
                            {
                                // If ucd is not found, ignore error
                                if (errIsInStack(MODULE_ID, 
                                                 vobsERR_INVALID_PROPERTY_ID)
                                    == mcsTRUE)
                                {
                                    errResetStack();
                                }
                                else
                                {
                                    return mcsFAILURE;
                                }
                            }
                        }
                        // if wvl is 2.20, the flux is mgK
                        if (lambdaFloat == (mcsFLOAT)2.20)
                        {
                            logTest("K = %s\n", flux);
                            if (star.SetPropertyValue(vobsSTAR_PHOT_JHN_K,
                                                      flux) == mcsFAILURE)
                            {
                                // If ucd is not found, ignore error
                                if (errIsInStack(MODULE_ID, 
                                                 vobsERR_INVALID_PROPERTY_ID)
                                    == mcsTRUE)
                                {
                                    errResetStack();
                                }
                                else
                                {
                                    return mcsFAILURE;
                                }
                            }
                        }
                    }
                }
            }
            // Put now the star in the star list
            if (starList.AddAtTail(star) == mcsFAILURE)
            {
                return mcsFAILURE;
            }
        }
    } while (linePtr != NULL);

    return mcsSUCCESS;
}

/*___oOo___*/
