/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsVIRTUAL_OBSERVATORY.C,v 1.10 2004-08-24 16:05:52 scetre Exp $"
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

static char *rcsId="@(#) $Id: vobsVIRTUAL_OBSERVATORY.C,v 1.10 2004-08-24 16:05:52 scetre Exp $"; 
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
mcsCOMPL_STAT vobsVIRTUAL_OBSERVATORY::Research(vobsREQUEST request,
                                                vobsSTAR_LIST &starList)
{
    logExtDbg("vobsVIRTUAL_OBSERVATORY::Research()");

    // load the asking scenario with the method LoadScenario
    if (LoadScenario(request, starList) == FAILURE)
    {
        return FAILURE;
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
 *
 * \return SUCCESS on successful completion. Otherwise FAILURE is returned.
 * 
 * \b Errors codes: 
 * The possible errors are:
 */
mcsCOMPL_STAT vobsVIRTUAL_OBSERVATORY::LoadScenario(vobsREQUEST request, 
                                                    vobsSTAR_LIST &starList)
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
        vobsSTAR_LIST list1;
        vobsCATALOG_CIO cio;
        // interrogation 1 on II/225
        if (cio.Search(request, list1) == FAILURE)
        {
            return FAILURE;
        }
        // list1 is the result of the first search
        // -> star list as parameter
        
        vobsSTAR_LIST list2;
        vobsCATALOG_PHOTO photo;
        // Interrogation 2 on II/7A
        if (photo.Search(request, list2) == FAILURE)
        {
            return FAILURE;
        }
        // list2 is the result of the first search
        // -> star list as parameter

        if (list1.Merge(list2, 0.1, 0.1) == FAILURE )
        {
            return FAILURE;
        }
        // list1 + list2 -> star list as parameter

        vobsSTAR_LIST list3;
        if (list3.Copy(list1) == FAILURE )
        {
            return FAILURE;
        }
        // list3 = list1 ->  star list as parameter
        
        vobsCATALOG_ASCC ascc;
        
        // Interrogation 3 on I/280
        if (ascc.Search(request, list3) == FAILURE)
        {
            return FAILURE;
        }

        if (starList.Merge(list3) == FAILURE)
        {
            return FAILURE;
        }
        if (list3.Clear() == FAILURE)
        {
            return FAILURE;
        }
        if (list3.Copy(starList) == FAILURE)
        {
            return FAILURE;
        }

        
        if ((list1.Clear() == FAILURE)||
            (list2.Clear() == FAILURE) )
        {
            return FAILURE;
        }

        if ((list1.Copy(list3) == FAILURE) ||
            (list2.Copy(list3) == FAILURE))
        {
            return FAILURE;
        }

        vobsCATALOG_CIO cio2;
        // Interrogation 4 on II/225
        if (cio2.Search(request,list3)== FAILURE)
        {
            return FAILURE;
        }

        if (starList.Merge(list3, 0.0, 0.0, mcsTRUE) == FAILURE)
        {
            return FAILURE;
        }
         
        if ((list3.Clear() == FAILURE) ||
            (list3.Copy(list1) == FAILURE) )
        {
            return FAILURE;
        }
        
        
        vobsCATALOG_HIC hic;
        // Interrogation 5 on I/196
        if (hic.Search(request, list1) == FAILURE )
        {
            return FAILURE;
        }

        if (starList.Merge(list1, 0.0, 0.0, mcsTRUE) == FAILURE)
        {
            return FAILURE;
        }

        vobsCATALOG_MASS mass;
        // Interrogation 6 on 2MASS
        if (mass.Search(request, list2) == FAILURE)
        {
            return FAILURE;
        }
        
        if (starList.Merge(list2, 0.0, 0.0, mcsTRUE) == FAILURE)
        {
            return FAILURE;
        }

        if ( (list1.Clear() == FAILURE) ||
             (list2.Clear() == FAILURE) )
        {
            return FAILURE;
        }
        
        if ( (list1.Copy(list3) == FAILURE) ||
             (list2.Copy(list3) == FAILURE) )
        {
            return FAILURE;
        }
        
        vobsCATALOG_LBSI lbsi;
        // interrogation 7 on LBSI
        if (lbsi.Search(request, list1)== FAILURE)
        {
            return FAILURE;
        }

        if (starList.Merge(list1, 0.0, 0.0, mcsTRUE) == FAILURE)
        {
            return FAILURE;
        }

        vobsCATALOG_CHARM charm;
        // Interrogation 8 on CHARM
        if (charm.Search(request, list2) == FAILURE)
        {
            return FAILURE;
        }

        if (starList.Merge(list2, 0.0, 0.0, mcsTRUE) == FAILURE)
        {
            return FAILURE;
        }

        if ( (list1.Clear() == FAILURE) ||
             (list2.Clear() == FAILURE) )
        {
            return FAILURE;
        }
        
        if ( (list1.Copy(list3) == FAILURE) ||
             (list2.Copy(list3) == FAILURE) )
        {
            return FAILURE;
        }

       
        vobsCATALOG_CHARM photo2;
        // Interrogation 9 on II/7A
        if (photo2.Search(request, list1)== FAILURE)
        {
            return FAILURE;
        }

        if (starList.Merge(list1, 0.0, 0.0, mcsTRUE) == FAILURE)
        {
                return FAILURE;
        }

        vobsCATALOG_BSC bsc;
        // Interrogation 10 on BSC
        if (bsc.Search(request, list2) == FAILURE)
        {
            return FAILURE;
        }

        if (starList.Merge(list2, 0.0, 0.0, mcsTRUE) == FAILURE)
        {
            return FAILURE;
        }

        if ( (list1.Clear() == FAILURE) ||
             (list1.Copy(list3) == FAILURE ))
        {
            return FAILURE;
        }
        
        vobsCATALOG_SBSC sbsc;
        // Interrogation 11 on SBSC
        if (sbsc.Search(request, list1) == FAILURE)
        {
            return FAILURE;
        }

        if (starList.Merge(list1, 0.0, 0.0, mcsTRUE) == FAILURE)
        {
            return FAILURE;
        }

        vobsCATALOG_DENIS denis;
        // Interrogation 12 on DENIS
        if ( denis.Search(request, list3) == FAILURE)
        {
            return FAILURE;
        }

        if (starList.Merge(list3, 0.0, 0.0, mcsTRUE) == FAILURE)
        {
            return FAILURE;
        }
    }
    // Scenario in band V
    else if (strcmp(band, "V")==0)
    {
        vobsSTAR_LIST list1;
        vobsCATALOG_ASCC ascc;
        // Interrogation 1 on I/280
        if (ascc.Search(request, list1) == FAILURE)
        {
            return FAILURE;
        }
        // list1 is the parameter list
        if ( starList.Merge(list1) == FAILURE)
        {
            return FAILURE;
        }
        
        vobsSTAR_LIST list2;
        if ( list2.Copy(list1) == FAILURE)
        {
            return FAILURE;
        }

        // list 2 become the parameter lis
        
        vobsCATALOG_HIC hic;
        // Interrogation 2 on I/196
        if ( hic.Search(request, list2) == FAILURE)
        {
            return FAILURE;
        }

        // list 2 become a result list

        if ( starList.Merge(list2, 0.0, 0.0, mcsTRUE) == FAILURE)
        {
            return FAILURE;
        }
        
        // list2 is put in final list starlist
        if ( list2.Clear() == FAILURE)
        {
            return FAILURE;
        }

        if ( list2.Copy(list1) == FAILURE)
        {
            return FAILURE;
        }

        // list 2 become the parameter list

        vobsCATALOG_MASS mass;
        // Interrogation 3 on 2MASS
        if ( mass.Search(request, list2) == FAILURE)
        {
            return FAILURE;
        }

        // list 2 become a result list

        if ( starList.Merge(list2, 0.0, 0.0, mcsTRUE) == FAILURE)
        {
            return FAILURE;
        }

        // list2 is put in final list starlist
        if ( list2.Clear() == FAILURE)
        {
            return FAILURE;
        }

        if ( list2.Copy(list1) == FAILURE)
        {
            return FAILURE;
        }

        // list 2 become the parameter list

        vobsCATALOG_CIO cio;
        //Interrogation 4 on II/225
        if ( cio.Search(request, list2) == FAILURE)
        {
            return FAILURE;
        }

        // list 2 become a result list

        if ( starList.Merge(list2, 0.0, 0.0, mcsTRUE) == FAILURE)
        {
            return FAILURE;
        }

        // list2 is put in final list starlist
        if ( list2.Clear() == FAILURE)
        {
            return FAILURE;
        }

        if ( list2.Copy(list1) == FAILURE)
        {
            return FAILURE;
        }

        // list 2 become the parameter list

        vobsCATALOG_LBSI lbsi;
        // Interrogation 5 on LBSI
        if ( lbsi.Search(request, list2) == FAILURE)
        {
            return FAILURE;
        }

        // list 2 become a result list

        if ( starList.Merge(list2, 0.0, 0.0, mcsTRUE) == FAILURE)
        {
            return FAILURE;
        }

        // list2 is put in final list starlist
        if ( list2.Clear() == FAILURE)
        {
            return FAILURE;
        }

        if ( list2.Copy(list1) == FAILURE)
        {
            return FAILURE;
        }

        // list 2 become the parameter list

        vobsCATALOG_CHARM charm;
        // Interrogation 6 on CHARM
        if ( charm.Search(request, list2) == FAILURE)
        {
            return FAILURE;
        }

        // list 2 become a result list

        if ( starList.Merge(list2, 0.0, 0.0, mcsTRUE) == FAILURE)
        {
            return FAILURE;
        }

        // list2 is put in final list starlist
        if ( list2.Clear() == FAILURE)
        {
            return FAILURE;
        }

        if ( list2.Copy(list1) == FAILURE)
        {
            return FAILURE;
        }

        // list 2 become the parameter list

        vobsCATALOG_PHOTO photo;
        // Interrogation 7 on II/7A
        if ( photo.Search(request, list2) == FAILURE)
        {
            return FAILURE;
        }

        // list 2 become a result list

        if ( starList.Merge(list2, 0.0, 0.0, mcsTRUE) == FAILURE)
        {
            return FAILURE;
        }

        // list2 is put in final list starlist
        if ( list2.Clear() == FAILURE)
        {
            return FAILURE;
        }

        if ( list2.Copy(list1) == FAILURE)
        {
            return FAILURE;
        }

        // list 2 become the parameter list
        
        vobsCATALOG_BSC bsc;
        // Interrogation 8 on BSC
        if ( bsc.Search(request, list2) == FAILURE)
        {
            return FAILURE;
        }

        // list 2 become a result list

        if ( starList.Merge(list2, 0.0, 0.0, mcsTRUE) == FAILURE)
        {
            return FAILURE;
        }

        // list2 is put in final list starlist
        if ( list2.Clear() == FAILURE)
        {
            return FAILURE;
        }

        if ( list2.Copy(list1) == FAILURE)
        {
            return FAILURE;
        }

        // list 2 become the parameter list

        vobsCATALOG_SBSC sbsc;
        // Interrogation 9 on SBSC
        if ( sbsc.Search(request, list2) == FAILURE)
        {
            return FAILURE;
        }

        // list 2 become a result list

        if ( starList.Merge(list2, 0.0, 0.0, mcsTRUE) == FAILURE)
        {
            return FAILURE;
        }

        // list2 is put in final list starlist
        
        vobsCATALOG_DENIS denis;
        // Interrogation 10 on DENIS
        if ( denis.Search(request, list1) == FAILURE)
        {
            return FAILURE;
        }

        // list 1 become a result list

        if ( starList.Merge(list1, 0.0, 0.0, mcsTRUE) == FAILURE)
        {
            return FAILURE;
        }

        // list1 is put in final list starlist
    }
    else
    {
        return FAILURE;
    }
   
    starList.Display();
    printf("number of star found : %d\n", starList.Size());
    return SUCCESS;
}


/*___oOo___*/
