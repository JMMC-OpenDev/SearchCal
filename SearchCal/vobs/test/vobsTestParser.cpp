/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsTestParser.cpp,v 1.11 2011-03-04 13:38:21 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.10  2011/02/25 15:58:58  lafrasse
 * Removed date and filelines from test outputs.
 *
 * Revision 1.9  2011/02/25 14:59:27  lafrasse
 * Fixed compilation due to API change.
 *
 * Revision 1.8  2009/02/03 08:53:08  mella
 * Made changes to fall back on UCD1 with viz-bin service
 *
 * Revision 1.7  2008/03/10 07:53:42  lafrasse
 * Changed VIZIER URI to reflect CDS changes.
 * Minor modifications on comments and log traces.
 *
 * Revision 1.6  2006/03/03 15:17:43  scetre
 * Changed rcsId to rcsId __attribute__ ((unused))
 *
 * Revision 1.5  2005/11/16 10:45:14  scetre
 * Updated vobs test
 *
 * Revision 1.4  2005/02/15 15:26:06  gzins
 * Changed SUCCESS/FAILURE to mcsSUCCESS/mcsFAILURE
 *
 * Revision 1.3  2005/02/13 16:01:08  gzins
 * Changed log level to logDEBUG
 *
 * Revision 1.2  2005/02/13 08:54:00  gzins
 * Updated after vobs classes changes
 * Added CVS log as modifification history
 *
 * scetre    12-Jul-2004  Created
 *
 ******************************************************************************/

static char *rcsId __attribute__ ((unused))="@(#) $Id: vobsTestParser.cpp,v 1.11 2011-03-04 13:38:21 lafrasse Exp $"; 

/* 
 * System Headers 
 */
#include <iostream>
using namespace std;
#include <string.h>

/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"

/*
 * Local Headers 
 */
#include "vobsPARSER.h"
#include "vobsPrivate.h"

/*
 * Local Variables
 */
char* uriBase = "http://vizier.u-strasbg.fr/viz-bin/asu-xml?";
//char* uriBase = "http://viz-beta.u-strasbg.fr/viz-bin/asu-xml?";

char* uriArgs[] = {
    "-source=I/280&-out.meta=hudU1&-oc.form=sexa&-c.ra=22:57:39.05&-c.dec=-29:37:20.1&Vmag=0.00..4.00&-c.eq=J2000&-out.max=100&-c.geom=b&-c.bm=3391/1200&-c.u=arcmin&-out.add=_RAJ2000,_DEJ2000&-oc=hms&-out=*POS_EQ_PMDEC&-out=*POS_EQ_PMRA&-out=*POS_PARLX_TRIG&-out=e_Plx&-out=*SPECT_TYPE_MK&-out=*PHOT_JHN_B&-out=*PHOT_JHN_V&-out=v1&-out=v2&-out=v3&-out=d5&-out=HIP&-out=HD&-out=DM&-out=TYC1&-sort=_r&SpType=%5bOBAFGKM%5d*",
    "-source=II/246/out&-out.meta=hudU1&-oc.form=sexa&-file=-c&-c.eq=J2000&-c.r=5&-c.u=arcsec&-out.max=1000&-out.add=_RAJ2000,_DEJ2000&-oc=hms&-out=2MASS&-out=Jmag&-out=Hmag&-out=Kmag&-out=*ID_CATALOG&-out=*CODE_QUALITY&-out=*POS_GAL_LAT&-out=*POS_GAL_LON&-out=*PHOT_PHG_R&-out=*PHOT_PHG_B&-out=*PHOT_PHG_V&opt=T&-out.form=List&-c=%3C%3C%3D%3D%3D%3Dresult1%5F280%2Etxt&22+57+39.049-29+37+20.05&+23+09+26.797-21+10+20.67&+23+22+58.227-20+06+02.08&+21+53+55.728-37+21+53.50&+21+26+40.027-22+24+40.80&+00+43+35.372-17+59+11.78&%3D%3D%3D%3Dresult1%5F280%2Etxt",
    "-source=II/246/out&-out.meta=hudU1&-oc.form=sexa&-c.ra=01:01:09.42&-c.dec=-72:27:28.4&Kmag=10.00..13.00&-out.max=1000&-c.rm=8&-out.add=_RAJ2000&-out.add=_DEJ2000&-oc=hms&-out=2MASS&-out=*POS_GAL_LAT&-out=*POS_GAL_LON&-out=*CODE_QUALITY&-out=Jmag&-out=Hmag&-out=Kmag&-out=*PHOT_PHG_R&-out=*PHOT_PHG_B&-out=*PHOT_PHG_V&-out=*ID_CATALOG&-sort=_r&opt=%5bTU%5d&Qflg=AAA",
    NULL
};



int main(int argc, char *argv[])
{
    // Initialize MCS services
    if (mcsInit(argv[0]) == mcsFAILURE)
    {
        // Exit from the application with FAILURE
        exit (EXIT_FAILURE);
    }

    logSetStdoutLogLevel(logTRACE);
    logSetPrintDate(mcsFALSE);
    logSetPrintFileLine(mcsFALSE);

    logInfo("Starting ...");

    vobsSTAR_LIST starList;
    vobsPARSER parser;

    int queryIndex = 0;
    while (uriArgs[queryIndex] != NULL)
    {
        logTest("--------------------------------------------------------------------------------");
        logTest("Try to retreive the xml file at the URL: %s", uriBase);

        if (parser.Parse(uriBase, uriArgs[queryIndex], "blah", starList) == mcsFAILURE)
        {
            errDisplayStack();
            errCloseStack();
        }
    
        logTest("\t Done.");
        logInfo("Size of the list = %d", starList.Size());
        logTest("--------------------------------------------------------------------------------");

        queryIndex++;
    }

    logInfo("Exiting ...");
    exit(EXIT_SUCCESS);

}


/*___oOo___*/
