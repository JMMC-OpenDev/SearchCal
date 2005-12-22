/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsVOTABLE.cpp,v 1.6 2005-12-22 10:38:45 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.5  2005/12/07 16:49:18  lafrasse
 * Added support for 'description' attribute in VOTable column descriptors FIELD.
 *
 * Revision 1.4  2005/12/07 15:28:20  lafrasse
 * Updated VOTable generation to include information about software version, request and date
 *
 * Revision 1.3  2005/12/06 08:30:21  lafrasse
 * Added support for 'ref' attribute in VOTable column descriptors FIELD.
 * Now generate validated VOTable 1.1.
 *
 * Revision 1.2  2005/12/02 17:43:42  lafrasse
 * *** empty log message ***
 *
 * Revision 1.1  2005/11/30 15:24:37  lafrasse
 * Exported VOTable generation code from vobsSTAR_LIST to vobsVOTABLE
 *
 ******************************************************************************/

/**
 * @file
 * Definition of vobsVOTABLE class.
 */

static char *rcsId="@(#) $Id: vobsVOTABLE.cpp,v 1.6 2005-12-22 10:38:45 scetre Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

/* 
 * System Headers 
 */
#include <iostream>
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
 *
 * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise. 
 *
 * @todo Automatically generate links to simbad
 */
mcsCOMPL_STAT vobsVOTABLE::Save(vobsSTAR_LIST& starList,
                                const char *fileName,
                                const char *header,
                                const char *softwareVersion,
                                const char *request)
{
    logTrace("vobsVOTABLE::Save()");

    miscoDYN_BUF buffer;

    // Add VOTable standard header
    buffer.AppendLine("<?xml version=\"1.0\"?>");
    buffer.AppendLine("");

    buffer.AppendLine("<VOTABLE version=\"1.1\"");
    buffer.AppendLine("         xmlns=\"http://www.ivoa.net/xml/VOTable/v1.1\"");
    buffer.AppendLine("         xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"");
    buffer.AppendLine("         xsi:schemaLocation=\"http://www.ivoa.net/xml/VOTable/v1.1 http://www.ivoa.net/xml/VOTable-1.1.xsd\">");
    buffer.AppendLine("");

    // Add header informations
    buffer.AppendLine(" <DESCRIPTION>");
    buffer.AppendLine("  ");
    buffer.AppendString(header);

    // Add request informations
    buffer.AppendLine("  Request parameters: ");
    buffer.AppendString(request);

    // Add current date
    mcsSTRING32 utcTime;
    if (miscGetUtcTimeStr(0, utcTime) == mcsFAILURE)
    {
        return mcsFAILURE;
    } 
    buffer.AppendLine("  Generated on (UTC): ");
    buffer.AppendString(utcTime);

    buffer.AppendLine(" </DESCRIPTION>");
    buffer.AppendLine("");

    // Add context specific informations
    buffer.AppendLine(" <COOSYS ID=\"J2000\" equinox=\"J2000.\" epoch=\"J2000.\" system=\"eq_FK5\"/>");
    buffer.AppendLine("");

    // Add software informations
    buffer.AppendLine(" <RESOURCE name=\"");
    buffer.AppendString(softwareVersion);
    buffer.AppendString("\">");

    buffer.AppendLine("  <TABLE name=\"");
    buffer.AppendString(fileName);
    buffer.AppendString("\">");

    // Get the first start of the list
    vobsSTAR* star = starList.GetNextStar(mcsTRUE);
    if (star == NULL)
    {
        // @todo add error message for empty star lists
        return mcsFAILURE;
    }

    // Serialize each of its properties as VOTable column description
    mcsUINT32   i = 0;
    mcsSTRING16 tmp;
    vobsSTAR_PROPERTY* starProperty = star->GetNextProperty(mcsTRUE);
    while (starProperty != NULL)
    {
        // Add standard field header
        buffer.AppendLine("   <FIELD");

        // Add field name
        buffer.AppendString(" name=\"");
        const char* starName = starProperty->GetName();
        buffer.AppendString(starName);
        buffer.AppendString("\"");

        // Add field ID
        buffer.AppendString(" ID=\"");
        sprintf(tmp, "col%d", i);
        buffer.AppendString(tmp);
        buffer.AppendString("\"");

        // Add field ucd
        buffer.AppendString(" ucd=\"");
        buffer.AppendString(starProperty->GetId());
        buffer.AppendString("\"");

        // Add field ref
        if ((strcmp(starName, "RAJ2000") == 0) ||
            (strcmp(starName, "DEJ2000") == 0))
        {
            buffer.AppendString(" ref=\"J2000\"");
        }

        // Add field datatype
        buffer.AppendString(" datatype=\"");
        switch (starProperty->GetType())
        {
            case vobsSTRING_PROPERTY:
                buffer.AppendString("char\" arraysize=\"8*");
                break;

            case vobsFLOAT_PROPERTY:
                buffer.AppendString("float");
                break;

            default:
                // Assertion - unknow type
                break;
        }
        buffer.AppendString("\"");

        // Add field unit
        buffer.AppendString(" unit=\"");
        buffer.AppendString(starProperty->GetUnit());
        buffer.AppendString("\"");

        // Close FIELD opened markup
        buffer.AppendString(">");
        
        // Add field description if present
        const char* description = starProperty->GetDescription();
        if (description != NULL)
        {
            buffer.AppendLine("    <DESCRIPTION>");
            buffer.AppendString(description);
            buffer.AppendString("</DESCRIPTION>");
        }
        
        // Add standard field footer
        buffer.AppendLine("   </FIELD>");

        // Retrieve the next property
        starProperty = star->GetNextProperty(mcsFALSE);
        i++;
    }

    // Serialize each star property value
    buffer.AppendLine("   <DATA>");
    buffer.AppendLine("    <TABLEDATA>");
    while (star != NULL)
    {
        // Add standard row header
        buffer.AppendLine("     <TR>");
        buffer.AppendLine("      ");

        mcsLOGICAL init = mcsTRUE;
        while((starProperty = star->GetNextProperty(init)) != NULL)
        {
            init = mcsFALSE;

            // Add standard column header
            buffer.AppendString("<TD>");

            buffer.AppendString(starProperty->GetValue());

            // Add standard column footer
            buffer.AppendString("</TD>");
        }

        // Add standard row footer
        buffer.AppendLine("     </TR>");

        // Jump on the next star of the list
        star = starList.GetNextStar();
    }

    // Add SCALIB data footer
    buffer.AppendLine("    </TABLEDATA>");
    buffer.AppendLine("   </DATA>");
    buffer.AppendLine("  </TABLE>");
    buffer.AppendLine(" </RESOURCE>");

    // Add VOTable standard footer
    buffer.AppendLine("</VOTABLE>");

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
