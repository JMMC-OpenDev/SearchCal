/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsVIRTUAL_OBSERVATORY.cpp,v 1.22 2005-02-22 13:23:16 scetre Exp $"
*
* History
* -------
* $Log: not supported by cvs2svn $
* Revision 1.21  2005/02/16 17:02:04  scetre
* Updated criteria association
*
* Revision 1.20  2005/02/16 14:28:31  scetre
* *** empty log message ***
*
* Revision 1.19  2005/02/10 08:07:38  scetre
* changed parser and hd, hip, dm number id in order to get all of them even if they have the same UCD
*
* Revision 1.18  2005/02/09 06:10:44  gzins
* Changed vobsSTAR_COMP_CRITERIA_LIST& to vobsSTAR_COMP_CRITERIA_LIST* in vobsSCENARIO
*
* Revision 1.17  2005/02/08 11:10:04  scetre
* changed action enumerate <NAME> in vobs<NAME>
*
* Revision 1.16  2005/02/08 11:10:04  scetre
* changed action enumerate <NAME> in vobs<NAME>
*
* Revision 1.15  2005/02/08 04:44:14  gzins
* Cleared all input/output lists when creating scenario
*
* Revision 1.14  2005/02/07 19:40:58  gzins
* Updated vobsREQUEST API
*
* Revision 1.13  2005/02/07 17:36:00  scetre
* updated band V scenario
*
* Revision 1.12  2005/02/04 14:31:50  scetre
* updated documentation
*
* Revision 1.11  2005/02/04 10:50:37  gzins
* Added N band scenario
*
* Revision 1.10  2005/02/04 07:51:15  gzins
* Changed printf to logTest
*
* Revision 1.9  2005/02/04 07:43:16  gzins
* Fixed bug related to creation of primary list when porviding a existing list of stars; list was merged instaed of copied.
*
* Revision 1.8  2005/01/28 12:48:24  scetre
* changed association criteria
*
* Revision 1.7  2005/01/27 13:46:57  scetre
* change order of entry in K band
*
* Revision 1.6  2005/01/26 08:20:00  scetre
* Chamge history. Use instence of criteria list instead of pointeur. Update criteria list affectation in LoadScenario()
*
* scetre    06-Jul-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * vobsVIRTUAL_OBSERVATORY class definition.
 */

static char *rcsId="@(#) $Id: vobsVIRTUAL_OBSERVATORY.cpp,v 1.22 2005-02-22 13:23:16 scetre Exp $";
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

/*
 * Local Headers
 */
#include "vobsVIRTUAL_OBSERVATORY.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"

/*
 * Class constructor
 */

/**
 * Build a virtual observatory object.
 */

vobsVIRTUAL_OBSERVATORY::vobsVIRTUAL_OBSERVATORY()
{
}

/*
 * Class destructor
 */

/**
 * Delete a virtual observatory object.
 */
vobsVIRTUAL_OBSERVATORY::~vobsVIRTUAL_OBSERVATORY()
{
}

/**
 * Method to start the research according to the constarints of the request.
 *
 * \param request a vobsREQUEST build
 * \param starList list of Stars to build and to send as the result of the
 * research
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 *
 * \b Errors codes:
 * The possible errors are:
 */
/*
 * Public methods
 */
