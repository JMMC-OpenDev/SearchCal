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
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsPARSER::Parse(vobsSCENARIO_RUNTIME &ctx,
                                const char *uri,
                                const char *data,
                                vobsORIGIN_INDEX catalogId,
                                const vobsCATALOG_META* catalogMeta,
                                vobsSTAR_LIST &starList,
                                vobsCATALOG_STAR_PROPERTY_CATALOG_MAPPING* propertyCatalogMap,
                                const char *logFileName)
{
    const char* catalogName = vobsGetOriginIndex(catalogId);

    logDebug("vobsPARSER::Parse() - catalog '%s'", catalogName);

    // Clear the output star list anyway (even if the query fails):
    starList.Clear();

    // Load a new document from the URI
    logTest("Get XML document from '%s' with POST data '%s'", uri, data);

    // Reset and get the response buffer:
    miscoDYN_BUF* responseBuffer = ctx.GetResponseBuffer();

    // Query the CDS
    FAIL(miscPerformHttpPost(uri, data, responseBuffer->GetInternalMiscDYN_BUF(), vobsTIME_OUT));

    miscDynSIZE storedBytesNb = 0;
    responseBuffer->GetNbStoredBytes(&storedBytesNb);

    logTest("Parsing XML document (%ld bytes)", storedBytesNb);

    char* buffer = responseBuffer->GetBuffer();

    /* EXTRACT CDS ERROR(**** ...) messages into the buffer */
    char* posError = strstr(buffer, "\n****"); /* \n to skip <!--  INFO Diagnostics (++++ are Warnings, **** are Errors) --> */
    if (isNotNull(posError))
    {
        const char* endError = strstr(posError, "-->"); /* --> to go until end of INFO block */

        if (isNull(endError))
        {
            /* Go to buffer end */
            endError = buffer + storedBytesNb;
        }

        mcsUINT32 length = (endError - posError);

        char* errorMsg = new char[length];
        FAIL_DO(responseBuffer->GetBytesFromTo(errorMsg, posError + 2 - buffer, endError - buffer),
                delete errorMsg);

        logError("vobsPARSER::Parse() CDS Errors found {{{\n%s}}}", errorMsg);

        delete errorMsg;
        return mcsFAILURE;
    }

    /* EXTRACT CDS ERROR(<INFO ID="fatalError" name="Error" value="..."/>) messages into the buffer */
    posError = strstr(buffer, "INFO ID=\"fatalError\"");
    if (isNotNull(posError))
    {
        static const char* ATTR_VALUE = "value=";
        const char* posValue = strstr(posError, ATTR_VALUE);

        if (isNull(posValue))
        {
            posValue = posError;
        }
        else
        {
            posValue += strlen(ATTR_VALUE);
        }

        const char* endError = strstr(posValue, "/>"); /* --> to go until end of INFO.value attribute */

        if (isNull(endError))
        {
            /* Go to buffer end */
            endError = buffer + storedBytesNb;
        }

        mcsUINT32 length = (endError - posValue);

        char* errorMsg = new char[length];
        FAIL_DO(responseBuffer->GetBytesFromTo(errorMsg, posValue - buffer, endError - buffer),
                delete errorMsg);

        logError("vobsPARSER::Parse() CDS Errors found {{{\n%s}}}", errorMsg);


        delete errorMsg;
        return mcsFAILURE;
    }

    /* Parse the VOTable */

    mcsLockGdomeMutex();

    // Get a DOMImplementation reference
    GdomeDOMImplementation* domimpl = gdome_di_mkref();

    GdomeException exc;

    // XML parsing of the CDS answer
    GdomeDocument* doc = gdome_di_createDocFromMemory(domimpl, buffer, GDOME_LOAD_PARSING, &exc);

    if (isNull(doc))
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

    if (isNull(root))
    {
        errAdd(vobsERR_GDOME_CALL, "gdome_doc_documentElement", exc);
        // free gdome object
        gdome_el_unref(root, &exc);
        gdome_doc_unref(doc, &exc);
        gdome_di_unref(domimpl, &exc);

        mcsUnlockGdomeMutex();

        return mcsFAILURE;
    }

    // Get the node name
    GdomeDOMString* nodeName = gdome_n_nodeName((GdomeNode *) root, &exc);
    if (exc != GDOME_NOEXCEPTION_ERR)
    {
        errAdd(vobsERR_GDOME_CALL, "gdome_n_nodeName", exc);
        // free gdome object
        gdome_str_unref(nodeName);
        gdome_el_unref(root, &exc);
        gdome_doc_unref(doc, &exc);
        gdome_di_unref(domimpl, &exc);

        mcsUnlockGdomeMutex();

        return mcsFAILURE;
    }

    // Check that the XML document contains one VOTABLE:
    if (strcmp(nodeName->str, "VOTABLE") != 0)
    {
        // Dump the beginning of the XML document in logs:
        logWarning("Incorrect root node '%s' in XML document :\n%s",
                   nodeName->str, buffer);

        // free gdome object
        gdome_str_unref(nodeName);
        gdome_el_unref(root, &exc);
        gdome_doc_unref(doc, &exc);
        gdome_di_unref(domimpl, &exc);

        mcsUnlockGdomeMutex();

        return mcsFAILURE;
    }

    // free gdome object
    gdome_str_unref(nodeName);

    // Create the cData parser;
    vobsCDATA* cData = ctx.GetCDataParser();
    miscoDYN_BUF* dataBuf = ctx.GetDataBuffer();

    // Set the catalog meta data if available:
    if (isNotNull(catalogMeta))
    {
        cData->SetCatalogMeta(catalogMeta);
    }
    else
    {
        cData->SetCatalogId(catalogId);
    }

    // Begin the recursive traversal of the XML tree
    if (ParseXmlSubTree((GdomeNode *) root, cData, dataBuf) == mcsFAILURE)
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
    if ((isNotNull(logFileName) && isFalse(miscIsSpaceStr(logFileName))) || doLog(logDEBUG))
    {
        mcsSTRING32 catalog;
        strcpy(catalog, catalogName);
        FAIL(miscReplaceChrByChr(catalog, '/', '_'));

        mcsSTRING256 xmlFileName;
        strcpy(xmlFileName, "$MCSDATA/tmp/");
        strcat(xmlFileName, catalog);
        strcat(xmlFileName, ".xml");

        char *resolvedPath;
        // Resolve path
        resolvedPath = miscResolvePath(xmlFileName);
        if (isNotNull(resolvedPath))
        {
            logTest("Save XML document to: %s", resolvedPath);

            FAIL_DO(responseBuffer->SaveInASCIIFile(resolvedPath), free(resolvedPath));

            free(resolvedPath);
        }

        mcsUINT32 nbParams = cData->GetNbParams();

        logDebug("CDATA description");
        logDebug("   Number of lines to be skipped : %d", cData->GetNbLinesToSkip());
        logDebug("   Number of parameters in table : %d", nbParams);

        char *paramName;
        char *ucdName;
        for (mcsUINT32 i = 0; i < nbParams; i++)
        {
            // Table header
            if (i == 0)
            {
                logDebug("   +----------+--------------+--------------------+");
                logDebug("   | Param #  | Name         | UCD1               |");
                logDebug("   +----------+--------------+--------------------+");
            }
            // Get the parameter name and UCD
            FAIL(cData->GetNextParamDesc(&paramName, &ucdName, (mcsLOGICAL) (i == 0)));

            logDebug("   |      %3d | %12s | %18s |", i + 1, paramName, ucdName);
            ++paramName;
            ++ucdName;

            // Table footer
            if (i == (nbParams - 1))
            {
                logDebug("   +----------+--------------+--------------------+");
            }
        }
    }

    mcsUINT32 nbLines = cData->GetNbLines();

    // If CDATA section has been found
    if (nbLines != 0)
    {
        // Save CDATA (if requested)
        if (isNotNull(logFileName) && isFalse(miscIsSpaceStr(logFileName)))
        {
            logTest("Save CDATA to: %s", logFileName);

            if (cData->SaveInASCIIFile(logFileName) == mcsFAILURE)
            {
                errCloseStack();
            }
        }

        // Parse the CDATA section

        // Because the lines to be skipped have been removed when appending
        // lines, there is no more line to skip.
        cData->SetNbLinesToSkip(0);

        logTest("Extracting data from CDATA section (%d rows)", nbLines);

        vobsSTAR star;
        if (cData->Extract(star, starList, mcsFALSE, propertyCatalogMap) == mcsFAILURE)
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
 * and an error is added to the error stack. The possible error is:
 * \li vobsERR_GDOME_CALL
 */
mcsCOMPL_STAT vobsPARSER::ParseXmlSubTree(GdomeNode *node, vobsCDATA *cData, miscoDYN_BUF* dataBuf)
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

    GdomeNode* child;
    unsigned short nodeType;

    // For each child
    for (mcsUINT32 i = 0; i < nbChildren; i++)
    {
        // Get the the child in the node list
        child = gdome_nl_item(nodeList, i, &exc);

        if (isNull(child))
        {
            errAdd(vobsERR_GDOME_CALL, "gdome_nl_item", exc);
            // free gdome object
            gdome_n_unref(child, &exc);
            gdome_nl_unref(nodeList, &exc);

            return mcsFAILURE;
        }

        // Get the child node type
        nodeType = gdome_n_nodeType(child, &exc);

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
                // Store buffer into data buffer:
                dataBuf->Reset();
                FAIL(dataBuf->AppendString(cdataValue->str));

                if (cData->AppendLines(dataBuf, cData->GetNbLinesToSkip()) == mcsFAILURE)
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
        else if (nodeType == GDOME_ELEMENT_NODE)
        {
            // If it is an element node, try to get information on attributes
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
            bool isCsv = false;

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
                for (mcsUINT32 j = 0; j < nbAttrs; j++)
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
                        bool isFieldName = false;
                        bool isFieldUcd = false;
                        bool isCsvHeadlines = false;

                        if (isField)
                        {
                            isFieldName = (strcmp(attrName->str, "name") == 0);

                            if (!isFieldName)
                            {
                                isFieldUcd = (strcmp(attrName->str, "ucd") == 0);
                            }

                        }
                        else if (isCsv)
                        {
                            isCsvHeadlines = (strcmp(attrName->str, "headlines") == 0);
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
                if (ParseXmlSubTree(child, cData, dataBuf) == mcsFAILURE)
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
