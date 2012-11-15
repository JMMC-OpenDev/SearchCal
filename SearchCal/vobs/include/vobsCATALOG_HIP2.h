#ifndef vobsCATALOG_HIP2_H
#define vobsCATALOG_HIP2_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * vobsCATALOG_HIP2 class declaration.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/*
 * Local header
 */
#include "vobsREMOTE_CATALOG.h"


/*
 * Class declaration
 */

/**
 * vobsCATALOG_HIP2 is a class which caracterise the Hipparcos, the New Reduction (van Leeuwen, 2007) Catalog.
 * 
 * vobsCATALOG_HIP2 methods allow to
 * \li Prepare a request
 * \li Send this request to the CDS
 * \li Build a star list from the CDS answer
 *
 * 
 */

class vobsCATALOG_HIP2 : public vobsREMOTE_CATALOG
{
public:
    // Constructor
    vobsCATALOG_HIP2();

    // Destructor
    virtual ~vobsCATALOG_HIP2();

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsCATALOG_HIP2(const vobsCATALOG_HIP2&);
    vobsCATALOG_HIP2& operator=(const vobsCATALOG_HIP2&);

    // Method to build specific CDS query part
    virtual mcsCOMPL_STAT WriteQuerySpecificPart(void);

    /**
     *  Use a static function that returns a new property mask (dynamic allocation)
     */
    static vobsSTAR_PROPERTY_MASK* GetOverwriteProperties()
    {
        static const char* overwriteIds [] = {
                                              vobsSTAR_POS_EQ_RA_MAIN,
                                              vobsSTAR_POS_EQ_DEC_MAIN,
                                              vobsSTAR_POS_EQ_RA_ERROR,
                                              vobsSTAR_POS_EQ_DEC_ERROR,
                                              vobsSTAR_POS_EQ_PMRA,
                                              vobsSTAR_POS_EQ_PMDEC,
                                              vobsSTAR_POS_EQ_PMRA_ERROR,
                                              vobsSTAR_POS_EQ_PMDEC_ERROR,
                                              vobsSTAR_POS_PARLX_TRIG,
                                              vobsSTAR_POS_PARLX_TRIG_ERROR
        };

        return vobsSTAR::GetPropertyMask(6, overwriteIds);
    }
};



#endif /*!vobsCATALOG_HIP2_H*/


/*___oOo___*/
