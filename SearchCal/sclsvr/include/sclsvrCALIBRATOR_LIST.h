#ifndef sclsvrCALIBRATOR_LIST_H
#define sclsvrCALIBRATOR_LIST_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: sclsvrCALIBRATOR_LIST.h,v 1.3 2004-12-06 17:04:28 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    15-Sep-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * Brief description of the header file, which ends at this dot.
 */


#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/*
 * header files
 */
#include<list>
#include "sclsvrCALIBRATOR.h"

/*
 * Class declaration
 */
class sclsvrCALIBRATOR_LIST: public vobsSTAR_LIST
{ 
public:
    sclsvrCALIBRATOR_LIST();//constructor
    virtual ~sclsvrCALIBRATOR_LIST();//destructor

    virtual mcsCOMPL_STAT Copy(vobsSTAR_LIST& list);//copy constructor
    virtual mcsCOMPL_STAT AddAtTail(sclsvrCALIBRATOR &calibrator);
    virtual mcsCOMPL_STAT AddAtTail(vobsSTAR &star);
 
    virtual mcsCOMPL_STAT Complete(vobsREQUEST request);
    
    virtual void Display();
    
    virtual mcsCOMPL_STAT Pack(miscDYN_BUF *buffer);
    virtual mcsCOMPL_STAT UnPack(miscDYN_BUF *buffer);
   
    virtual mcsCOMPL_STAT GetCoherentDiameterList(sclsvrCALIBRATOR_LIST *list);
    virtual mcsCOMPL_STAT GetVisibilityOkList(sclsvrCALIBRATOR_LIST *list);
protected:

private:
    // Declaration assignment operator as private
    // methods, in order to hide them from the users.
    sclsvrCALIBRATOR_LIST& operator=(const sclsvrCALIBRATOR_LIST&);
    sclsvrCALIBRATOR_LIST (const sclsvrCALIBRATOR_LIST& list);//copy constructor
};




#endif /*!sclsvrCALIBRATOR_LIST_H*/

/*___oOo___*/
