/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Definition of vobsVOTABLE class.
 */


/*
 * System Headers
 */
#include <iostream>
#include <stdio.h>
#include <string.h>
using namespace std;

/*
 * MCS Headers
 */
#include "mcs.h"
#include "log.h"
#include "err.h"
#include "miscoDYN_BUF.h"

/*
 * Local Headers
 */
#include "vobsVOTABLE.h"
#include "vobsSTAR.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"

/** flag to estimate the line buffer size */
#define vobsVOTABLE_LINE_SIZE_STATS false

/** char buffer capacity to store a complete TR line (large enough to avoid overflow and segfault) */
#define vobsVOTABLE_LINE_BUFFER_CAPACITY 32768

/*
 * Public methods
 */

/**
 * Fast strcat alternative (destination and source MUST not overlap)
 * No buffer overflow checks
 * @param dest destination pointer (updated when this function returns to indicate the position of the last character)
 * @param src source buffer
 */
void vobsStrcatFast(char*& dest, const char* src)
{
    while (*dest) dest++;
    while ((*dest++ = *src++));
    --dest;
}

/**
 * Class constructor
 */
vobsVOTABLE::vobsVOTABLE()
{
}

/**
 * Class destructor
 */
vobsVOTABLE::~vobsVOTABLE()
{
}


/*
 * Public methods
 */

/**
 * Serialize a star list in a VOTable v1.1 XML file.
 *
 * @param starList the the list of stars to serialize
 * @param command server command (SearchCal or GetStar)
 * @param fileName the path to the file in which the VOTable should be saved
 * @param header header of the VO Table
 * @param softwareVersion software version
 * @param request user request
 * @param xmlRequest user request as XML
 * @param log optional server log for that request
 * @param trimColumns true to trim empty columns
 * @param buffer the output buffer
 *
 * @return always mcsSUCCESS.
 */
