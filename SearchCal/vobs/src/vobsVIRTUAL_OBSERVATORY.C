/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsVIRTUAL_OBSERVATORY.C,v 1.14 2004-10-15 07:52:20 scetre Exp $"
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

static char *rcsId="@(#) $Id: vobsVIRTUAL_OBSERVATORY.C,v 1.14 2004-10-15 07:52:20 scetre Exp $"; 
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
#include "vobsCATALOG.h"
#include "vobsCATALOG_ASCC.h"
#include "vobsCATALOG_BSC.h"
#include "vobsCATALOG_CHARM.h"
#include "vobsCATALOG_CIO.h"
#include "vobsCATALOG_DENIS.h"
#include "vobsCATALOG_HIC.h"
#include "vobsCATALOG_LBSI.h"
#include "vobsCATALOG_MASS.h"
#include "vobsCATALOG_PHOTO.h"
#include "vobsCATALOG_SBSC.h"
#include "vobsSCENARIO.h"
#include"vobsPrivate.h"

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
    int kindOfRequest;
    if (request.GetKindOfRequest(kindOfRequest) == FAILURE)
    {
        return FAILURE;
    }
    if (kindOfRequest == 0)
    {
        vobsSTAR_LIST list1;
        vobsCATALOG_CIO cio;
        // interrogation 1 on II/225
        if (cio.Search(request, list1) == FAILURE)
        {
            return FAILURE;
        }
        // list1 is the result of the first search
        // -> star list as parameter

        vobsCATALOG_ASCC ascc;

        // Interrogation 3 on I/280
        if (ascc.Search(request, list1) == FAILURE)
        {
            return FAILURE;
        }
        if (starList.Merge(list1,0.3,0.3) == FAILURE)
        {
            return FAILURE;
        }

        if (starList.Size()==1)
        {
            starList.DisplayOne();
        }
        else
        {
            printf("No Info\n");
        }
    }
    else
    {
        vobsSCENARIO scenario;

        // load the asking scenario with the method LoadScenario
        if (LoadScenario(request, starList, scenario) == FAILURE)
        {
            return FAILURE;
        }

        scenario.Execute(request, starList);

        starList.Display();
        printf("number of star found : %d\n", starList.Size());
    }
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
mcsCOMPL_STAT vobsVIRTUAL_OBSERVATORY::LoadScenario(vobsREQUEST     &request, 
                                                    vobsSTAR_LIST   &starList,
                                                    vobsSCENARIO    &scenario)
{
    logExtDbg("vobsVIRTUAL_OBSERVATORY::LoadScenario()");

    mcsSTRING16 band;
    request.GetConstraint(OBSERVED_BAND_ID, band);
    // Scenario in band K
    if ((strcmp(band, "I")==0)||
        (strcmp(band, "J")==0)||
        (strcmp(band, "H")==0)||
        (strcmp(band, "K")==0) )
    { 
        static vobsSTAR_LIST listP;
        static vobsSTAR_LIST listS;
        // II/225
        static vobsCATALOG_CIO cio;
        scenario.AddEntry(&cio, NULL, &listP, COPY, 0.0, 0.0);
        // II/7A
        static vobsCATALOG_PHOTO photo;           
        scenario.AddEntry(&photo, NULL, &listP, MERGE, 0.1, 0.1);
        // I/280
        static vobsCATALOG_ASCC ascc;
        scenario.AddEntry(&ascc, &listP, &listS, COPY, 0.0, 0.0);
        // II/225
        static vobsCATALOG_CIO cio2;
        scenario.AddEntry(&cio2, &listS, &listS, UPDATE_ONLY, 0.0, 0.0);
        // I/196
        static vobsCATALOG_HIC hic;
        scenario.AddEntry(&hic, &listS, &listS, UPDATE_ONLY, 0.0, 0.0);
        // 2MASS
        static vobsCATALOG_MASS mass;
        scenario.AddEntry(&mass, &listS, &listS, UPDATE_ONLY, 0.0, 0.0);
        // LBSI
        static vobsCATALOG_LBSI lbsi;
        scenario.AddEntry(&lbsi, &listS, &listS, UPDATE_ONLY, 0.0, 0.0);
        // CHARM
        static vobsCATALOG_CHARM charm;
        scenario.AddEntry(&charm, &listS, &listS, UPDATE_ONLY, 0.0, 0.0);
        // II/7A
        static vobsCATALOG_PHOTO photo2;
        scenario.AddEntry(&photo2, &listS, &listS, UPDATE_ONLY, 0.0, 0.0);
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
    // Scenario in band V
    else if (strcmp(band, "V")==0)
    {
        static vobsSTAR_LIST listP;
        static vobsSTAR_LIST listS;

        // I/280
        static vobsCATALOG_ASCC ascc;
        scenario.AddEntry(&ascc, NULL, &listS, COPY, 0.0, 0.0);
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