mcsCOMPL_STAT vobsVIRTUAL_OBSERVATORY::Search(vobsREQUEST &request,
                                                vobsSTAR_LIST &starList)
{
    logExtDbg("vobsVIRTUAL_OBSERVATORY::Research()");

    // Create a asking scenario in which will be stored catalog to query
    vobsSCENARIO scenario;

    // Get the observed band
    const char *band;
    band = request.GetSearchBand();

    // load the asking scenario with the method LoadScenario
    if (LoadScenario(band, starList, scenario) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Clear the resulting list
    starList.Clear();
    // Run the method to execute the scenario which had been loaded into memory
    scenario.Execute(request, starList);
    
    //starList.Display();
    logTest("Number of stars found : %d\n", starList.Size());

    return mcsSUCCESS;
}


/*
 * Protected methods
 */

/**
 * Method to load research scenario.
 *
 * \param band the observed band 
 * \param starList list of Stars to build and to send as the result of the
 * research
 * \param scenario the scenario to load
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 *
 * \b Errors codes:
 * The possible errors are:
 */
mcsCOMPL_STAT vobsVIRTUAL_OBSERVATORY::LoadScenario(const char      *band,
                                                    vobsSTAR_LIST   &starList,
                                                    vobsSCENARIO    &scenario)
{
    logExtDbg("vobsVIRTUAL_OBSERVATORY::LoadScenario()");
   

    // Clear the list input and list output which will be used
    _starListP.Clear();
    _starListS.Clear();

    // Create a criteria list
    vobsSTAR_COMP_CRITERIA_LIST criteriaList;

    // Add criteria on right ascension
    if (criteriaList.Add(vobsSTAR_POS_EQ_RA_MAIN, 0.1) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // Add criteria on declinaison
    if (criteriaList.Add(vobsSTAR_POS_EQ_DEC_MAIN, 0.1) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    // Scenario in band K
    if ((strcmp(band, "I")==0)||
        (strcmp(band, "J")==0)||
        (strcmp(band, "H")==0)||
        (strcmp(band, "K")==0) )
    {
        // If the lis is empty, it is a calibrator research
        // the primary is build with the result of the catalog II/225, II/7A and
        // I/280
        if (starList.IsEmpty() == mcsTRUE)
        {
            // II/225
            scenario.AddEntry(&_cio, NULL, &_starListP, vobsCOPY);
            // II/7A
            scenario.AddEntry(&_photo, NULL, &_starListP, vobsMERGE);
            // I/280
            scenario.AddEntry(&_ascc, &_starListP, &_starListS, vobsCOPY);
        }
        // If the list is not empty, it is a single resarch
        // The catalog I/280 is used to build the primary list of one star
        else
        {
            _starListP.Copy(starList);
            // I/280
            scenario.AddEntry(&_ascc, &_starListP, &_starListS, vobsCOPY,
                              &criteriaList);
        }

        /* 
         * The primary list is completed with the query on catalogs II/225, 
         * I/196, 2MASS, LBSI, CHARM, II/7A, BSC, SBSC, DENIS
         */
        
        /*
         * Change criteria on right ascension and declinaison
         */
        if (criteriaList.Remove(vobsSTAR_POS_EQ_RA_MAIN) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
        if (criteriaList.Remove(vobsSTAR_POS_EQ_RA_MAIN) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
        if (criteriaList.Add(vobsSTAR_POS_EQ_RA_MAIN, 0.01) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
        if (criteriaList.Add(vobsSTAR_POS_EQ_DEC_MAIN, 0.01) == mcsFAILURE)
        {
            return mcsFAILURE;
        }

        // II/225
        scenario.AddEntry(&_cio, &_starListS, &_starListS, vobsUPDATE_ONLY,
                          &criteriaList);
        /*
         * Add mgV criteria
         */
        /*if (criteriaList.Add(vobsSTAR_PHOT_JHN_V, 0.1) == mcsFAILURE)
        {
            return mcsFAILURE;
        }*/
        // II/7A
        scenario.AddEntry(&_photo, &_starListS, &_starListS, vobsUPDATE_ONLY,
                          &criteriaList);
        // Removed mgV criteria
        /*if (criteriaList.Remove(vobsSTAR_PHOT_JHN_V) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
        // Add mgK criteria
        if (criteriaList.Add(vobsSTAR_PHOT_JHN_K, 0.3) == mcsFAILURE)
        {
            return mcsFAILURE;
        }*/
        
        // 2MASS
        scenario.AddEntry(&_mass, &_starListS, &_starListS, vobsUPDATE_ONLY,
                          &criteriaList);
        // LBSI
        scenario.AddEntry(&_lbsi, &_starListS, &_starListS, vobsUPDATE_ONLY,
                          &criteriaList);
        // CHARM
        scenario.AddEntry(&_charm, &_starListS, &_starListS, vobsUPDATE_ONLY,
                          &criteriaList);        
        // DENIS
        scenario.AddEntry(&_denis, &_starListS, &_starListS, vobsUPDATE_ONLY,
                          &criteriaList);        
        // Removed mgK criteria
        /*if (criteriaList.Remove(vobsSTAR_PHOT_JHN_K) == mcsFAILURE)
        {
            return mcsFAILURE;
        }*/

        // Add hd criteria
        if (criteriaList.Add(vobsSTAR_ID_HD, 0) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
        // I/196
        scenario.AddEntry(&_hic, &_starListS, &_starListS, vobsUPDATE_ONLY,
                          &criteriaList);
        // BSC
        scenario.AddEntry(&_bsc, &_starListS, &_starListS, vobsUPDATE_ONLY,
                          &criteriaList);
        // SBSC
        scenario.AddEntry(&_sbsc, &_starListS, &_starListS, vobsUPDATE_ONLY,
                          &criteriaList);
    }
    // Scenario in band V
    else if (strcmp(band, "V")==0)
    {
        // If the lis is empty, it is a calibrator research
        // the primary is build with the result of the catalog I/280
        if (starList.IsEmpty() == mcsTRUE)
        {
            scenario.AddEntry(&_ascc, NULL, &_starListS, vobsCOPY);
        }
        // If the list is not empty, it is a single resarch
        // The catalog I/280 is used to build the primary list of one star     
        else
        {
            _starListP.Copy(starList);
            scenario.AddEntry(&_ascc, &_starListP, &_starListS, vobsCOPY,
                              &criteriaList);
        }
        // The primary list is completed with the query on catalogs I/196,
        // MASS, II/225, LBSI, CHARM, II/7A, BSC, SBSC, DENIS
        // I/196
        scenario.AddEntry(&_hic, &_starListS, &_starListS, vobsUPDATE_ONLY,
                          &criteriaList);
        // MASS
        scenario.AddEntry(&_mass, &_starListS, &_starListS, vobsUPDATE_ONLY,
                          &criteriaList);
        // II/225
        scenario.AddEntry(&_cio, &_starListS, &_starListS, vobsUPDATE_ONLY,
                          &criteriaList);
        // LBSI
        scenario.AddEntry(&_lbsi, &_starListS, &_starListS, vobsUPDATE_ONLY,
                          &criteriaList);
        // CHARM
        scenario.AddEntry(&_charm, &_starListS, &_starListS, vobsUPDATE_ONLY,
                          &criteriaList);
        // II/7A
        scenario.AddEntry(&_photo, &_starListS, &_starListS, vobsUPDATE_ONLY,
                          &criteriaList);
        // BSC
        scenario.AddEntry(&_bsc, &_starListS, &_starListS, vobsUPDATE_ONLY,
                          &criteriaList);
        // SBSC
        scenario.AddEntry(&_sbsc, &_starListS, &_starListS, vobsUPDATE_ONLY,
                          &criteriaList);
        // DENIS
        scenario.AddEntry(&_denis, &_starListS, &_starListS, vobsUPDATE_ONLY,
                          &criteriaList);
    }
    // Scenario in band N
    else if (strcmp(band, "N") == 0)
    {
        // Just consult MIDI catalog 
        _starListP.Copy(starList);
        scenario.AddEntry(&_midi, NULL, &_starListS, vobsCOPY);
    }
    // Until now, the virtual observatory can't find star in other band
    else
    {
        errAdd(vobsERR_UNKNOWN_BAND, band);
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}


/*___oOo___*/
