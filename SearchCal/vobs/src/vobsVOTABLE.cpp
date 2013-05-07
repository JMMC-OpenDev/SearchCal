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
    const char* serverVersion = (logGetPrintFileLine() == mcsTRUE) ? softwareVersion : "SearchCal Regression Test Mode";

    const unsigned int nbStars = starList.Size();
    const int nbProperties = star->NbProperties();

    /* buffer capacity = fixed (8K) 
     * + column definitions (3 x nbProperties x 280 [248.229980] ) 
     * + data ( nbStars x 5400 [...] ) */
    const int capacity = 8192 + 3 * nbProperties * 280 + nbStars * 5400;

    // logTest("GetVotable: %d stars - buffer capacity = %d bytes", nbStars, capacity);

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
    if (logGetPrintDate() == mcsTRUE)
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
    if (fileName != NULL)
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

    // Write the server version as parameter:
    votBuffer->AppendLine("<PARAM name=\"SearchCalServerVersion\" datatype=\"char\" arraysize=\"*\" unit=\"\" value=\"");
    votBuffer->AppendString(serverVersion);
    votBuffer->AppendString("\"/>");

    // xml request contains <PARAM> tags (from cmdCOMMAND)
    // note: xmlRequest starts by '\n':
    votBuffer->AppendString(xmlRequest);

    // Filter star properties once:
    int filteredPropertyIndexes[nbProperties];

    vobsSTAR_PROPERTY* starProperty;
    int propIdx = 0;
    int i = 0;

    while ((starProperty = star->GetNextProperty((mcsLOGICAL) (i == 0))) != NULL)
    {
        if (useProperty(starProperty) == mcsTRUE)
        {
            filteredPropertyIndexes[propIdx] = i;
            propIdx++;
        }

        i++;
    }

    const int nbFilteredProps = propIdx;

    // Serialize each of its properties with origin and confidence index
    // as VOTable column description (i.e FIELDS)

    const char* propertyName;
    const char* unit;
    const char* description;
    const char* link;

    i = 0; // reset counter i

    for (propIdx = 0; propIdx < nbFilteredProps; propIdx++)
    {
        starProperty = star->GetProperty(filteredPropertyIndexes[propIdx]);

        // Add standard field header
        votBuffer->AppendLine("   <FIELD");

        // Add field name
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
                votBuffer->AppendString("float");
                break;

            default:
                // Assertion - unknow type
                break;
        }
        votBuffer->AppendString("\"");

        // Add field unit if it is not vobsSTAR_PROP_NOT_SET
        unit = starProperty->GetUnit();

        // If the unit exists (not the default vobsSTAR_PROP_NOT_SET)
        if (strcmp(unit, vobsSTAR_PROP_NOT_SET) != 0)
        {
            // Add field unit
            votBuffer->AppendString(" unit=\"");
            votBuffer->AppendString(unit);
            votBuffer->AppendString("\"");
        }

        // Close FIELD opened markup
        votBuffer->AppendString(">");

        // Add field description if present
        description = starProperty->GetDescription();
        if (description != NULL)
        {
            votBuffer->AppendLine("    <DESCRIPTION>");
            votBuffer->AppendString(description);
            votBuffer->AppendString("</DESCRIPTION>");
        }

        // Add field link if present
        link = starProperty->GetLink();
        if (link != NULL)
        {
            votBuffer->AppendLine("    <LINK href=\"");
            votBuffer->AppendString(link);
            votBuffer->AppendString("\"/>");
        }

        // Add standard field footer
        votBuffer->AppendLine("   </FIELD>");
        i++;

        // Add ORIGIN field
        votBuffer->AppendLine("   <FIELD type=\"hidden\"");

        // Add field name
        votBuffer->AppendString(" name=\"");
        votBuffer->AppendString(propertyName);
        votBuffer->AppendString(".origin\"");

        // Add field ID
        votBuffer->AppendString(" ID=\"");
        sprintf(tmp, "col%d", i);
        votBuffer->AppendString(tmp);
        votBuffer->AppendString("\"");

        // Add field ucd "REFER_CODE" for the ORIGIN field:
        votBuffer->AppendString(" ucd=\"REFER_CODE\"");

        // Add field datatype
        votBuffer->AppendString(" datatype=\"char\" arraysize=\"*\"");

        // Close FIELD opened markup
        votBuffer->AppendString(">");

        // Add field description
        votBuffer->AppendLine("    <DESCRIPTION>Origin of property ");
        votBuffer->AppendString(propertyName);
        votBuffer->AppendString("</DESCRIPTION>");

        // Add standard field footer
        votBuffer->AppendLine("   </FIELD>");
        i++;

        // Add CONFIDENCE field
        votBuffer->AppendLine("   <FIELD type=\"hidden\"");

        // Add field name
        votBuffer->AppendString(" name=\"");
        votBuffer->AppendString(propertyName);
        votBuffer->AppendString(".confidence\"");

        // Add field ID
        votBuffer->AppendString(" ID=\"");
        sprintf(tmp, "col%d", i);
        votBuffer->AppendString(tmp);
        votBuffer->AppendString("\"");

        // Add field ucd "CODE_QUALITY" for the CONFIDENCE field:
        votBuffer->AppendString(" ucd=\"CODE_QUALITY\"");

        // Add field datatype
        votBuffer->AppendString(" datatype=\"int\" ");

        // Close FIELD opened markup
        votBuffer->AppendString(">");

        // Add field description
        votBuffer->AppendLine("    <DESCRIPTION>Confidence index of property ");
        votBuffer->AppendString(propertyName);
        votBuffer->AppendString("</DESCRIPTION>");

        // Add standard field footer
        votBuffer->AppendLine("   </FIELD>");
        i++;
    }

    // Add the beginning of the deletedFlag field
    // TODO: remove the deleteFlag column from server side (ASAP)
    votBuffer->AppendLine("   <FIELD type=\"hidden\" name=\"deletedFlag\" ID=\"");
    // Add field ID
    sprintf(tmp, "col%d", i);
    votBuffer->AppendString(tmp);
    // Add the end of the deletedFlag field
    votBuffer->AppendString("\" ucd=\"DELETED_FLAG\" datatype=\"boolean\">");
    // Add deleteFlag description
    votBuffer->AppendLine("    <DESCRIPTION>Used by SearchCal to flag deleted stars</DESCRIPTION>");
    // Add standard field footer
    votBuffer->AppendLine("   </FIELD>");
    // Add the beginning of the deletedFlag origin field
    votBuffer->AppendLine("   <FIELD type=\"hidden\" name=\"deletedFlag.origin\" ID=\"");
    // Add field ID
    sprintf(tmp, "col%d", i + 1);
    votBuffer->AppendString(tmp);
    // Add the end of the deletedFlag field
    votBuffer->AppendString("\" ucd=\"DELETED_FLAG.origin\" datatype=\"char\" arraysize=\"*\">");
    // Add deleteFlag description
    votBuffer->AppendLine("    <DESCRIPTION>Origin of property deletedFlag</DESCRIPTION>");
    // Add standard field footer
    votBuffer->AppendLine("   </FIELD>");
    // Add the beginning of the deletedFlag confidence indexfield
    votBuffer->AppendLine("   <FIELD type=\"hidden\" name=\"deletedFlag.confidence\" ID=\"");
    // Add field ID
    sprintf(tmp, "col%d", i + 2);
    votBuffer->AppendString(tmp);
    // Add the end of the deletedFlag field
    votBuffer->AppendString("\" ucd=\"DELETED_FLAG.confidence\" datatype=\"char\" arraysize=\"*\">");
    // Add deleteFlag description
    votBuffer->AppendLine("    <DESCRIPTION>Confidence index of property deletedFlag</DESCRIPTION>");
    // Add standard field footer
    votBuffer->AppendLine("   </FIELD>");

    // Serialize each of its properties as group description
    i = 0; // reset counter i

    for (propIdx = 0; propIdx < nbFilteredProps; propIdx++)
    {
        starProperty = star->GetProperty(filteredPropertyIndexes[propIdx]);

        // Add standard group header
        votBuffer->AppendLine("   <GROUP");

        // Add group name
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
        votBuffer->AppendString(" with its origin and confidence index</DESCRIPTION>");

        // Bind main field ref
        sprintf(tmp, "col%d", i);
        votBuffer->AppendLine("    <FIELDref ref=\"");
        votBuffer->AppendString(tmp);
        votBuffer->AppendString("\" />");

        // Bind ORIGIN field ref
        sprintf(tmp, "col%d", i + 1);
        votBuffer->AppendLine("    <FIELDref ref=\"");
        votBuffer->AppendString(tmp);
        votBuffer->AppendString("\" />");

        // Bind CONFIDENCE field ref
        sprintf(tmp, "col%d", i + 2);
        votBuffer->AppendLine("    <FIELDref ref=\"");
        votBuffer->AppendString(tmp);
        votBuffer->AppendString("\" />");

        // Add standard group footer
        votBuffer->AppendLine("   </GROUP>");

        i += 3;
    }

    // Add deleteFlag group
    // TODO: remove the deleteFlag column from server side (ASAP)
    votBuffer->AppendLine("   <GROUP name=\"deletedFlag\" ucd=\"DELETED_FLAG\">");
    // Add field description
    votBuffer->AppendLine("    <DESCRIPTION>DELETED_FLAG with its origin and confidence index</DESCRIPTION>");
    // Bind main field ref
    sprintf(tmp, "col%d", i);
    votBuffer->AppendLine("    <FIELDref ref=\"");
    votBuffer->AppendString(tmp);
    votBuffer->AppendString("\" />");
    // Bind ORIGIN field ref
    sprintf(tmp, "col%d", i + 1);
    votBuffer->AppendLine("    <FIELDref ref=\"");
    votBuffer->AppendString(tmp);
    votBuffer->AppendString("\" />");
    // Bind CONFIDENCE field ref
    sprintf(tmp, "col%d", i + 2);
    votBuffer->AppendLine("    <FIELDref ref=\"");
    votBuffer->AppendString(tmp);
    votBuffer->AppendString("\" />");
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
    const char* origin;

    // long lineSizes = 0;

    while (star != NULL)
    {
        // Add standard row header
        strcpy(line, "     <TR>");

        // reset line pointer:
        linePtr = line;

        for (propIdx = 0; propIdx < nbFilteredProps; propIdx++)
        {
            starProperty = star->GetProperty(filteredPropertyIndexes[propIdx]);

            // Add standard column header beginning
            vobsStrcatFast(linePtr, "<TD>");

            // Add value if it is not vobsSTAR_PROP_NOT_SET
            value = starProperty->GetValue();

            if (strcmp(value, vobsSTAR_PROP_NOT_SET) != 0)
            {
                vobsStrcatFast(linePtr, value);
            }

            vobsStrcatFast(linePtr, "</TD><TD>");

            // Add ORIGIN value if it is not vobsSTAR_UNDEFINED
            origin = starProperty->GetOrigin();

            if (strcmp(origin, vobsSTAR_UNDEFINED) != 0)
            {
                vobsStrcatFast(linePtr, origin);
            }

            vobsStrcatFast(linePtr, "</TD><TD>");

            // Add CONFIDENCE value if computed value
            if (starProperty->IsComputed() == mcsTRUE)
            {
                vobsStrcatFast(linePtr, vobsGetConfidenceIndexAsInt(starProperty->GetConfidenceIndex()));
            }

            // Add standard column footer
            vobsStrcatFast(linePtr, "</TD>");
        }

        // Add default deleteFlag value
        // TODO: remove the deleteFlag column from server side (ASAP)
        vobsStrcatFast(linePtr, "<TD>false</TD><TD></TD><TD></TD>");

        // Add standard row footer
        vobsStrcatFast(linePtr, "</TR>");

        votBuffer->AppendLine(line);

        // lineSizes += strlen(line);

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

        // logTest("GetVotable: line size   = %ld / %lf bytes", lineSizes, 1. * (lineSizes / (double) nbStars));
        logTest("GetVotable: size = %d bytes / capacity = %d bytes", storedBytes, capacity);
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
