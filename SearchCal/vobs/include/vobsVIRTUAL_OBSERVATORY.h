#ifndef vobsVIRTUAL_OBSERVATORY_H
#define vobsVIRTUAL_OBSERVATORY_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsVIRTUAL_OBSERVATORY.h,v 1.1 2004-07-13 13:41:09 scetre Exp $"
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
#include"vobsCALIBRATOR_STAR_LIST.h"
#include"vobsREQUEST.h"

class vobsVIRTUAL_OBSERVATORY
{
 public:
  vobsVIRTUAL_OBSERVATORY();
  virtual ~vobsVIRTUAL_OBSERVATORY();
 
  vobsCALIBRATOR_STAR_LIST GetCalibStarList();
  
  void LoadScenario(vobsREQUEST request);
  
  vobsCALIBRATOR_STAR_LIST Research(vobsREQUEST request);
  vobsCALIBRATOR_STAR_LIST ResearchOne(vobsREQUEST request);

  void Merge(std::list<vobsCALIBRATOR_STAR>list2);
  void UpdateCalibStar(vobsCALIBRATOR_STAR &calibStar1, vobsCALIBRATOR_STAR &calibStar2);
  int CompareRaDec(char *ra1, char *dec1, char *ra2, char *dec2, float raInterval, float decInterval);
  std::list<vobsCALIBRATOR_STAR> FillAndErase( std::list<vobsCALIBRATOR_STAR>SList);
 protected:

 private:
  vobsCATALOG catalog;
  vobsCALIBRATOR_STAR_LIST StarList;
  std::list<vobsCALIBRATOR_STAR>list;
  
  std::list<char *>catalogList;
  std::list<char *>coordRaList;
  std::list<char *>coordDeList;  
  char *listForK;
  char *listForV;
  char *listOne;
};


#endif /*!vobsVIRTUAL_OBSERVATORY_H*/


/*___oOo___*/
