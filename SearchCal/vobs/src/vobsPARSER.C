/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsPARSER.C,v 1.9 2004-09-07 11:56:53 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    06-Jul-2004  Created
*
*******************************************************************************/

static char *rcsId="@(#) $Id: vobsPARSER.C,v 1.9 2004-09-07 11:56:53 scetre Exp $"; 
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
 *
 * \return SUCCESS on successful completion. Otherwise FAILURE is returned and
 * an error is added to the error stack. The possible errors are :
 * \li vobsERR_GDOME_CALL
 * \li vobsERR_CDATA_NOT_FOUND
 */
mcsCOMPL_STAT vobsPARSER::Parse(char *uri, vobsSTAR_LIST &starList)
{
    GdomeDOMImplementation *domimpl;
    GdomeDocument          *doc;
    GdomeElement           *root;
    GdomeException         exc;
    vobsCDATA              cData;
    vobsSTAR_LIST starListToReturn;
        
    std::vector<vobsCDATA *> listCDATA;
    
    logExtDbg("vobsPARSER::MainParser()");	

    // Get a DOMImplementation reference
    domimpl = gdome_di_mkref ();

    // Load a new document from the URI
    logTest("Get XML document from '%s'", uri);
    doc = gdome_di_createDocFromURI(domimpl, uri, GDOME_LOAD_PARSING, &exc);
    if (doc == NULL) 
    {
        errAdd(vobsERR_GDOME_CALL, "gdome_di_createDocFromURI", exc);
        return FAILURE;
    }

    // Get reference to the root element of the document
    root = gdome_doc_documentElement (doc, &exc);
    if (root == NULL)
    {
        errAdd(vobsERR_GDOME_CALL, "gdome_doc_documentElement", exc);
        gdome_di_freeDoc (domimpl, doc, &exc);
        gdome_di_unref (domimpl, &exc);
        return FAILURE;
    }

    // Begin the recursif look of the tree
    memset(&cData, '\0', sizeof(cData)); 
    if (ParseXmlSubTree((GdomeNode *)root, listCDATA, &cData) == FAILURE)
    {
        gdome_di_freeDoc (domimpl, doc, &exc);
        gdome_di_unref (domimpl, &exc);
        return FAILURE;
    }
    std::vector<vobsCDATA *>::iterator iterCDATA=listCDATA.begin();
    while (iterCDATA!=listCDATA.end())
    {
        // Print out CDATA description
        if (logGetStdoutLogLevel() >= logTEST)
        {
            logTest("CDATA description");
            logTest("    Number of lines to be skipped : %d", (**iterCDATA).nbLineToJump);
            logTest("    Number of columns in table    : %d", (**iterCDATA).colName.size());
            std::vector<char *>::iterator colName=(**iterCDATA).colName.begin();
            std::vector<char *>::iterator ucdName=(**iterCDATA).ucdName.begin();
            for (unsigned int i = 0; i < (**iterCDATA).colName.size(); i++)
            {
                // Table header
                if (i == 0)
                {
                    logTest("    +----------+--------------+--------------------+");
                    logTest("    | Column # | Name         | UCD                |");
                    logTest("    +----------+--------------+--------------------+");
                }
                logTest("    |      %3d | %12s | %18s |",
                        i+1, *colName, *ucdName);
                ++colName;
                ++ucdName;

                // Table footer
                if (i == ((**iterCDATA).colName.size() -1))
                {
                    logTest("    +----------+--------------+--------------------+");
                }
            }
        }
        ++iterCDATA;
    }
    
    
    // If CDATA section has not be found
    if (listCDATA.size() == 0)
    {
        // Handle error
        errAdd(vobsERR_CDATA_NOT_FOUND);
        gdome_di_freeDoc (domimpl, doc, &exc);
        gdome_di_unref (domimpl, &exc);
        //return FAILURE;
    }
    
    else
    {
        iterCDATA=listCDATA.begin();
        int i=1;
        while (iterCDATA!=listCDATA.end())
        {
            //printf("%deme passage\n",i);
            //printf("%s\n\n",(**iterCDATA).ptr);
            // Parse the CDATA section
            if (ParseCData((*iterCDATA), starListToReturn) == FAILURE)
            {
                gdome_di_freeDoc (domimpl, doc, &exc);
                gdome_di_unref (domimpl, &exc);
                return FAILURE;
            }
            ++i;
            ++iterCDATA;
        }
        // Free the document structure and the DOMImplementation
        gdome_di_freeDoc (domimpl, doc, &exc);
        gdome_di_unref (domimpl, &exc);

        // Print out the star list 
        /*if (logGetStdoutLogLevel() >= logTEST)
        {
            starList.Display();
        }*/

    }

    starList.Clear();
    starList.Copy(starListToReturn);
    
    return SUCCESS;
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
 * \return SUCCESS on successful completion. Otherwise FAILURE is returned and
 * an error is added to the error stack. The possible error is :
 * \li vobsERR_GDOME_CALL
 *
 * \todo
 * nblinetojump problem
 */
mcsCOMPL_STAT vobsPARSER::ParseXmlSubTree(GdomeNode *node, std::vector<vobsCDATA *> &listCDATA, vobsCDATA *cData)
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
    
    //cData->nbLineToJump=0;
    //printf("1 nb line to jump = %d\n",cData->nbLineToJump);
    //memset(&cData, '\0', sizeof(cData)); 
    
    logExtDbg("vobsPARSER:ParseXmlSubTree()");

    // Get the node list containing all children of this node
    nodeList = gdome_n_childNodes (node, &exc);
    if (exc != GDOME_NOEXCEPTION_ERR)
    {
        errAdd(vobsERR_GDOME_CALL, "gdome_n_childNodes", exc);
        return FAILURE;
    }

    // Get the number of children
    nbChildren = gdome_nl_length (nodeList, &exc);
    if (exc != GDOME_NOEXCEPTION_ERR)
    {
        errAdd(vobsERR_GDOME_CALL, "gdome_nl_length", exc);
        return FAILURE;
    }

    // If there is no child; return
    if (nbChildren == 0)
    {
        return SUCCESS;
    }

    // For each child
    for (unsigned int i = 0; i < nbChildren; i++)
    {
        // Get the ith child in the node list
        child = gdome_nl_item (nodeList, i, &exc);
        if (child == NULL) 
        {
            errAdd(vobsERR_GDOME_CALL, "gdome_nl_item", exc);
            return FAILURE;
        }

        // If it is the CDATA section
        if (gdome_n_nodeType (child, &exc) == GDOME_CDATA_SECTION_NODE) 
        {
            vobsCDATA *cDataForList=new vobsCDATA;
            cDataForList->colName=cData->colName;
            cDataForList->ucdName=cData->ucdName;
            cDataForList->nbLineToJump=cData->nbLineToJump;     

            /* Get CDATA */
            cDataForList->ptr = new char[strlen(gdome_cds_data(GDOME_CDS(child), &exc)->str)+1];
            strcpy(cDataForList->ptr, gdome_cds_data(GDOME_CDS(child), &exc)->str);
            if (cDataForList->ptr == NULL)
            {
                errAdd(vobsERR_GDOME_CALL, "gdome_cds_data", exc);
                return FAILURE;
            }
            listCDATA.push_back(cDataForList);
        }

        // If it is an element node, try to get information on attributes
        if (gdome_n_nodeType (child, &exc) == GDOME_ELEMENT_NODE) 
        {
            // Get the node name
            nodeName = gdome_n_nodeName(child, &exc);
            if (exc != GDOME_NOEXCEPTION_ERR)
            {
                errAdd(vobsERR_GDOME_CALL, "gdome_n_nodeName", exc);
                return FAILURE;
            }
            logTest("Parsing node %s...", nodeName->str);
            
            // Get the attributes list
            attrList = gdome_n_attributes(child, &exc);
            if (exc != GDOME_NOEXCEPTION_ERR)
            {
                errAdd(vobsERR_GDOME_CALL, "gdome_n_attributes", exc);
                return FAILURE;
            }

            // Get the number of attributes
            nbAttrs = gdome_nnm_length (attrList, &exc);
            if (exc != GDOME_NOEXCEPTION_ERR)
            {
                errAdd(vobsERR_GDOME_CALL, "gdome_nnm_length", exc);
                return FAILURE;
            }

            // For each attribute
            for (unsigned int j=0; j < nbAttrs; j++)
            {
                // Get the ith attribute in the node list
                attr = gdome_nnm_item(attrList, j, &exc);
                if (exc != GDOME_NOEXCEPTION_ERR)
                {
                    errAdd(vobsERR_GDOME_CALL, "gdome_nnm_item", exc);
                    return FAILURE;
                }
                else
                {
                    // Get the attribute name
                    attrName = gdome_n_nodeName(attr, &exc);
                    if (exc != GDOME_NOEXCEPTION_ERR)
                    {
                        errAdd(vobsERR_GDOME_CALL, "gdome_n_nodeName", exc);
                        return FAILURE;
                    }

                    // Get the attribute name
                    attrValue = gdome_n_nodeValue(attr, &exc);
                    if (exc != GDOME_NOEXCEPTION_ERR)
                    {
                        errAdd(vobsERR_GDOME_CALL, "gdome_n_nodeValue", exc);
                        return FAILURE;
                    }
                    
                    // If it is the name of the column table of CDATA 
                    if ((strcmp(nodeName->str, "FIELD") == 0) &&
                        (strcmp(attrName->str, "name")  == 0))
                    {
                        cData->colName.push_back(attrValue->str); 
                    }

                    // If it is the UCD name of the corresponding
                    // column table  
                    if ((strcmp(nodeName->str, "FIELD") == 0) &&
                        (strcmp(attrName->str, "ucd")  == 0))
                    {
                        cData->ucdName.push_back(attrValue->str); 
                    }

                    // If it is the number of lines to be skipped
                    // before accessing to data in CDATA table 
                    if ((strcmp(nodeName->str, "CSV") == 0) &&
                        (strcmp(attrName->str, "headlines")  == 0))
                    {
                        cData->nbLineToJump = atoi(attrValue->str);
                    }
                }
            }
            // If there are children nodes, parse corresponding XML sub-tree
            if (gdome_n_hasChildNodes (child, &exc))
            {
                if (ParseXmlSubTree(child, listCDATA, cData) == FAILURE)
                {
                    return FAILURE;
                }
            }
        }
        // Free child instance
        gdome_n_unref (child, &exc);
        if (exc != GDOME_NOEXCEPTION_ERR)
        {
            errAdd(vobsERR_GDOME_CALL, "gdome_n_unref", exc);
            return FAILURE;
        }
    }
    return SUCCESS;
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
 * \return SUCCESS on successful completion. Otherwise FAILURE is returned and
 * an error is added to the error stack. The possible error is :
 * \li vobsERR_INVALID_CDATA_FORMAT
 */
mcsCOMPL_STAT vobsPARSER::ParseCData(vobsCDATA *cData, vobsSTAR_LIST &starList)
{
    logExtDbg("vobsPARSER::ParseCData()"); 

    char *linePtr;
    miscDYN_BUF linePtrList; // Table containing pointers to the CDATA lines
    int nbLines;             // Number of the in CDATA

    // Allocate memory to store pointers to the lines of the CDATA buffer
    if (miscDynBufAlloc(&linePtrList, 256*sizeof(char*)) == FAILURE)
    {
        return FAILURE;
    }
    // Find all lines of the CDATA buffer (terminated by CR), convert them in
    // individual string (i.e. replace CR by '\0') and place the corresponding
    // pointer in line pointer table
    int cDataLength;
    cDataLength = strlen(cData->ptr);
    nbLines = 0;
    linePtr = cData->ptr;
    
    for (int i=0; i < cDataLength; i++)
    {       
        if (cData->ptr[i] == '\n')
        {
            // Replace CR by '\0' in order to have string
            cData->ptr[i] = '\0';
           
            miscDynBufAppendBytes(&linePtrList, 
                                  (char *)&linePtr, sizeof(char*));
            nbLines++;
            // Set line pointer to the next line
            linePtr = &cData->ptr[i] + 1;
        }
    }
    
    // For each line in buffer, get the value for each defined UCD (value are
    // separated by '\t' character), store them in star object and add this
    // new star in the list.
    // NOTE: Skip one line more than the value given by nbLineToJump because
    // the CDATA buffer always contains an empty line at first.
    char **linePtrTab;
    char *delimiters = "\t";
    std::vector<char *>::iterator ucdName;
    linePtrTab = (char **)miscDynBufGetBufferPointer(&linePtrList);
    for (int i = (cData->nbLineToJump+1); i < nbLines; i++)
    {
        int  nbUcd;
        char *ucdValue;
        vobsSTAR star;

        // Number of UCDs per line
        nbUcd = cData->ucdName.size();

        // Scan UCD list
        char *nextLinePtr;
        linePtr = linePtrTab[i];
        ucdName = cData->ucdName.begin();
        for (int j=0; j < nbUcd; j++)
        {
            // Get the UCD value
            ucdValue = strtok_r(linePtr, delimiters, &nextLinePtr);
            if (ucdValue == NULL)
            {
                errAdd(vobsERR_INVALID_CDATA_FORMAT, linePtr);
                //printf("titi\n");
                //return FAILURE;
            }
            else
            {
            linePtr = nextLinePtr;

            // Check if value if empty
            if (miscIsSpaceStr(ucdValue) == mcsTRUE)
            {
                ucdValue = vobsSTAR_PROP_NOT_SET;
            }

            // Set star property
            if (star.SetProperty(*ucdName, ucdValue) == FAILURE)
            {
                // If ucd is not found, ignore error
                if (errIsInStack(MODULE_ID, 
                                 vobsERR_INVALID_UCD_NAME) == mcsTRUE)
                {
                    errResetStack();
                }
                else
                {
                    return FAILURE;
                }
            }

            // Next UCD
            ucdName++;
            }
        }

        // Put now the star in the star list
        if (starList.AddAtTail(star) == FAILURE)
        {
            return FAILURE;
        }
    }

    miscDynBufDestroy(&linePtrList);
    return SUCCESS;
}

/*___oOo___*/
