/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsREQUEST.cpp,v 1.4 2005-01-24 10:58:44 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    26-Jul-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * vobsREQUEST class definition.
 */

static char *rcsId="@(#) $Id: vobsREQUEST.cpp,v 1.4 2005-01-24 10:58:44 scetre Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

/* 
 * System Headers 
 */
#include <iostream>
using namespace std; 
/**
 * Export standard iostream objects (cin, cout,...).
 */


/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"


/*
 * Local Headers 
 */
#include "vobsREQUEST.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"


/* Local variables */
static char *constraintNameList[] =
{
   "STAR_NAME",
   "RA_ID",
   "DEC_ID",
   "STAR_WLEN",
   "STAR_MAGNITUDE",
   "MIN_MAGNITUDE_RANGE",
   "MAX_MAGNITUDE_RANGE",   
   "SEARCH_BOX_RA",
   "SEARCH_BOX_DEC",
   "STAR_EXPECTED_VIS",
   "STAR_MAX_ERR_VIS",
   "OBSERVED_BAND",
   "BASEMIN",
   "BASEMAX",
   NULL
};

/*
 * Class constructor
 */

/**
 * Build a request object.
 *
 * \n 
 * \sa OPTIONAL. See also section, in witch you can refer other documented
 * entities. Doxygen will create the link automatically.
 * \sa For example modcppPERSON(char *name).
 * 
 */
vobsREQUEST::vobsREQUEST()
{
    for (int constraintId=0; constraintId<vobsNB_REQUEST_CONSTRAINTS; constraintId++)
    {
        strcpy(_constraints[constraintId], vobsREQUEST_CONSTRAINT_NOT_SET);
    }
}
/**
 * Build a request object from another one (copy constructor).
 */
vobsREQUEST::vobsREQUEST(const vobsREQUEST &request)
{
    for (int constraintId=0; constraintId<vobsNB_REQUEST_CONSTRAINTS; constraintId++)
    {
        request.GetConstraint((vobsCONSTRAINT_ID)constraintId, (char *)_constraints[constraintId]);
    }
}

/*
 * Class destructor
 */

/** Delete a request object. */
vobsREQUEST::~vobsREQUEST()
{
}

/*
 * Public methods
 */

/**
 * Set a request constraint.
 *
 * Set value constraint corresponding to the CONSTRAINT definition
 * 
 * \param constraint CONSTRAINT name. 
 * \param value constraint value to set
 * 
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 *
 * \b Errors code:\n
 * The possible errors are :
 * \li vobsERR_INVALID_CONSTRAINT
 *
 */
mcsCOMPL_STAT vobsREQUEST::SetConstraint(char *constraint, char *value)
{
    //logExtDbg("vobsSTAR::SetConstraint()");

    // Get Id corresponding to the specified CONSTRAINT
    vobsCONSTRAINT_ID constraintId;
    constraintId = Constraint2Id(constraint);
    if (constraintId == UNKNOWN_CONSTRAINT_ID)
    {
        errAdd(vobsERR_INVALID_CONSTRAINT, constraint);
        return mcsFAILURE;
    }
    // Affect constraint value
    strcpy(_constraints[constraintId], value);

    return mcsSUCCESS;
}

/**
 * Set a request constraint.
 *
 * Set value constraint corresponding to the CONSTRAINT definition
 * 
 * \param constraintId CONSTRAINT id. 
 * \param value constraint value to set
 * 
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 *
 * \b Errors code:\n
 * The possible errors are :
 * \li vobsERR_INVALID_CONSTRAINT
 *
 */
mcsCOMPL_STAT vobsREQUEST::SetConstraint(vobsCONSTRAINT_ID constraintId, char *value)
{
    //logExtDbg("vobsSTAR::SetConstraint()");

    // Get Id corresponding to the specified CONSTRAINT
    if (constraintId == UNKNOWN_CONSTRAINT_ID)
    {
        errAdd(vobsERR_INVALID_CONSTRAINT, constraintId);
        return mcsFAILURE;
    }
    // Affect constraint value
    strcpy(_constraints[constraintId], value);

    return mcsSUCCESS;
}

