/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsVIRTUAL_OBSERVATORY.C,v 1.19 2004-11-17 08:32:34 gzins Exp $"
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

static char *rcsId="@(#) $Id: vobsVIRTUAL_OBSERVATORY.C,v 1.19 2004-11-17 08:32:34 gzins Exp $"; 
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
#include "vobs.h"
#include"vobsPrivate.h"
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
mcsCOMPL_STAT vobsVIRTUAL_OBSERVATORY::Research(vobsREQUEST &request,
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
        static vobsSTAR_LIST listP;
        static vobsSTAR_LIST listS;
        if (starList.IsEmpty() == mcsTRUE)
        {
            // II/225
            static vobsCATALOG_CIO cio;
            scenario.AddEntry(&cio, NULL, &listP, COPY, 0.0, 0.0);
            // II/7A
            static vobsCATALOG_PHOTO photo;           
            scenario.AddEntry(&photo, NULL, &listP, MERGE, 0.1, 0.1);
            // I/280
            static vobsCATALOG_ASCC ascc;
            scenario.AddEntry(&ascc, &listP, &listS, COPY, 0.0, 0.0);
        }
        else
        {
            listP.Copy(starList);
            // I/280
            static vobsCATALOG_ASCC ascc;
            scenario.AddEntry(&ascc, &listP, &listS, UPDATE_ONLY, 0.0, 0.0);
        }
        // II/225
        static vobsCATALOG_CIO cio2;
        scenario.AddEntry(&cio2, &listS, &listS, UPDATE_ONLY, 0.1, 0.1);
        // I/196
        static vobsCATALOG_HIC hic;
        scenario.AddEntry(&hic, &listS, &listS, UPDATE_ONLY, 0.1, 0.1);
        // 2MASS
        static vobsCATALOG_MASS mass;
        scenario.AddEntry(&mass, &listS, &listS, UPDATE_ONLY, 0.0, 0.0);
        // LBSI
        static vobsCATALOG_LBSI lbsi;
        scenario.AddEntry(&lbsi, &listS, &listS, UPDATE_ONLY, 0.1, 0.1);
        // CHARM
        static vobsCATALOG_CHARM charm;
        scenario.AddEntry(&charm, &listS, &listS, UPDATE_ONLY, 0.1, 0.1);
        // II/7A
        static vobsCATALOG_PHOTO photo2;
        scenario.AddEntry(&photo2, &listS, &listS, UPDATE_ONLY, 0.1, 0.1);
        // BSC
        static vobsCATALOG_BSC bsc;
        scenario.AddEntry(&bsc, &listS, &listS, UPDATE_ONLY, 0.1, 0.1);
        // SBSC
        static vobsCATALOG_SBSC sbsc;
        scenario.AddEntry(&sbsc, &listS, &listS, UPDATE_ONLY, 0.0, 0.0);
        // DENIS
        static vobsCATALOG_DENIS denis;
        scenario.AddEntry(&denis, &listS, &listS, UPDATE_ONLY, 0.0, 0.0);            

    }
    // Scenario in band V
    else if (strcmp(band, "V")==0)
    {
        static vobsSTAR_LIST listP;
        static vobsSTAR_LIST listS;

        // I/280
        static vobsCATALOG_ASCC ascc;
        if (starList.IsEmpty() == mcsTRUE)
        {
            scenario.AddEntry(&ascc, NULL, &listS, COPY, 0.0, 0.0);
        }
        else
        {
            listP.Copy(starList);
            scenario.AddEntry(&ascc, &listP, &listS, MERGE, 0.1, 0.1);
        }

        // I/196
        static vobsCATALOG_HIC hic;
        scenario.AddEntry(&hic, &listS, &listS, UPDATE_ONLY, 0.0, 0.0);
        // MASS
        static vobsCATALOG_MASS mass;
        scenario.AddEntry(&mass, &listS, &listS, UPDATE_ONLY, 0.0, 0.0);
        // II/225
        static vobsCATALOG_CIO cio;
        scenario.AddEntry(&cio, &listS, &listS, UPDATE_ONLY, 0.0, 0.0);
        // LBSI
        static vobsCATALOG_LBSI lbsi;
        scenario.AddEntry(&lbsi, &listS, &listS, UPDATE_ONLY, 0.0, 0.0);
        // CHARM
        static vobsCATALOG_CHARM charm;
        scenario.AddEntry(&charm, &listS, &listS, UPDATE_ONLY, 0.0, 0.0);
        // II/7A
        static vobsCATALOG_PHOTO photo;
        scenario.AddEntry(&photo, &listS, &listS, UPDATE_ONLY, 0.0, 0.0);
        // BSC
        static vobsCATALOG_BSC bsc;
        scenario.AddEntry(&bsc, &listS, &listS, UPDATE_ONLY, 0.0, 0.0);
        // SBSC
        static vobsCATALOG_SBSC sbsc;
        scenario.AddEntry(&sbsc, &listS, &listS, UPDATE_ONLY, 0.0, 0.0);
        // DENIS
        static vobsCATALOG_DENIS denis;
        scenario.AddEntry(&denis, &listS, &listS, UPDATE_ONLY, 0.0, 0.0);            
    }
    else
    {
        return FAILURE;
    }


    return SUCCESS;
}


/*___oOo___*/
