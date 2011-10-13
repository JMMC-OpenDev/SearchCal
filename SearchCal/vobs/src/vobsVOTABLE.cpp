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
mcsCOMPL_STAT vobsVOTABLE::GetVotable(vobsSTAR_LIST&  starList,
                                      const char*     fileName,
                                      const char*     header,
                                      const char*     softwareVersion,
                                      const char*     request,
                                      const char*     xmlRequest,
                                      miscoDYN_BUF*   buffer)
{
    // Get the first start of the list
    vobsSTAR* star = starList.GetNextStar(mcsTRUE);
    if (star == NULL)
    {
        errAdd(vobsERR_EMPTY_STAR_LIST);
        return mcsFAILURE;
    }
    
    /* buffer capacity = fixed (8K) 
     * + column definitions (3 x star->NbProperties() x 280 [248.229980] ) 
     * + data ( starList.Size() x 4000 [3694.4] ) */
    const int capacity = 8192 + 3 * star->NbProperties() * 280 + starList.Size() * 4000;

    logTest("GetVotable: buffer capacity = %d bytes", capacity);
    
    buffer->Alloc(capacity);
    
    // Add VOTable standard header
    buffer->AppendLine("<?xml version=\"1.0\"?>");
    buffer->AppendLine("");

    buffer->AppendLine("<VOTABLE version=\"1.1\"");
    buffer->AppendLine("         xmlns=\"http://www.ivoa.net/xml/VOTable/v1.1\"");
    buffer->AppendLine("         xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"");
    buffer->AppendLine("         xsi:schemaLocation=\"http://www.ivoa.net/xml/VOTable/v1.1 http://www.ivoa.net/xml/VOTable/VOTable-1.1.xsd\">");
    buffer->AppendLine("");

    // Add header informations
    buffer->AppendLine(" <DESCRIPTION>");
    buffer->AppendLine("  ");
    buffer->AppendString(header);

    // Add request informations
    buffer->AppendLine("  Request parameters: ");
    buffer->AppendString(request);

    // Add current date
    buffer->AppendLine("  Generated on (UTC): ");
    // If not in regression test mode (-noDate)
    if (logGetPrintDate() == mcsTRUE)
    {
        mcsSTRING32 utcTime;
        if (miscGetUtcTimeStr(0, utcTime) == mcsFAILURE)
        {
            return mcsFAILURE;
        } 
        buffer->AppendString(utcTime);
    }
    else
    {
        buffer->AppendString("SearchCal Regression Test Mode");
    }

    buffer->AppendLine(" </DESCRIPTION>");
    buffer->AppendLine("");

    // Add context specific informations
    buffer->AppendLine(" <COOSYS ID=\"J2000\" equinox=\"J2000.\" epoch=\"J2000.\" system=\"eq_FK5\"/>");
    buffer->AppendLine("");

    // Add software informations
    buffer->AppendLine(" <RESOURCE name=\"");
    // If not in regression test mode (-noFileLine)
    if (logGetPrintFileLine() == mcsTRUE)
    {
        buffer->AppendString(softwareVersion);
    }
    else
    {
        buffer->AppendString("SearchCal Regression Test Mode");
    }
    buffer->AppendString("\">");

    buffer->AppendLine("  <TABLE");
    if (fileName != NULL)
    {
        buffer->AppendLine(" name=\"");
        buffer->AppendString(fileName);
        buffer->AppendString("\"");
    }
    buffer->AppendString(">");

    // Add PARAMs
    buffer->AppendLine(xmlRequest);
    
    // Serialize each of its properties with origin and confidence index
    // as VOTable column description (i.e FIELDS)
    mcsUINT32   i = 0;
    mcsSTRING16 tmp;
    vobsSTAR_PROPERTY* starProperty = star->GetNextProperty(mcsTRUE);
    while (starProperty != NULL)
    {
        // Add standard field header
        buffer->AppendLine("   <FIELD");

        // Add field name
        buffer->AppendString(" name=\"");
        const char* propertyName = starProperty->GetName();
        buffer->AppendString(propertyName);
        buffer->AppendString("\"");

        // Add field ID
        buffer->AppendString(" ID=\"");
        sprintf(tmp, "col%d", i);
        buffer->AppendString(tmp);
        buffer->AppendString("\"");

        // Add field ucd
        buffer->AppendString(" ucd=\"");
        buffer->AppendString(starProperty->GetId());
        buffer->AppendString("\"");

        // Add field ref
        if ((strcmp(propertyName, "RAJ2000") == 0) ||
            (strcmp(propertyName, "DEJ2000") == 0))
        {
            buffer->AppendString(" ref=\"J2000\"");
        }

        // Add field datatype
        buffer->AppendString(" datatype=\"");
        switch (starProperty->GetType())
        {
            case vobsSTRING_PROPERTY:
                buffer->AppendString("char\" arraysize=\"*");
                break;

            case vobsFLOAT_PROPERTY:
                buffer->AppendString("float");
                break;

            default:
                // Assertion - unknow type
                break;
        }
        buffer->AppendString("\"");

        // Add field unit if it is not vobsSTAR_PROP_NOT_SET
        const char* unit = starProperty->GetUnit();
        if (unit != NULL)
        {
            // If the unit exists (not the default vobsSTAR_PROP_NOT_SET)
            if (strcmp(unit, vobsSTAR_PROP_NOT_SET) != 0)
            {
                // Add field unit
                buffer->AppendString(" unit=\"");
                buffer->AppendString(unit);
                buffer->AppendString("\"");
            }
        }

        // Close FIELD opened markup
        buffer->AppendString(">");
        
        // Add field description if present
        const char* description = starProperty->GetDescription();
        if (description != NULL)
        {
            buffer->AppendLine("    <DESCRIPTION>");
            buffer->AppendString(description);
            buffer->AppendString("</DESCRIPTION>");
        }
        
        // Add field link if present
        const char* link = starProperty->GetLink();
        if (link != NULL)
        {
            buffer->AppendLine("    <VALUES/>");
            buffer->AppendLine("    <LINK href=\"");
            buffer->AppendString(link);
            buffer->AppendString("\"/>");
        }
        
        // Add standard field footer
        buffer->AppendLine("   </FIELD>");

        // Add ORIGIN field
        i++;
        buffer->AppendLine("   <FIELD type=\"hidden\"");

        // Add field name
        buffer->AppendString(" name=\"");
        buffer->AppendString(propertyName);
        buffer->AppendString(".origin\"");

        // Add field ID
        buffer->AppendString(" ID=\"");
        sprintf(tmp, "col%d", i);
        buffer->AppendString(tmp);
        buffer->AppendString("\"");

        // Add field ucd
        buffer->AppendString(" ucd=\"");
        buffer->AppendString(starProperty->GetId());
        buffer->AppendString(".origin\"");

        // Add field datatype
        buffer->AppendString(" datatype=\"char\" arraysize=\"*\"");

        // Close FIELD opened markup
        buffer->AppendString(">");

        // Add field description
        buffer->AppendLine("    <DESCRIPTION>Origin of property ");
        buffer->AppendString(propertyName);
        buffer->AppendString("</DESCRIPTION>");

        // Add standard field footer
        buffer->AppendLine("   </FIELD>");

        // Add CONFIDENCE field
        i++;
        buffer->AppendLine("   <FIELD type=\"hidden\"");

        // Add field name
        buffer->AppendString(" name=\"");
        buffer->AppendString(propertyName);
        buffer->AppendString(".confidence\"");

        // Add field ID
        buffer->AppendString(" ID=\"");
        sprintf(tmp, "col%d", i);
        buffer->AppendString(tmp);
        buffer->AppendString("\"");

        // Add field ucd
        buffer->AppendString(" ucd=\"");
        buffer->AppendString(starProperty->GetId());
        buffer->AppendString(".confidence\"");

        // Add field datatype
        buffer->AppendString(" datatype=\"char\" arraysize=\"*\"");

        // Close FIELD opened markup
        buffer->AppendString(">");

        // Add field description
        buffer->AppendLine("    <DESCRIPTION>Confidence index of property ");
        buffer->AppendString(propertyName);
        buffer->AppendString("</DESCRIPTION>");

        // Add standard field footer
        buffer->AppendLine("   </FIELD>");

        // Retrieve the next property
        starProperty = star->GetNextProperty(mcsFALSE);
        i++;
    }
    
    // Add the beginning of the deletedFlag field
    buffer->AppendLine("   <FIELD type=\"hidden\" name=\"deletedFlag\" ID=\"");
    // Add field ID
    sprintf(tmp, "col%d", i);
    buffer->AppendString(tmp);
    // Add the end of the deletedFlag field
    buffer->AppendString("\" ucd=\"DELETED_FLAG\" datatype=\"boolean\">");
    // Add deleteFlag description
    buffer->AppendLine("    <DESCRIPTION>Used by SearchCal to flag deleted stars</DESCRIPTION>");
    // Add standard field footer
    buffer->AppendLine("   </FIELD>");
    // Add the beginning of the deletedFlag origin field
    buffer->AppendLine("   <FIELD type=\"hidden\" name=\"deletedFlag.origin\" ID=\"");
    // Add field ID
    sprintf(tmp, "col%d", i + 1);
    buffer->AppendString(tmp);
    // Add the end of the deletedFlag field
    buffer->AppendString("\" ucd=\"DELETED_FLAG.origin\" datatype=\"char\" arraysize=\"*\">");
    // Add deleteFlag description
    buffer->AppendLine("    <DESCRIPTION>Origin of property deletedFlag</DESCRIPTION>");
    // Add standard field footer
    buffer->AppendLine("   </FIELD>");
    // Add the beginning of the deletedFlag confidence indexfield
    buffer->AppendLine("   <FIELD type=\"hidden\" name=\"deletedFlag.confidence\" ID=\"");
    // Add field ID
    sprintf(tmp, "col%d", i + 2);
    buffer->AppendString(tmp);
    // Add the end of the deletedFlag field
    buffer->AppendString("\" ucd=\"DELETED_FLAG.confidence\" datatype=\"char\" arraysize=\"*\">");
    // Add deleteFlag description
    buffer->AppendLine("    <DESCRIPTION>Confidence index of property deletedFlag</DESCRIPTION>");
    // Add standard field footer
    buffer->AppendLine("   </FIELD>");
    
    // Serialize each of its properties as group description
    int j = 0;
    starProperty = star->GetNextProperty(mcsTRUE);
    while (starProperty != NULL)
    {
        // Add standard group header
        buffer->AppendLine("   <GROUP");

        // Add group name
        buffer->AppendString(" name=\"");
        const char* propertyName = starProperty->GetName();
        buffer->AppendString(propertyName);
        buffer->AppendString("\"");

        // Add group ucd
        buffer->AppendString(" ucd=\"");
        buffer->AppendString(starProperty->GetId());
        buffer->AppendString("\"");

        // Close GROUP opened markup
        buffer->AppendString(">");
        
        // Add field description
        buffer->AppendLine("    <DESCRIPTION>");
        buffer->AppendString(propertyName);
        buffer->AppendString(" with its origin and confidence index</DESCRIPTION>");
 
        // Bind main field ref
        sprintf(tmp, "col%d", j);
        buffer->AppendLine("    <FIELDref ref=\"");
        buffer->AppendString(tmp);
        buffer->AppendString("\" />");

        // Bind ORIGIN field ref
        sprintf(tmp, "col%d", j + 1);
        buffer->AppendLine("    <FIELDref ref=\"");
        buffer->AppendString(tmp);
        buffer->AppendString("\" />");

        // Bind CONFIDENCE field ref
        sprintf(tmp, "col%d", j + 2);
        buffer->AppendLine("    <FIELDref ref=\"");
        buffer->AppendString(tmp);
        buffer->AppendString("\" />");

        // Add standard group footer
        buffer->AppendLine("   </GROUP>");
        
        // Retrieve the next property
        starProperty = star->GetNextProperty(mcsFALSE);
        j += 3;
    }

    // Add deleteFlag group
    buffer->AppendLine("   <GROUP name=\"deletedFlag\" ucd=\"DELETED_FLAG\">");
    // Add field description
    buffer->AppendLine("    <DESCRIPTION>DELETED_FLAG with its origin and confidence index</DESCRIPTION>");
    // Bind main field ref
    sprintf(tmp, "col%d", j);
    buffer->AppendLine("    <FIELDref ref=\"");
    buffer->AppendString(tmp);
    buffer->AppendString("\" />");
    // Bind ORIGIN field ref
    sprintf(tmp, "col%d", j + 1);
    buffer->AppendLine("    <FIELDref ref=\"");
    buffer->AppendString(tmp);
    buffer->AppendString("\" />");
    // Bind CONFIDENCE field ref
    sprintf(tmp, "col%d", j + 2);
    buffer->AppendLine("    <FIELDref ref=\"");
    buffer->AppendString(tmp);
    buffer->AppendString("\" />");
    // Add standard group footer
    buffer->AppendLine("   </GROUP>");
    
    // Serialize each star property value
    buffer->AppendLine("   <DATA>");
    buffer->AppendLine("    <TABLEDATA>");

    // line buffer to avoid too many calls to dynamic buf:
    // Note: 8K is large enough to contain one line
    // No buffer overflow checks !
    char line[8192];
    char* linePtr;
    
    // long lineSizes = 0;
    
    while (star != NULL)
    {
        // Add standard row header
        strcpy(line, "     <TR>\n");
        
        // reset line pointer:
        linePtr = line;
        vobsStrcatFast(linePtr, "      ");

        mcsLOGICAL init = mcsTRUE;
        while((starProperty = star->GetNextProperty(init)) != NULL)
        {
            init = mcsFALSE;

            // Add standard column header beginning
            vobsStrcatFast(linePtr, "<TD>");

            // Add value if it is not vobsSTAR_PROP_NOT_SET
            const char* value = starProperty->GetValue();
            if (value != NULL)
            {
                // If the value exists (not the default vobsSTAR_PROP_NOT_SET)
                if (strcmp(value, vobsSTAR_PROP_NOT_SET) != 0)
                {
                    // Add value
                    vobsStrcatFast(linePtr, value);
                }
            }

            // Add standard column footer
            vobsStrcatFast(linePtr, "</TD>");

            // Add ORIGIN value
            vobsStrcatFast(linePtr, "<TD>");
            
            const char* origin = starProperty->GetOrigin();
            if (origin != NULL)
            {
                vobsStrcatFast(linePtr, origin);
            }

            // Add standard column footer
            vobsStrcatFast(linePtr, "</TD>");
            
            // Add CONFIDENCE value
            vobsStrcatFast(linePtr, "<TD>");

            if (starProperty->IsComputed() == mcsTRUE)
            {
                vobsStrcatFast(linePtr, vobsGetConfidenceIndex(starProperty->GetConfidenceIndex()));
            }

            // Add standard column footer
          vobsStrcatFast(linePtr, "</TD>");
        }

        // Add default deleteFlag value
        vobsStrcatFast(linePtr, "<TD>false</TD><TD></TD><TD></TD>\n");

        // Add standard row footer
        vobsStrcatFast(linePtr, "     </TR>");
        
        buffer->AppendLine(line);
        
        // lineSizes += strlen(line);

        // Jump on the next star of the list
        star = starList.GetNextStar();
    }

    // Add SCALIB data footer
    buffer->AppendLine("    </TABLEDATA>");
    buffer->AppendLine("   </DATA>");
    buffer->AppendLine("  </TABLE>");
    buffer->AppendLine(" </RESOURCE>");

    // Add VOTable standard footer
    buffer->AppendLine("</VOTABLE>");
    
    if (doLog(logTEST))
    {
        mcsUINT32 storedBytes;
        buffer->GetNbStoredBytes(&storedBytes);
        
        // logTest("GetVotable: line size   = %ld / %lf bytes", lineSizes, 1. * (lineSizes / (double)starList.Size()));
        logTest("GetVotable: buffer size = %d / %d bytes", storedBytes, capacity);
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
mcsCOMPL_STAT vobsVOTABLE::Save(vobsSTAR_LIST&  starList,
                                const char*     fileName,
                                const char*     header,
                                const char*     softwareVersion,
                                const char*     request,
                                const char*     xmlRequest)
{
    logTrace("vobsVOTABLE::Save()");

    miscoDYN_BUF buffer;

    // Get the star list in the VOTable format
    if (GetVotable(starList, fileName, header, softwareVersion, request, xmlRequest, &buffer) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Try to save the generated VOTable in the specified file as ASCII
    return(buffer.SaveInASCIIFile(fileName));
}


/*
 * Protected methods
 */


/*
 * Private methods
 */


/*___oOo___*/