/**
 * Get a request constraint, as a string.
 *
 * Get value constraint, as a string, corresponding to the CONSTRAINT
 * definition
 * 
 * \param constraint CONSTRAINT name. 
 * \param value constraint value to get, as a string.
 * 
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 *
 * \b Errors code:\n
 * The possible errors are :
 * \li vobsERR_INVALID_CONSTRAINT
 *
 */
mcsCOMPL_STAT vobsREQUEST::GetConstraint(char *constraint, char *value)
{
    //logExtDbg("vobsSTAR::GetConstraint()");

    // Get Id corresponding to the specified CONSTRAINT
    vobsCONSTRAINT_ID constraintId;
    constraintId = Constraint2Id(constraint);
    if (constraintId == UNKNOWN_CONSTRAINT_ID)
    {
        errAdd(vobsERR_INVALID_CONSTRAINT, constraint);
        return mcsFAILURE;
    }
    
    // Retrieve constraint value
    strcpy(value,_constraints[constraintId]);

    return mcsSUCCESS;
}

/**
 * Get a request constraint, as a string.
 *
 * Get value constraint, as a string, corresponding to the CONSTRAINT
 * definition
 * 
 * \param constraintId CONSTRAINT id. 
 * \param value constraint value to get, as a string. 
 * 
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 *
 * \b Errors code:\n
 * The possible errors are :
 * \li vobsERR_INVALID_CONSTRAINT
 */
mcsCOMPL_STAT vobsREQUEST::GetConstraint(vobsCONSTRAINT_ID constraintId , char *value) const
{
    //logExtDbg("vobsSTAR::GetConstraint()");
    
    // Check CONSTRAINT id
    if ((constraintId <= UNKNOWN_CONSTRAINT_ID) ||
        (constraintId >= vobsNB_REQUEST_CONSTRAINTS))
    {
        errAdd(vobsERR_INVALID_CONSTRAINT, constraintId);
        return mcsFAILURE;
    }
   
    // Retrieve string value form the constraint
    strcpy(value, _constraints[constraintId]);
            
    return mcsSUCCESS;
}

/**
 * Get a request constraint, as an integer.
 *
 * Get value constraint, as an integer, corresponding to the CONSTRAINT
 * definition
 * 
 * \param constraint CONSTRAINT name. 
 * \param value constraint value to get, as an integer. 
 * 
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 *
 */
mcsCOMPL_STAT vobsREQUEST::GetConstraint(char *constraint, int *value)
{
    //logExtDbg("vobsSTAR::GetConstraint()");

    // Get Id corresponding to the specified CONSTRAINT
    vobsCONSTRAINT_ID constraintId;
    constraintId = Constraint2Id(constraint);

    // Retrieve and return constraint value
    return (GetConstraint(constraintId, value));
}

/**
 * Get a request constraint, as an integer.
 *
 * Get value constraint value, as an integer, corresponding to the CONSTRAINT
 * definition
 * 
 * \param constraintId CONSTRAINT id. 
 * \param value constraint value to get, as an integer. 
 * 
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 *
 * \b Errors code:\n
 * The possible errors are :
 * \li vobsERR_INVALID_CONSTRAINT
 * \li vobsERR_INVALID_CONSTRAINT_FORMAT
 *
 */
mcsCOMPL_STAT vobsREQUEST::GetConstraint(vobsCONSTRAINT_ID constraintId, int *value) const
{
    //logExtDbg("vobsSTAR::GetConstraint()");

    // Check CONSTRAINT id
    if ((constraintId <= UNKNOWN_CONSTRAINT_ID) ||
        (constraintId >= vobsNB_REQUEST_CONSTRAINTS))
    {
        errAdd(vobsERR_INVALID_CONSTRAINT, constraintId);
        return mcsFAILURE;
    }

    // Convert constraint string value to integer value
    if (sscanf(_constraints[constraintId], "%d", value) != 1)
    {
        errAdd(vobsERR_INVALID_CONSTRAINT_FORMAT, value, _constraints[constraintId]);
        return mcsFAILURE;
    }
        
    return mcsSUCCESS;
}

