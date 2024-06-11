#ifndef sclsvrCALIBRATOR_LIST_H
#define sclsvrCALIBRATOR_LIST_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Brief description of the header file, which ends at this dot.
 */


#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/*
 * System header
 */
#include <list>

/*
 * Local header
 */
#include "sclsvrCALIBRATOR.h"

/*
 * Class declaration
 */
class sclsvrCALIBRATOR_LIST : public vobsSTAR_LIST
{
public:
    // Constructor
    sclsvrCALIBRATOR_LIST(const char* name);
    // Destructor
    virtual ~sclsvrCALIBRATOR_LIST();

    /** Convert stars to calibrators and clear the given star list */
    virtual void Move(vobsSTAR_LIST& list);

    virtual void Copy(const vobsSTAR_LIST& list);

    virtual void AddAtTail(const sclsvrCALIBRATOR &calibrator);
    virtual void AddAtTail(const vobsSTAR &star);

    virtual mcsCOMPL_STAT Complete(const sclsvrREQUEST &request);

    virtual mcsCOMPL_STAT Pack(miscoDYN_BUF *buffer);
    virtual mcsCOMPL_STAT UnPack(const char *buffer);

    virtual mcsCOMPL_STAT Save(const char *filename,
                               const sclsvrREQUEST &request,
                               mcsLOGICAL extendedFormat = mcsFALSE);

    virtual mcsCOMPL_STAT Load(const char *filename,
                               sclsvrREQUEST &request);

    mcsCOMPL_STAT GetTSV(const char* header,
                         const char* softwareVersion,
                         const char* request,
                         miscoDYN_BUF *buffer);

    mcsCOMPL_STAT SaveTSV(const char *filename,
                          const char* header,
                          const char* softwareVersion,
                          const char* request);

    virtual mcsCOMPL_STAT GetScienceObject(sclsvrCALIBRATOR &scienceObject) const;

    /** Filter this list to keep only valid diameters */
    virtual mcsCOMPL_STAT FilterDiameterOk();


protected:


private:
    // Declaration assignment operator as private
    // methods, in order to hide them from the users.
    sclsvrCALIBRATOR_LIST& operator=(const sclsvrCALIBRATOR_LIST&) ;
    sclsvrCALIBRATOR_LIST(const sclsvrCALIBRATOR_LIST& list); //copy constructor
} ;


#endif /*!sclsvrCALIBRATOR_LIST_H*/

/*___oOo___*/
