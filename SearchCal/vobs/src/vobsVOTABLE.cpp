/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsVOTABLE.cpp,v 1.2 2005-12-02 17:43:42 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2005/11/30 15:24:37  lafrasse
 * Exported VOTable generation code from vobsSTAR_LIST to vobsVOTABLE
 *
 ******************************************************************************/

/**
 * \file
 *  Definition of vobsVOTABLE class.
 */

static char *rcsId="@(#) $Id: vobsVOTABLE.cpp,v 1.2 2005-12-02 17:43:42 lafrasse Exp $"; 
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
 *
 * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise. 
 */
mcsCOMPL_STAT vobsVOTABLE::Save(vobsSTAR_LIST&  starList,
                                const char*           fileName)
{
    logTrace("vobsVOTABLE::Save()");

    miscoDYN_BUF buffer;

    // Add VOTable standard header
    buffer.AppendLine("<?xml version=\"1.0\"?>");
    buffer.AppendLine(" <VOTABLE version=\"1.1\"");
    buffer.AppendLine("          xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"");
    buffer.AppendLine("          xsi:noNamespaceSchemaLocation=\"http://www.ivoa.net/xml/VOTable/VOTable-1.1.xsd\">");

    // Add SCALIB specific informations
    buffer.AppendLine(" <DESCRIPTION>");
    buffer.AppendLine("  SCALIB output");
    buffer.AppendLine(" </DESCRIPTION>");

    // Add context specific informations
    buffer.AppendLine(" <COOSYS ID=\"J2000\" equinox=\"J2000.\" epoch=\"J2000.\" system=\"eq_FK5\"/>");

    // Add SCALIB data header
    buffer.AppendLine(" <RESOURCE name=\"SCALIB calibrators\">");
    buffer.AppendLine("  <TABLE name=\"results\">");
    buffer.AppendLine("   <DESCRIPTION>");
    buffer.AppendLine("    Calibrators for the ");
    //buffer.AppendString(science object name);
    buffer.AppendString(" science objetc");
    buffer.AppendLine("   </DESCRIPTION>");

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
        buffer.AppendString(starProperty->GetName());
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
/*
        // Add field ref
        buffer.AppendString(" ref=\"");
        //buffer.AppendString(starProperty->GetRef());
        buffer.AppendString("\"");
*/
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

        // Add standard row footer
        buffer.AppendString("/>");

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