/**
 * Get a request constraint, as a float.
 *
 * Get value constraint, as a float, corresponding to the CONSTRAINT
 * definition
 * 
 * \param constraint CONSTRAINT name. 
 * \param value constraint value to get, as a float. 
 * 
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 *
 */
mcsCOMPL_STAT vobsREQUEST::GetConstraint(char *constraint, float *value)
{
    //logExtDbg("vobsSTAR::GetConstraint()");

    // Get Id corresponding to the specified CONSTRAINT
    vobsCONSTRAINT_ID constraintId;
    constraintId = Constraint2Id(constraint);

    // Retrieve and return constraint value
    return (GetConstraint(constraintId, value));
}

/**
 * Get a request constraint, as a float.
 *
 * Get value constraint value, as a float, corresponding to the CONSTRAINT
 * definition
 * 
 * \param constraintId CONSTRAINT id. 
 * \param value constraint value to get, as a float. 
 * 
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 *
 * \b Errors code:\n
 * The possible errors are :
 * \li vobsERR_INVALID_CONSTRAINT
 * \li vobsERR_INVALID_CONSTRAINT_FORMAT
 *
 */
mcsCOMPL_STAT vobsREQUEST::GetConstraint(vobsCONSTRAINT_ID constraintId, float *value) const
{
    //logExtDbg("vobsREQUEST::GetConstraint()");

    // Check CONSTRAINT id
    if ((constraintId <= UNKNOWN_CONSTRAINT_ID) ||
        (constraintId >= vobsNB_REQUEST_CONSTRAINTS))
    {
        errAdd(vobsERR_INVALID_CONSTRAINT, constraintId);
        return mcsFAILURE;
    }

    // Convert constraint string value to integer value
    if (sscanf(_constraints[constraintId], "%f", value) != 1)
    {
        errAdd(vobsERR_INVALID_CONSTRAINT_FORMAT, value, _constraints[constraintId]);
        return mcsFAILURE;
    }
        
    return mcsSUCCESS;
}

/**
 * Test if the request is build correctly
 *
 * \return TRUE on successful completion. Otherwise FALSE is returned.
 * \b Errors code:\n
 * The possible errors are :
 * \li vobsERR_CHECK_OF_KIND_OF_REQUEST_FAILED
 * \li vobsERR_CHECK_FOR_SIMPLE_STAR_FAILED
 * \li vobsERR_CHECK_FOR_STAR_LIST_FAILED
 */
mcsLOGICAL vobsREQUEST::Check(void)
{
    logExtDbg("vobsREQUEST::Check()");

    // Check if the CONSTRAINTS are build for the star list search
    for (int i=0; i<vobsNB_REQUEST_CONSTRAINTS; i++)
    {
        if (strcmp(_constraints[(vobsCONSTRAINT_ID)i],
                   vobsREQUEST_CONSTRAINT_NOT_SET)==0)
        {
            errAdd(vobsERR_CHECK_FOR_STAR_LIST_FAILED);
            return mcsFALSE;
        }
    }

    return mcsTRUE;
}

/**
 * Display all request constraints on the console.
 *
 * 
 */
void vobsREQUEST::Display(void)
{
    logExtDbg("vobsREQUEST::Display()");
    cout << "-----------------------------------------" << endl;
    for (int i=0; i<vobsNB_REQUEST_CONSTRAINTS; i++)
    {
        cout << constraintNameList[i] << " = " << _constraints[i] << endl;
    }
}

/*
 * Protected methods
 */

/**
 * Convert a given CONSTRAINT to an id.
 *
 * \param constraint CONSTRAINT name.
 *
 * \return an CONSTRAINT id.
 *
 */
vobsCONSTRAINT_ID vobsREQUEST::Constraint2Id(char *constraint) const
{
    logExtDbg("vobsREQUEST::Constraint2Id()");
    
    for (int i=0; i<vobsNB_REQUEST_CONSTRAINTS; i++)
    {
        if (strcmp(constraintNameList[i], constraint) == 0)
        {
            return (vobsCONSTRAINT_ID)i;
        }
    }

    return UNKNOWN_CONSTRAINT_ID;
}

/*___oOo___*/
