#ifndef vobvobsRATOR_STAR_LIST_H
#define vobvobsRATOR_STAR_LIST_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsCALIBRATOR_STAR_LIST.h,v 1.2 2004-07-20 07:21:48 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    06-Jul-2004  Created
*
*
*******************************************************************************/

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/*
 * header files
 */
#include<list>
#include"vobsCALIBRATOR_STAR.h"

/*
 * constants
 */
/*none*/

class vobsCALIBRATOR_STAR_LIST
{ 
 public:
    vobsCALIBRATOR_STAR_LIST();//constructor
    virtual ~vobsCALIBRATOR_STAR_LIST();//destructor
   
    //void SetCalibrator(vobsCALIBRATOR_STAR_LIST x);
    //vobsCALIBRATOR_STAR getCalibrator();
    void Set(std::list<vobsCALIBRATOR_STAR>list);
    
    // this two methods may be not necessary
    void Put(vobsCALIBRATOR_STAR calibStar);
    void Remove(vobsCALIBRATOR_STAR calibStar);

    // this method sort the list of calibrator star
    void Sort();
    // this methods fill the blancks ormodify the list we have in order
    // to have coherent result.
    void Fill();

    void Filtre();

    void Save();
    
    void Print();

    int Size();
 protected:

 private:
    // Declaration of the list of calibrator star as private in order to
    // hide it from the users
    std::list<vobsCALIBRATOR_STAR> listOfStar;
};


#endif /*!vobCALIBRATOR_STAR_LIST_H*/


/*___oOo___*/
