/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsVIRTUAL_OBSERVATORY.cpp,v 1.1 2004-12-05 21:00:35 gzins Exp $"
 *
 * who       when         what
 * --------  -----------  -------------------------------------------------------
 * scetre    06-Jul-2004  Created
 *
 *
 *******************************************************************************/
/**
 * \file
 * vobsVIRTUAL_OBSERVATORY class definition.
 */

static char *rcsId="@(#) $Id: vobsVIRTUAL_OBSERVATORY.cpp,v 1.1 2004-12-05 21:00:35 gzins Exp $";
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
 * \return SUCCESS on successful completion. Otherwise FAILURE is returned.
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
    vobsSCENARIO scenario;

    // Get the observed band
    mcsSTRING16 band;
    request.GetConstraint(OBSERVED_BAND_ID, band);

    // load the asking scenario with the method LoadScenario
    if (LoadScenario(band, starList, scenario) == FAILURE)
    {
        return FAILURE;
    }

    starList.Clear();
    scenario.Execute(request, starList);

    //starList.Display();
    printf("number of star found : %d\n", starList.Size());

    return SUCCESS;
}


/*
 * Protected methods
 */

/**
 * Method to load research scenario.
 *
 * \param request a vobsREQUEST build
 * \param starList list of Stars to build and to send as the result of the
 * research
 * \param scenario the scenario to load
 *
 * \return SUCCESS on successful completion. Otherwise FAILURE is returned.
 *
 * \b Errors codes:
 * The possible errors are:
 */
mcsCOMPL_STAT vobsVIRTUAL_OBSERVATORY::LoadScenario(mcsSTRING16     band,
                                                    vobsSTAR_LIST   &starList,
                                                    vobsSCENARIO    &scenario)
{
    logExtDbg("vobsVIRTUAL_OBSERVATORY::LoadScenario()");

    // Scenario in band K
    if ((strcmp(band, "I")==0)||
        (strcmp(band, "J")==0)||
        (strcmp(band, "H")==0)||
        (strcmp(band, "K")==0) )
    {
        _starListP.Clear();
        _starListS.Clear();
        
        if (starList.IsEmpty() == mcsTRUE)
        {
            // II/225
            scenario.AddEntry(&_cio, NULL, &_starListP, COPY, 0.0, 0.0);
            // II/7A
            scenario.AddEntry(&_photo, NULL, &_starListP, MERGE, 0.1, 0.1);
            // I/280
            scenario.AddEntry(&_ascc, &_starListP, &_starListS, COPY, 0.0, 0.0);
        }
        else
        {
            _starListP.Copy(starList);
            // I/280
            scenario.AddEntry(&_ascc, &_starListP, &_starListS, UPDATE_ONLY, 0.0, 0.0);
        }
        // II/225
        scenario.AddEntry(&_cio, &_starListS, &_starListS, UPDATE_ONLY, 0.1, 0.1);
        // I/196
        scenario.AddEntry(&_hic, &_starListS, &_starListS, UPDATE_ONLY, 0.1, 0.1);
        // 2MASS
        scenario.AddEntry(&_mass, &_starListS, &_starListS, UPDATE_ONLY, 0.0, 0.0);
        // LBSI
        scenario.AddEntry(&_lbsi, &_starListS, &_starListS, UPDATE_ONLY, 0.1, 0.1);
        // CHARM
        scenario.AddEntry(&_charm, &_starListS, &_starListS, UPDATE_ONLY, 0.1, 0.1);
        // II/7A
        scenario.AddEntry(&_photo, &_starListS, &_starListS, UPDATE_ONLY, 0.1, 0.1);
        // BSC
        scenario.AddEntry(&_bsc, &_starListS, &_starListS, UPDATE_ONLY, 0.1, 0.1);
        // SBSC
        scenario.AddEntry(&_sbsc, &_starListS, &_starListS, UPDATE_ONLY, 0.0, 0.0);
        // DENIS
        scenario.AddEntry(&_denis, &_starListS, &_starListS, UPDATE_ONLY, 0.0, 0.0);
    }
    // Scenario in band V
    else if (strcmp(band, "V")==0)
    {
        _starListP.Clear();
        _starListS.Clear();

        // I/280
        if (starList.IsEmpty() == mcsTRUE)
        {
            scenario.AddEntry(&_ascc, NULL, &_starListS, COPY, 0.0, 0.0);
        }
        else
        {
            _starListP.Copy(starList);
            scenario.AddEntry(&_ascc, &_starListP, &_starListS, MERGE, 0.1, 0.1);
        }
        // I/196
        scenario.AddEntry(&_hic, &_starListS, &_starListS, UPDATE_ONLY, 0.0, 0.0);
        // MASS
        scenario.AddEntry(&_mass, &_starListS, &_starListS, UPDATE_ONLY, 0.0, 0.0);
        // II/225
        scenario.AddEntry(&_cio, &_starListS, &_starListS, UPDATE_ONLY, 0.0, 0.0);
        // LBSI
        scenario.AddEntry(&_lbsi, &_starListS, &_starListS, UPDATE_ONLY, 0.0, 0.0);
        // CHARM
        scenario.AddEntry(&_charm, &_starListS, &_starListS, UPDATE_ONLY, 0.0, 0.0);
        // II/7A
        scenario.AddEntry(&_photo, &_starListS, &_starListS, UPDATE_ONLY, 0.0, 0.0);
        // BSC
        scenario.AddEntry(&_bsc, &_starListS, &_starListS, UPDATE_ONLY, 0.0, 0.0);
        // SBSC
        scenario.AddEntry(&_sbsc, &_starListS, &_starListS, UPDATE_ONLY, 0.0, 0.0);
        // DENIS
        scenario.AddEntry(&_denis, &_starListS, &_starListS, UPDATE_ONLY, 0.0, 0.0);
    }
    else
    {
        return FAILURE;
    }


    return SUCCESS;
}


/*___oOo___*/
