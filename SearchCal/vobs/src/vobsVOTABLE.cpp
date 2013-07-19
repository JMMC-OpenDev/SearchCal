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
 * @param fileName the path to the file in which the VOTable should be saved
 * @param header header of the VO Table
 * @param softwareVersion software version
 * @param request user request
 * @param xmlRequest user request as XML
 * @param buffer the output buffer
 *
 * @return always mcsSUCCESS. 
 */
mcsCOMPL_STAT vobsVOTABLE::GetVotable(const vobsSTAR_LIST& starList,
                                      const char* fileName,
                                      const char* header,
                                      const char* softwareVersion,
                                      const char* request,
                                      const char* xmlRequest,
                                      miscoDYN_BUF* votBuffer)
{
    // Get the first start of the list
    vobsSTAR* star = starList.GetNextStar(mcsTRUE);
    FAIL_NULL_DO(star, errAdd(vobsERR_EMPTY_STAR_LIST));

    // If not in regression test mode (-noFileLine)
    const char* serverVersion = isTrue(logGetPrintFileLine()) ? softwareVersion : "SearchCal Regression Test Mode";

    const unsigned int nbStars = starList.Size();
    const int nbProperties = star->NbProperties();

    // Filtered star property indexes:
    int filteredPropertyIndexes[nbProperties];

    // Property infos:
    mcsSTRING256 propertyInfos[nbProperties];

    // flags to serialize confidence / origin indexes as PARAM (single value) or FIELD (multiple values) ?
    bool propertyConfidenceField[nbProperties];
    bool propertyOriginField    [nbProperties];

    // single values used to serialize PARAMs:
    vobsCONFIDENCE_INDEX propertyConfidenceValue[nbProperties];
    vobsORIGIN_INDEX     propertyOriginValue    [nbProperties];

    vobsSTAR_PROPERTY* starProperty = NULL;
    int propIdx, i, filterPropIdx;

    vobsCONFIDENCE_INDEX confidence;
    vobsORIGIN_INDEX     origin;

    /* use block to reduce variable scope */
    {
        logInfo("Star Property statistics:");

        miscoDYN_BUF statBuf;

        // Prepare buffer:
        FAIL(statBuf.Reset());
        FAIL(statBuf.Reserve(4 * 1024));

        const char* propId;
        mcsSTRING64 tmp;

        mcsUINT32 nbSet = 0;
        mcsUINT32 nbOrigin = 0;
        mcsUINT32 nbOrigins[vobsNB_ORIGIN_INDEX];
        mcsUINT32 nbConfidence = 0;
        mcsUINT32 nbConfidences[vobsNB_CONFIDENCE_INDEX];

        bool useProperty;

        /* stats on each star property */
        for (propIdx = 0, filterPropIdx = 0; propIdx < nbProperties; propIdx++)
        {
            /* reset stats */
            FAIL(statBuf.Reset());
            nbSet = 0;
            nbOrigin = 0;
            origin = vobsORIG_NONE;
            nbConfidence = 0;
            confidence = vobsCONFIDENCE_NO;

            for (i = 0; i < vobsNB_ORIGIN_INDEX; i++)
            {
                nbOrigins[i] = 0;
            }

            for (i = 0; i < vobsNB_CONFIDENCE_INDEX; i++)
            {
                nbConfidences[i] = 0;
            }

            // traverse all stars again:
            for (star = starList.GetNextStar(mcsTRUE); isNotNull(star); star = starList.GetNextStar())
            {
                starProperty = star->GetProperty(propIdx);

                // Take value into account if set
                if (isTrue(starProperty->IsSet()))
                {
                    nbSet++;

                    nbOrigins[starProperty->GetOriginIndex()]++;
                    nbConfidences[starProperty->GetConfidenceIndex()]++;
                }
            }

            sprintf(tmp, "values (%d)", nbSet);

            statBuf.AppendString(tmp);

            if (nbSet != 0)
            {
                statBuf.AppendString(" origins (");

                for (i = 0; i < vobsNB_ORIGIN_INDEX; i++)
                {
                    if (nbOrigins[i] != 0)
                    {
                        origin = (vobsORIGIN_INDEX) i;
                        nbOrigin++;
                        sprintf(tmp, "%d [%s] ", nbOrigins[i], vobsGetOriginIndex(origin));
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
                        sprintf(tmp, "%d [%s] ", nbConfidences[i], vobsGetConfidenceIndex(confidence));
                        statBuf.AppendString(tmp);
                    }
                }
                statBuf.AppendString(")");
            }

            // Dump stats:
            logInfo("Property [%s]: %s", starProperty->GetName(), statBuf.GetBuffer());

            strncpy(propertyInfos[propIdx], statBuf.GetBuffer(), sizeof (propertyInfos[propIdx]) - 1);

            // Filter property ?
            useProperty = true;

            propId = vobsSTAR::GetPropertyMeta(propIdx)->GetId();

            if ((nbSet == 0)
                || (strcmp(propId, vobsSTAR_ID_TARGET) == 0)
                || (strcmp(propId, vobsSTAR_JD_DATE) == 0))
            {
                useProperty = false;
            }

            if (useProperty)
            {
                filteredPropertyIndexes[filterPropIdx++] = propIdx;

                propertyOriginField    [propIdx] = (nbOrigin     >  1);
                propertyOriginValue    [propIdx] = (nbOrigin     == 1) ? origin     : vobsORIG_NONE;

                propertyConfidenceField[propIdx] = (nbConfidence >  1);
                propertyConfidenceValue[propIdx] = (nbConfidence == 1) ? confidence : vobsCONFIDENCE_NO;
            }
        } // loop on star properties
    }

    const int nbFilteredProps = filterPropIdx;

    /* buffer capacity = fixed (8K) 
     * + column definitions (3 x nbProperties x 280 [248.229980] ) 
     * + data ( nbStars x 2400 [2391.5] ) */
    const int capacity = 8192 + 3 * nbFilteredProps * 280 + nbStars * 2450;

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
    if (isTrue(logGetPrintDate()))
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

    // Add context specific informations
    votBuffer->AppendLine(" <COOSYS ID=\"J2000\" equinox=\"J2000.\" epoch=\"J2000.\" system=\"eq_FK5\"/>\n");

    // Add software informations
    votBuffer->AppendLine(" <RESOURCE name=\"");
    votBuffer->AppendString(serverVersion);
    votBuffer->AppendString("\">");

    votBuffer->AppendLine("  <TABLE");
    if (isNotNull(fileName))
    {
        votBuffer->AppendString(" name=\"");
        votBuffer->AppendString(fileName);
        votBuffer->AppendString("\"");
    }

    // number of rows (useful for partial parser)
    votBuffer->AppendString(" nrows=\"");
    sprintf(tmp, "%d", nbStars);
    votBuffer->AppendString(tmp);
    votBuffer->AppendString("\">");

    // Add PARAMs

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
        starProperty = star->GetProperty(filterPropIdx);

        // Add standard field header
        votBuffer->AppendLine("   <FIELD");

        // Add field name (note: name conflict with GROUP !)
        votBuffer->AppendString(" name=\"");
        propertyName = starProperty->GetName();
        votBuffer->AppendString(propertyName);
        votBuffer->AppendString("\"");

        // Add field ID
        votBuffer->AppendString(" ID=\"");
        sprintf(tmp, "col%d", i);
        votBuffer->AppendString(tmp);
        votBuffer->AppendString("\"");

        // Add field ucd
        votBuffer->AppendString(" ucd=\"");
        votBuffer->AppendString(starProperty->GetId());
        votBuffer->AppendString("\"");

        // Add field ref
        if ((strcmp(propertyName, "RAJ2000") == 0) || (strcmp(propertyName, "DEJ2000") == 0))
        {
            votBuffer->AppendString(" ref=\"J2000\"");
        }

        // Add field datatype
        votBuffer->AppendString(" datatype=\"");
        switch (starProperty->GetType())
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
        unit = starProperty->GetUnit();
        if (isNotNull(unit))
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

        description = starProperty->GetDescription();
        if (isNotNull(description))
        {
            votBuffer->AppendString(description);
        }
        votBuffer->AppendString("</DESCRIPTION>");
        
        votBuffer->AppendLine("    <!-- ");
        votBuffer->AppendString(propertyInfos[filterPropIdx]);
        votBuffer->AppendString(" -->");

        // Add field link if present
        link = starProperty->GetLink();
        if (isNotNull(link))
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

        // TODO: put multiple used values (<values><option name="" value="" /></values>)

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
        starProperty = star->GetProperty(filterPropIdx);

        // Add standard group header
        votBuffer->AppendLine("   <GROUP");

        // Add group name (note: name conflict with FIELD !)
        votBuffer->AppendString(" name=\"");
        propertyName = starProperty->GetName();
        votBuffer->AppendString(propertyName);
        votBuffer->AppendString("\"");

        // Add group ucd
        votBuffer->AppendString(" ucd=\"");
        votBuffer->AppendString(starProperty->GetId());
        votBuffer->AppendString("\"");

        // Close GROUP opened markup
        votBuffer->AppendString(">");

        // Add field description
        votBuffer->AppendLine("    <DESCRIPTION>");
        votBuffer->AppendString(propertyName);
        votBuffer->AppendString(" with its origin and confidence indexes</DESCRIPTION>");

        // Bind main field ref
        sprintf(tmp, "col%d", i);
        votBuffer->AppendLine("    <FIELDref ref=\"");
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

        // Add standard group footer
        votBuffer->AppendLine("   </GROUP>");

        i += 3;
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
    // Note: 8K is large enough to contain one line
    // Warning: no buffer overflow checks !
    char line[8192];
    char* linePtr;
    const char* value;

    long lineSizes = 0;

    while (isNotNull(star))
    {
        // Add standard row header (no indentation)
        strcpy(line, "<TR>");

        // reset line pointer:
        linePtr = line;

        for (propIdx = 0; propIdx < nbFilteredProps; propIdx++)
        {
            filterPropIdx = filteredPropertyIndexes[propIdx];
            starProperty = star->GetProperty(filterPropIdx);

            // Add value if it is not vobsSTAR_PROP_NOT_SET
            if (isTrue(starProperty->IsSet()))
            {
                value = starProperty->GetValue();
                vobsStrcatFast(linePtr, "<TD>");
                vobsStrcatFast(linePtr, value);
                vobsStrcatFast(linePtr, "</TD>");

                // Add ORIGIN value if needed
                if (propertyOriginField[filterPropIdx])
                {
                    origin = starProperty->GetOriginIndex();
                    vobsStrcatFast(linePtr, "<TD>");
                    vobsStrcatFast(linePtr, vobsGetOriginIndexAsInt(origin));
                    vobsStrcatFast(linePtr, "</TD>");
                }

                // Add CONFIDENCE value if needed AND computed value or (converted value ie LOW/MEDIUM)
                if (propertyConfidenceField[filterPropIdx])
                {
                    confidence = starProperty->GetConfidenceIndex();
                    vobsStrcatFast(linePtr, "<TD>");
                    vobsStrcatFast(linePtr, vobsGetConfidenceIndexAsInt(confidence));
                    vobsStrcatFast(linePtr, "</TD>");
                }
            }
            else
            {
                /* handle / fix null value handling 
                 * as VOTABLE 1.1 does not support nulls for integer (-INF) / double values (NaN)
                 * note: stilts complains and replaces empty cells by (-INF) and (NaN) */

                /* TODO: switch to VOTABLE 1.3 that supports null values */

                switch (starProperty->GetType())
                {
                    case vobsFLOAT_PROPERTY:
                        vobsStrcatFast(linePtr, "<TD>NaN</TD>");
                        break;
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
            lineSizes += strlen(line);
        }

        // Jump on the next star of the list
        star = starList.GetNextStar();
    }

    // Add SCALIB data footer
    votBuffer->AppendLine("    </TABLEDATA>");
    votBuffer->AppendLine("   </DATA>");
    votBuffer->AppendLine("  </TABLE>");
    votBuffer->AppendLine(" </RESOURCE>");

    // Add VOTable standard footer
    votBuffer->AppendLine("</VOTABLE>");

    if (doLog(logTEST))
    {
        mcsUINT32 storedBytes;
        votBuffer->GetNbStoredBytes(&storedBytes);

        if (vobsVOTABLE_LINE_SIZE_STATS)
        {
            logTest("GetVotable: line size   = %ld / %.1lf bytes", lineSizes, (1.0 * lineSizes) / (double) nbStars);
        }
        logTest("GetVotable: size=%d bytes / capacity=%d bytes", storedBytes, capacity);
    }

    return mcsSUCCESS;
}

/**
 * Save the star list serialization (in VOTable v1.1 format) in file.
 *
 * @param starList the the list of stars to serialize
 * @param fileName the path to the file in which the VOTable should be saved
 * @param header header of the VO Table
 * @param softwareVersion software version
 * @param request user request
 * @param xmlRequest user request as XML
 *
 * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise. 
 */
mcsCOMPL_STAT vobsVOTABLE::Save(vobsSTAR_LIST& starList,
                                const char* fileName,
                                const char* header,
                                const char* softwareVersion,
                                const char* request,
                                const char* xmlRequest)
{
    miscoDYN_BUF votBuffer;

    // Get the star list in the VOTable format
    FAIL(GetVotable(starList, fileName, header, softwareVersion, request, xmlRequest, &votBuffer));

    // Try to save the generated VOTable in the specified file as ASCII
    return (votBuffer.SaveInASCIIFile(fileName));
}


/*___oOo___*/
