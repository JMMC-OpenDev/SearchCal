#ifndef vobsVIRTUAL_OBSERVATORY_H
#define vobsVIRTUAL_OBSERVATORY_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsVIRTUAL_OBSERVATORY.h,v 1.2 2004-07-28 14:19:41 scetre Exp $"
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
#include"vobsCATALOG.h"
#include"vobsSTAR_LIST.h"
#include"vobsREQUEST.h"

class vobsVIRTUAL_OBSERVATORY
{
 public:
  vobsVIRTUAL_OBSERVATORY();
  virtual ~vobsVIRTUAL_OBSERVATORY();
 
  vobsSTAR_LIST GetCalibStarList();
  
  void LoadScenario(vobsREQUEST request);
  
  vobsSTAR_LIST Research(vobsREQUEST request);
  vobsSTAR_LIST ResearchOne(vobsREQUEST request);

  void Merge(std::list<vobsSTAR>list2);
  void UpdateCalibStar(vobsSTAR &calibStar1, vobsSTAR &calibStar2);
  int CompareRaDec(char *ra1, char *dec1, char *ra2, char *dec2, float raInterval, float decInterval);
  std::list<vobsSTAR> FillAndErase( std::list<vobsSTAR>SList);
 protected:

 private:
  vobsCATALOG catalog;
  vobsSTAR_LIST StarList;
  std::list<vobsSTAR>list;
  
  std::list<char *>catalogList;
  std::list<char *>coordRaList;
  std::list<char *>coordDeList;  
  char *listForK;
  char *listForV;
  char *listOne;
};


#endif /*!vobsVIRTUAL_OBSERVATORY_H*/


/*___oOo___*/