mcsCOMPL_STAT vobsVOTABLE::GetVotable(const vobsSTAR_LIST& starList,
                                      const char* command,
                                      const char* fileName,
                                      const char* header,
                                      const char* softwareVersion,
                                      const char* request,
                                      const char* xmlRequest,
                                      const char *log,
                                      mcsLOGICAL trimColumns,
                                      miscoDYN_BUF* votBuffer)
{
    // Get the first start of the list
    vobsSTAR* star = starList.GetNextStar(mcsTRUE);
    FAIL_NULL_DO(star, errAdd(vobsERR_EMPTY_STAR_LIST));

    // If not in regression test mode (-noFileLine)
    const char* serverVersion = IS_TRUE(logGetPrintFileLine()) ? softwareVersion : "SearchCal Regression Test Mode";

    const mcsUINT32 nbStars = starList.Size();
    const mcsINT32 nbProperties = star->NbProperties();

    const bool doTrimProperties = IS_TRUE(trimColumns);

    // Filtered star property indexes:
    mcsINT32 filteredPropertyIndexes[nbProperties];

    // flag to serialize error as FIELD
    bool propertyErrorField[nbProperties];

    // Property infos:
    mcsSTRING256 propertyInfos[nbProperties];

    // flags to serialize confidence / origin indexes as PARAM (single value) or FIELD (multiple values) ?
    bool propertyConfidenceField[nbProperties];
    bool propertyOriginField    [nbProperties];

    // single values used to serialize PARAMs:
    vobsCONFIDENCE_INDEX propertyConfidenceValue[nbProperties];
    vobsORIGIN_INDEX     propertyOriginValue    [nbProperties];

    vobsSTAR_PROPERTY* property = NULL;
    mcsINT32 propIdx, i, filterPropIdx;

    vobsCONFIDENCE_INDEX confidence;
    vobsORIGIN_INDEX     origin;

    /* use block to reduce variable scope */
    {
        logInfo("Star Property statistics:");

        miscoDYN_BUF statBuf;

        // Prepare buffer:
        FAIL(statBuf.Reset());
        FAIL(statBuf.Reserve(4 * 1024));

        mcsSTRING64 tmp;

        mcsUINT32 nbSet = 0;
        mcsUINT32 nbError = 0;
        mcsUINT32 nbOrigin = 0;
        mcsUINT32 nbOrigins[vobsNB_ORIGIN_INDEX];
        mcsUINT32 nbConfidence = 0;
        mcsUINT32 nbConfidences[vobsNB_CONFIDENCE_INDEX];

        bool      propErrorMeta;

        /* stats on each star property */
        for (propIdx = 0, filterPropIdx = 0; propIdx < nbProperties; propIdx++)
        {
            /* reset stats */
            FAIL(statBuf.Reset());
            nbSet    = 0;
            nbError  = 0;
            nbOrigin = 0;
            origin   = vobsORIG_NONE;
            nbConfidence = 0;
            confidence = vobsCONFIDENCE_NO;
            propErrorMeta = false;

            for (i = 0; i < vobsNB_ORIGIN_INDEX; i++)
            {
                nbOrigins[i] = 0;
            }

            for (i = 0; i < vobsNB_CONFIDENCE_INDEX; i++)
            {
                nbConfidences[i] = 0;
            }

            // traverse all stars again:
            for (star = starList.GetNextStar(mcsTRUE); IS_NOT_NULL(star); star = starList.GetNextStar())
            {
                property = star->GetProperty(propIdx);

                if (IS_NOT_NULL(property->GetErrorMeta()))
                {
                    propErrorMeta = true;
                }

                // Take value into account if set
                if (isPropSet(property))
                {
                    nbSet++;

                    // Take error into account if set
                    if (IS_TRUE(property->IsErrorSet()))
                    {
                        nbError++;
                    }

                    nbOrigins[property->GetOriginIndex()]++;
                    nbConfidences[property->GetConfidenceIndex()]++;
                }
            }

            sprintf(tmp, "values (%u)", nbSet);
            statBuf.AppendString(tmp);

            if (nbError != 0)
            {
                sprintf(tmp, " errors (%u)", nbError);
                statBuf.AppendString(tmp);
            }

            if (nbSet != 0)
            {
                statBuf.AppendString(" origins (");

                for (i = 0; i < vobsNB_ORIGIN_INDEX; i++)
                {
                    if (nbOrigins[i] != 0)
                    {
                        origin = (vobsORIGIN_INDEX) i;
                        nbOrigin++;
                        sprintf(tmp, "%u [%s] ", nbOrigins[i], vobsGetOriginIndex(origin));
                        statBuf.AppendString(tmp);
                    }
                }
                statBuf.AppendString(") confidences (");

                for (i = 0; i < vobsNB_CONFIDENCE_INDEX; i++)
                {
                    if (nbConfidences[i] != 0)
                    {
                        confidence = (vobsCONFIDENCE_INDEX) i;
                        nbConfidence++;
                        sprintf(tmp, "%u [%s] ", nbConfidences[i], vobsGetConfidenceIndex(confidence));
                        statBuf.AppendString(tmp);
                    }
                }
                statBuf.AppendString(")");
            }

            // Dump stats:
            logInfo("Property [%s]: %s", property->GetName(), statBuf.GetBuffer());

            strncpy(propertyInfos[propIdx], statBuf.GetBuffer(), sizeof (propertyInfos[propIdx]) - 1);

            // Filter property (column) if no value set and trim column enabled
            if ((nbSet != 0) || !doTrimProperties)
            {
                filteredPropertyIndexes[filterPropIdx++] = propIdx;
                propertyErrorField     [propIdx]         = (nbError != 0) || (!doTrimProperties && propErrorMeta);

                propertyOriginField    [propIdx] = (nbOrigin     >  1) || !doTrimProperties;
                propertyOriginValue    [propIdx] = (nbOrigin     == 1) ? origin     : vobsORIG_NONE;

                propertyConfidenceField[propIdx] = (nbConfidence >  1) || !doTrimProperties;
                propertyConfidenceValue[propIdx] = (nbConfidence == 1) ? confidence : vobsCONFIDENCE_NO;
            }
        } // loop on star properties
    }

    const mcsINT32 nbFilteredProps = filterPropIdx;

    // Encode optional log:
    std::string encodedStr;
    if (IS_NOT_NULL(log))
    {
        // Encode xml character restrictions:
        // encode [& < >] characters by [&amp; &lt; &gt;]
        encodedStr.reserve((strlen(log) * 101) / 100);
        encodedStr.append(log);
        ReplaceStringInPlace(encodedStr, "&", "&amp;");
        ReplaceStringInPlace(encodedStr, "<", "&lt;");
        ReplaceStringInPlace(encodedStr, ">", "&gt;");
    }

    /* buffer capacity = fixed (8K)
     * + column definitions (3 x nbProperties x 300 [275.3] )
     * + data ( nbStars x 7200 [mean: 6855] ) */
    const miscDynSIZE capacity = 8192 + 3 * nbFilteredProps * 300 + nbStars * 7200L + encodedStr.length();

    if (capacity > 10 * 1024 * 1024)
    {
        logTest("GetVotable: allocating votable buffer with capacity=%lu bytes", capacity);
    }

    mcsSTRING16 tmp;

    votBuffer->Reserve(capacity);

    // Add VOTable standard header
    votBuffer->AppendLine("<?xml version=\"1.0\"?>\n");

    votBuffer->AppendLine("<VOTABLE version=\"1.1\"");
    votBuffer->AppendLine("         xmlns=\"http://www.ivoa.net/xml/VOTable/v1.1\"");
    votBuffer->AppendLine("         xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"");
    votBuffer->AppendLine("         xsi:schemaLocation=\"http://www.ivoa.net/xml/VOTable/v1.1 http://www.ivoa.net/xml/VOTable/VOTable-1.1.xsd\">\n");

    // Add header informations
    votBuffer->AppendLine(" <DESCRIPTION>\n  ");
    votBuffer->AppendString(header);

    // Add request informations
    votBuffer->AppendLine("  Request parameters: ");
    votBuffer->AppendString(request);

    // Add current date
    votBuffer->AppendLine("  Generated on (UTC): ");
    // If not in regression test mode (-noDate)
    if (IS_TRUE(logGetPrintDate()))
    {
        mcsSTRING32 utcTime;
        FAIL(miscGetUtcTimeStr(0, utcTime));
        votBuffer->AppendString(utcTime);
    }
    else
    {
        votBuffer->AppendString("SearchCal Regression Test Mode");
    }

    votBuffer->AppendLine(" </DESCRIPTION>\n");

    if (IS_NOT_NULL(log))
    {
        votBuffer->AppendLine(" <INFO>\n");
        votBuffer->AppendString(encodedStr.c_str());
        votBuffer->AppendLine(" </INFO>\n");
    }
    // free string anyway
    encodedStr.clear();

    // Add context specific informations
    votBuffer->AppendLine(" <COOSYS ID=\"J2000\" equinox=\"J2000.\" epoch=\"J2000.\" system=\"eq_FK5\"/>\n");

    // Add software informations
    votBuffer->AppendLine(" <RESOURCE name=\"");
    votBuffer->AppendString(serverVersion);
    votBuffer->AppendString("\">");

    votBuffer->AppendLine("  <TABLE");
    if (IS_NOT_NULL(fileName))
    {
        votBuffer->AppendString(" name=\"");
        votBuffer->AppendString(fileName);
        votBuffer->AppendString("\"");
    }

    // number of rows (useful for partial parser)
    votBuffer->AppendString(" nrows=\"");
    sprintf(tmp, "%u", nbStars);
    votBuffer->AppendString(tmp);
    votBuffer->AppendString("\">");

    // Add PARAMs

    if (IS_NOT_NULL(command))
    {
        // Write the server command as parameter 'ServerCommand':
        votBuffer->AppendLine("<PARAM name=\"ServerCommand\" datatype=\"char\" arraysize=\"*\" value=\"");
        votBuffer->AppendString(command);
        votBuffer->AppendString("\"/>");
    }

    // If not in regression test mode (-noDate)
    if (IS_TRUE(logGetPrintDate()))
    {
        mcsSTRING32 utcTime;
        FAIL(miscGetUtcTimeStr(0, utcTime));

        // Write the server date as parameter 'ServerDate':
        votBuffer->AppendLine("<PARAM name=\"ServerDate\" datatype=\"char\" arraysize=\"*\" value=\"");
        votBuffer->AppendString(utcTime);
        votBuffer->AppendString("\"/>");
    }

    // Write the server version as parameter 'SearchCalServerVersion':
    votBuffer->AppendLine("<PARAM name=\"SearchCalServerVersion\" datatype=\"char\" arraysize=\"*\" value=\"");
    votBuffer->AppendString(serverVersion);
    votBuffer->AppendString("\"/>");

    // xml request contains <PARAM> tags (from cmdCOMMAND)
    // note: xmlRequest starts by '\n':
    votBuffer->AppendString(xmlRequest);

    // Write the confidence indexes as parameter 'ConfidenceIndexes':
    votBuffer->AppendLine("<PARAM name=\"ConfidenceIndexes\" datatype=\"int\" value=\"0\">");
    votBuffer->AppendLine(" <VALUES>");

    for (i = 0; i < vobsNB_CONFIDENCE_INDEX; i++)
    {
        votBuffer->AppendLine  ("  <OPTION name=\"");
        votBuffer->AppendString(vobsCONFIDENCE_INT[i]);
        votBuffer->AppendString("\" value=\"");
        votBuffer->AppendString(vobsCONFIDENCE_STR[i]);
        votBuffer->AppendString("\"/>");
    }
    votBuffer->AppendLine(" </VALUES>");
    votBuffer->AppendLine("</PARAM>");

    // Write the origin indexes as parameter 'OriginIndexes':
    votBuffer->AppendLine("<PARAM name=\"OriginIndexes\" datatype=\"int\" value=\"0\">");
    votBuffer->AppendLine(" <VALUES>");

    for (i = 0; i < vobsNB_ORIGIN_INDEX; i++)
    {
        votBuffer->AppendLine  ("  <OPTION name=\"");
        votBuffer->AppendString(vobsORIGIN_INT[i]);
        votBuffer->AppendString("\" value=\"");
        votBuffer->AppendString(vobsORIGIN_STR[i]);
        votBuffer->AppendString("\"/>");
    }
    votBuffer->AppendLine(" </VALUES>");
    votBuffer->AppendLine("</PARAM>");

    // Serialize each of its properties with origin and confidence index
    // as VOTable column description (i.e FIELDS)
    const vobsSTAR_PROPERTY_META* propMeta;
    const char* propertyName;
    const char* unit;
    const char* description;
    const char* link;
    bool        useField;

    // traverse all stars again:
    star = starList.GetNextStar(mcsTRUE);

    for (i = 0, propIdx = 0; propIdx < nbFilteredProps; propIdx++)
    {
        filterPropIdx = filteredPropertyIndexes[propIdx];
        property  = star->GetProperty(filterPropIdx);
        propMeta      = property->GetMeta();

        if (IS_NOT_NULL(propMeta))
        {
            // Add standard field header
            votBuffer->AppendLine("   <FIELD");

            // Add field name (note: name conflict with GROUP !)
            votBuffer->AppendString(" name=\"");
            propertyName = propMeta->GetName();
            votBuffer->AppendString(propertyName);
            votBuffer->AppendString("\"");

            // Add field ID
            votBuffer->AppendString(" ID=\"");
            sprintf(tmp, "col%d", i);
            votBuffer->AppendString(tmp);
            votBuffer->AppendString("\"");

            // Add field ucd
            votBuffer->AppendString(" ucd=\"");
            votBuffer->AppendString(propMeta->GetId());
            votBuffer->AppendString("\"");

            // Add field ref
            if ((strcmp(propertyName, "RAJ2000") == 0) || (strcmp(propertyName, "DEJ2000") == 0))
            {
                votBuffer->AppendString(" ref=\"J2000\"");
            }

            // Add field datatype
            votBuffer->AppendString(" datatype=\"");
            switch (propMeta->GetType())
            {
                case vobsSTRING_PROPERTY:
                    votBuffer->AppendString("char\" arraysize=\"*");
                    break;

                case vobsFLOAT_PROPERTY:
                    votBuffer->AppendString("double"); // double instead of float
                    break;

                case vobsINT_PROPERTY:
                    votBuffer->AppendString("int");
                    break;

                case vobsBOOL_PROPERTY:
                    votBuffer->AppendString("boolean");
                    break;

                default:
                    // Assertion - unknow type
                    break;
            }
            votBuffer->AppendString("\"");

            // Add field unit if not null
            unit = propMeta->GetUnit();
            if (IS_NOT_NULL(unit))
            {
                // Add field unit
                votBuffer->AppendString(" unit=\"");
                votBuffer->AppendString(unit);
                votBuffer->AppendString("\"");
            }

            // Close FIELD opened markup
            votBuffer->AppendString(">");

            // Add field description
            votBuffer->AppendLine("    <DESCRIPTION>");

            description = propMeta->GetDescription();
            if (IS_NOT_NULL(description))
            {
                votBuffer->AppendString(description);
            }
            votBuffer->AppendString("</DESCRIPTION>");

            votBuffer->AppendLine("    <!-- ");
            votBuffer->AppendString(propertyInfos[filterPropIdx]);
            votBuffer->AppendString(" -->");

            // Add field link if present
            link = propMeta->GetLink();
            if (IS_NOT_NULL(link))
            {
                votBuffer->AppendLine("    <LINK href=\"");
                votBuffer->AppendString(link);
                votBuffer->AppendString("\"/>");
            }

            // Add standard field footer
            votBuffer->AppendLine("   </FIELD>");


            // Add ORIGIN field or param:
            useField = propertyOriginField[filterPropIdx];

            votBuffer->AppendLine((useField) ? "   <FIELD" : "   <PARAM");

            // Add field name
            votBuffer->AppendString(" name=\"");
            votBuffer->AppendString(propertyName);
            votBuffer->AppendString(".origin\"");

            // Add field ID
            votBuffer->AppendString(" ID=\"");
            sprintf(tmp, "col%d", i + 1);
            votBuffer->AppendString(tmp);
            votBuffer->AppendString("\"");

            // Add field ucd "REFER_CODE" for the ORIGIN field:
            votBuffer->AppendString(" ucd=\"REFER_CODE\"");

            // Add field datatype
            votBuffer->AppendString(" datatype=\"int\"");

            if (useField)
            {
                // hide column
                votBuffer->AppendString(" type=\"hidden\"");
            }
            else
            {
                votBuffer->AppendString(" value=\"");
                votBuffer->AppendString(vobsGetOriginIndexAsInt(propertyOriginValue[filterPropIdx]));
                votBuffer->AppendString("\"");
            }

            // Close FIELD opened markup
            votBuffer->AppendString(">");

            // Add field description
            votBuffer->AppendLine("    <DESCRIPTION>Origin index of property ");
            votBuffer->AppendString(propertyName);

            if (!useField)
            {
                votBuffer->AppendString(" (");
                votBuffer->AppendString(vobsGetOriginIndex(propertyOriginValue[filterPropIdx]));
                votBuffer->AppendString(")");
            }
            votBuffer->AppendString("</DESCRIPTION>");

            // Add standard field or param footer
            votBuffer->AppendLine((useField) ? "   </FIELD>" : "   </PARAM>");


            // Add CONFIDENCE field
            useField = propertyConfidenceField[filterPropIdx];

            votBuffer->AppendLine((useField) ? "   <FIELD" : "   <PARAM");

            // Add field name
            votBuffer->AppendString(" name=\"");
            votBuffer->AppendString(propertyName);
            votBuffer->AppendString(".confidence\"");

            // Add field ID
            votBuffer->AppendString(" ID=\"");
            sprintf(tmp, "col%d", i + 2);
            votBuffer->AppendString(tmp);
            votBuffer->AppendString("\"");

            // Add field ucd "CODE_QUALITY" for the CONFIDENCE field:
            votBuffer->AppendString(" ucd=\"CODE_QUALITY\"");

            // Add field datatype
            votBuffer->AppendString(" datatype=\"int\"");

            if (useField)
            {
                // hide column
                votBuffer->AppendString(" type=\"hidden\"");
            }
            else
            {
                votBuffer->AppendString(" value=\"");
                votBuffer->AppendString(vobsGetConfidenceIndexAsInt(propertyConfidenceValue[filterPropIdx]));
                votBuffer->AppendString("\"");
            }

            // Close FIELD opened markup
            votBuffer->AppendString(">");

            // Add field description
            votBuffer->AppendLine("    <DESCRIPTION>Confidence index of property ");
            votBuffer->AppendString(propertyName);

            if (!useField)
            {
                votBuffer->AppendString(" (");
                votBuffer->AppendString(vobsGetConfidenceIndex(propertyConfidenceValue[filterPropIdx]));
                votBuffer->AppendString(")");
            }
            votBuffer->AppendString("</DESCRIPTION>");

            // Add standard field or param footer
            votBuffer->AppendLine((useField) ? "   </FIELD>" : "   </PARAM>");

            i += 3;

            // Add optional Error FIELD
            if (propertyErrorField[filterPropIdx])
            {
                propMeta = property->GetErrorMeta();

                // Add standard field header
                votBuffer->AppendLine("   <FIELD");

                // Add field name
                votBuffer->AppendString(" name=\"");
                propertyName = propMeta->GetName();
                votBuffer->AppendString(propertyName);
                votBuffer->AppendString("\"");

                // Add field ID
                votBuffer->AppendString(" ID=\"");
                sprintf(tmp, "col%d", i);
                votBuffer->AppendString(tmp);
                votBuffer->AppendString("\"");

                // Add field ucd
                votBuffer->AppendString(" ucd=\"");
                votBuffer->AppendString(propMeta->GetId());
                votBuffer->AppendString("\"");

                // Add field datatype (double)
                votBuffer->AppendString(" datatype=\"double\"");

                // Add field unit if not null
                unit = propMeta->GetUnit();
                if (IS_NOT_NULL(unit))
                {
                    // Add field unit
                    votBuffer->AppendString(" unit=\"");
                    votBuffer->AppendString(unit);
                    votBuffer->AppendString("\"");
                }

                // Close FIELD opened markup
                votBuffer->AppendString(">");

                // Add field description
                votBuffer->AppendLine("    <DESCRIPTION>");

                description = propMeta->GetDescription();
                if (IS_NOT_NULL(description))
                {
                    votBuffer->AppendString(description);
                }
                votBuffer->AppendString("</DESCRIPTION>");

                // Add standard field footer
                votBuffer->AppendLine("   </FIELD>");

                i++;
            }
        }
    }

    // Add the beginning of the deletedFlag field
    // TODO: remove the deleteFlag column from server side (ASAP)
    votBuffer->AppendLine("   <FIELD name=\"deletedFlag\" ID=\"");
    // Add field ID
    sprintf(tmp, "col%d", i);
    votBuffer->AppendString(tmp);
    // Add the end of the deletedFlag field
    votBuffer->AppendString("\" ucd=\"DELETED_FLAG\" datatype=\"boolean\" type=\"hidden\">");
    // Add deleteFlag description
    votBuffer->AppendLine("    <DESCRIPTION>Used by SearchCal GUI to flag deleted stars</DESCRIPTION>");
    // Add standard field footer
    votBuffer->AppendLine("   </FIELD>");
    // Add the beginning of the deletedFlag origin field
    votBuffer->AppendLine("   <PARAM name=\"deletedFlag.origin\" ID=\"");
    // Add field ID
    sprintf(tmp, "col%d", i + 1);
    votBuffer->AppendString(tmp);
    // Add the end of the deletedFlag field
    votBuffer->AppendString("\" ucd=\"DELETED_FLAG.origin\" datatype=\"int\" value=\"");
    votBuffer->AppendString(vobsGetOriginIndexAsInt(vobsORIG_NONE));
    votBuffer->AppendString("\">");
    // Add deleteFlag description
    votBuffer->AppendLine("    <DESCRIPTION>Origin of property deletedFlag (NO)</DESCRIPTION>");
    // Add standard field footer
    votBuffer->AppendLine("   </PARAM>");
    // Add the beginning of the deletedFlag confidence indexfield
    votBuffer->AppendLine("   <PARAM name=\"deletedFlag.confidence\" ID=\"");
    // Add field ID
    sprintf(tmp, "col%d", i + 2);
    votBuffer->AppendString(tmp);
    // Add the end of the deletedFlag field
    votBuffer->AppendString("\" ucd=\"DELETED_FLAG.confidence\" datatype=\"int\" value=\"");
    votBuffer->AppendString(vobsGetConfidenceIndexAsInt(vobsCONFIDENCE_NO));
    votBuffer->AppendString("\">");
    // Add deleteFlag description
    votBuffer->AppendLine("    <DESCRIPTION>Confidence index of property deletedFlag (NO)</DESCRIPTION>");
    // Add standard field footer
    votBuffer->AppendLine("   </PARAM>");

    // Serialize each of its properties as group description
    for (i = 0, propIdx = 0; propIdx < nbFilteredProps; propIdx++)
    {
        filterPropIdx = filteredPropertyIndexes[propIdx];
        property = star->GetProperty(filterPropIdx);

        if (IS_NOT_NULL(property))
        {
            // Add standard group header
            votBuffer->AppendLine("   <GROUP");

            // Add group name (note: name conflict with FIELD !)
            votBuffer->AppendString(" name=\"");
            propertyName = property->GetName();
            votBuffer->AppendString(propertyName);
            votBuffer->AppendString("\"");

            // Add group ucd
            votBuffer->AppendString(" ucd=\"");
            votBuffer->AppendString(property->GetId());
            votBuffer->AppendString("\"");

            // Close GROUP opened markup
            votBuffer->AppendString(">");

            // Add field description
            votBuffer->AppendLine("    <DESCRIPTION>");
            votBuffer->AppendString(propertyName);
            votBuffer->AppendString(" with its origin and confidence indexes and its error when available</DESCRIPTION>");

            // Bind main field ref
            votBuffer->AppendLine("    <FIELDref ref=\"");
            sprintf(tmp, "col%d", i);
            votBuffer->AppendString(tmp);
            votBuffer->AppendString("\"/>");

            // Bind ORIGIN field or param ref
            // Add ORIGIN field or param:
            if (propertyOriginField[filterPropIdx])
            {
                votBuffer->AppendLine("    <FIELDref ref=\"");
            }
            else
            {
                votBuffer->AppendLine("    <PARAMref ref=\"");
            }
            sprintf(tmp, "col%d", i + 1);
            votBuffer->AppendString(tmp);
            votBuffer->AppendString("\"/>");

            // Bind CONFIDENCE field or param ref
            if (propertyConfidenceField[filterPropIdx])
            {
                votBuffer->AppendLine("    <FIELDref ref=\"");
            }
            else
            {
                votBuffer->AppendLine("    <PARAMref ref=\"");
            }
            sprintf(tmp, "col%d", i + 2);
            votBuffer->AppendString(tmp);
            votBuffer->AppendString("\"/>");

            i += 3;

            // Add optional Error FIELD
            if (propertyErrorField[filterPropIdx])
            {
                votBuffer->AppendLine("    <FIELDref ref=\"");
                sprintf(tmp, "col%d", i);
                votBuffer->AppendString(tmp);
                votBuffer->AppendString("\"/>");

                i++;
            }

            // Add standard group footer
            votBuffer->AppendLine("   </GROUP>");
        }
    }

    // Add deleteFlag group
    // TODO: remove the deleteFlag column from server side (ASAP)
    votBuffer->AppendLine("   <GROUP name=\"deletedFlag\" ucd=\"DELETED_FLAG\">");
    // Add field description
    votBuffer->AppendLine("    <DESCRIPTION>DELETED_FLAG with its origin and confidence indexes</DESCRIPTION>");
    // Bind main field ref
    sprintf(tmp, "col%d", i);
    votBuffer->AppendLine("    <FIELDref ref=\"");
    votBuffer->AppendString(tmp);
    votBuffer->AppendString("\"/>");
    // Bind ORIGIN field ref
    sprintf(tmp, "col%d", i + 1);
    votBuffer->AppendLine("    <PARAMref ref=\"");
    votBuffer->AppendString(tmp);
    votBuffer->AppendString("\"/>");
    // Bind CONFIDENCE field ref
    sprintf(tmp, "col%d", i + 2);
    votBuffer->AppendLine("    <PARAMref ref=\"");
    votBuffer->AppendString(tmp);
    votBuffer->AppendString("\"/>");
    // Add standard group footer
    votBuffer->AppendLine("   </GROUP>");

    // Serialize each star property value
    votBuffer->AppendLine("   <DATA>");
    votBuffer->AppendLine("    <TABLEDATA>");

    // line buffer to avoid too many calls to dynamic buf:
    // Note: 16K is large enough to contain one line
    // Warning: no buffer overflow checks (segfault is possible) !
    char  line[vobsVOTABLE_LINE_BUFFER_CAPACITY];
    char* linePtr;
    mcsSTRING32 converted;
    const char* strValue;

    mcsUINT32 strLen, maxLineSize = 0;
    mcsUINT64 totalLineSizes = 0;

    for (star = starList.GetNextStar(mcsTRUE); IS_NOT_NULL(star); star = starList.GetNextStar())
    {
        // Add standard row header (no indentation)
        strcpy(line, "<TR>");

        // reset line pointer:
        linePtr = line;

        for (propIdx = 0; propIdx < nbFilteredProps; propIdx++)
        {
            filterPropIdx = filteredPropertyIndexes[propIdx];
            property = star->GetProperty(filterPropIdx);

            if (IS_NOT_NULL(property))
            {
                // Add value if set
                if (isPropSet(property))
                {
                    if (property->GetType() == vobsSTRING_PROPERTY)
                    {
                        strValue = property->GetValue();

                        // use string:

                        // Encode xml character restrictions:
                        // encode [< >] characters by [&lt; &gt;]
                        encodedStr.reserve((strlen(strValue) * 101) / 100);
                        encodedStr.append(strValue);
                        ReplaceStringInPlace(encodedStr, "&", "&amp;");
                        ReplaceStringInPlace(encodedStr, "<", "&lt;");
                        ReplaceStringInPlace(encodedStr, ">", "&gt;");

                        vobsStrcatFast(linePtr, "<TD>");
                        vobsStrcatFast(linePtr, encodedStr.c_str());
                        vobsStrcatFast(linePtr, "</TD>");

                        // free string anyway
                        encodedStr.clear();
                    }
                    else
                    {
                        property->GetFormattedValue(converted);
                        vobsStrcatFast(linePtr, "<TD>");
                        vobsStrcatFast(linePtr, converted);
                        vobsStrcatFast(linePtr, "</TD>");
                    }

                    // Add ORIGIN value if needed
                    if (propertyOriginField[filterPropIdx])
                    {
                        origin = property->GetOriginIndex();
                        vobsStrcatFast(linePtr, "<TD>");
                        vobsStrcatFast(linePtr, vobsGetOriginIndexAsInt(origin));
                        vobsStrcatFast(linePtr, "</TD>");
                    }

                    // Add CONFIDENCE value if needed AND computed value or (converted value ie LOW/MEDIUM)
                    if (propertyConfidenceField[filterPropIdx])
                    {
                        confidence = property->GetConfidenceIndex();
                        vobsStrcatFast(linePtr, "<TD>");
                        vobsStrcatFast(linePtr, vobsGetConfidenceIndexAsInt(confidence));
                        vobsStrcatFast(linePtr, "</TD>");
                    }

                    // Add optional Error value
                    if (propertyErrorField[filterPropIdx])
                    {
                        if (IS_TRUE(property->IsErrorSet()))
                        {
                            /* do not use NaN (useless and annoying in XSLT scripts) */
                            property->GetFormattedError(converted);
                            vobsStrcatFast(linePtr, "<TD>");
                            vobsStrcatFast(linePtr, converted);
                            vobsStrcatFast(linePtr, "</TD>");
                        }
                        else
                        {
                            vobsStrcatFast(linePtr, "<TD/>");      // NaN
                        }
                    }
                }
                else
                {
                    /* handle / fix null value handling
                     * as VOTABLE 1.1 does not support nulls for integer (-INF) / double values (NaN)
                     * note: stilts complains and replaces empty cells by (-INF) and (NaN) */

                    /* TODO: switch to VOTABLE 1.3 that supports null values */

                    switch (property->GetType())
                    {
                        case vobsFLOAT_PROPERTY:
                            /* do not use NaN (useless and annoying in XSLT scripts) */
                            //                        vobsStrcatFast(linePtr, "<TD>NaN</TD>");
                            //                        break;
                        case vobsSTRING_PROPERTY:
                        default:
                            vobsStrcatFast(linePtr, "<TD/>");
                            break;
                        case vobsINT_PROPERTY:
                        case vobsBOOL_PROPERTY:
                            vobsStrcatFast(linePtr, "<TD>0</TD>"); // 0 or false as defaults
                    }

                    if (propertyOriginField[filterPropIdx])
                    {
                        vobsStrcatFast(linePtr, "<TD>0</TD>"); // vobsORIG_NONE
                    }
                    if (propertyConfidenceField[filterPropIdx])
                    {
                        vobsStrcatFast(linePtr, "<TD>0</TD>"); // vobsCONFIDENCE_NO
                    }
                    if (propertyErrorField[filterPropIdx])
                    {
                        vobsStrcatFast(linePtr, "<TD/>");      // NaN
                    }
                }
            }
        }

        // Add default deleteFlag value
        // TODO: remove the deleteFlag column from server side
        vobsStrcatFast(linePtr, "<TD>0</TD>");

        // Add standard row footer
        vobsStrcatFast(linePtr, "</TR>");

        votBuffer->AppendLine(line);

        if (vobsVOTABLE_LINE_SIZE_STATS)
        {
            strLen     = strlen(line);
            totalLineSizes += strLen;

            if (strLen > maxLineSize)
            {
                maxLineSize = strLen;
            }
        }
    }

    // Add SCALIB data footer
    votBuffer->AppendLine("    </TABLEDATA>");
    votBuffer->AppendLine("   </DATA>");
    votBuffer->AppendLine("  </TABLE>");
    votBuffer->AppendLine(" </RESOURCE>");

    // Add VOTable standard footer
    votBuffer->AppendLine("</VOTABLE>\n");

    if (doLog(logTEST))
    {
        miscDynSIZE storedBytes;
        votBuffer->GetNbStoredBytes(&storedBytes);

        if (vobsVOTABLE_LINE_SIZE_STATS)
        {
            logTest("GetVotable: total line size = %ld [max line size = %d / %d]: average line size = %.1lf",
                    totalLineSizes, maxLineSize, vobsVOTABLE_LINE_BUFFER_CAPACITY,
                    (1.0 * totalLineSizes) / (double) nbStars);
        }
        logTest("GetVotable: size=%ld bytes / capacity=%ld bytes", storedBytes, capacity);
    }

    return mcsSUCCESS;
}

/**
 * Save the star list serialization (in VOTable v1.1 format) in file.
 *
 * @param starList the the list of stars to serialize
 * @param command server command (SearchCal or GetStar)
 * @param fileName the path to the file in which the VOTable should be saved
 * @param header header of the VO Table
 * @param softwareVersion software version
 * @param request user request
 * @param xmlRequest user request as XML
 * @param log optional server log for that request
 *
 * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */
mcsCOMPL_STAT vobsVOTABLE::Save(vobsSTAR_LIST& starList,
                                const char* command,
                                const char* fileName,
                                const char* header,
                                const char* softwareVersion,
                                const char* request,
                                const char* xmlRequest,
                                const char *log,
                                mcsLOGICAL trimColumns)
{
    miscoDYN_BUF votBuffer;

    /* TODO: save votable using fixed buffer size (64K) and save file by chunks (buffered write) to avoid allocating very huge buffers ! */

    // Get the star list in the VOTable format
    FAIL(GetVotable(starList, command, fileName, header, softwareVersion, request, xmlRequest, log, trimColumns, &votBuffer));

    logInfo("Saving Votable: %s", fileName);

    // Try to save the generated VOTable in the specified file as ASCII
    return (votBuffer.SaveInASCIIFile(fileName));
}


/*___oOo___*/
