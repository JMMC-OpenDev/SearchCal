#ifndef vobsCATALOG_H
#define vobsCATALOG_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsCATALOG.h,v 1.1 2004-07-13 13:41:09 scetre Exp $"
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
#include<stdlib.h>
#include<errno.h>
#include<math.h>
#include<string.h>
//#include<times.h>
#include <libxml/xmlreader.h>
#include"vobsREQUEST.h"
#include"vobsCALIBRATOR_STAR_LIST.h"
#include"gdome.h"
#include"vobsPARSER.h"

class vobsCATALOG
{
 public:
  vobsCATALOG();
  vobsCATALOG(char *x);
  virtual ~vobsCATALOG();

  void SetCatalogName(char *x);
  char *GetCatalogName();
  
  char *PrepareFirst(vobsREQUEST r);
  char *Prepare(vobsREQUEST r, char *listForK);
  
  std::list<vobsCALIBRATOR_STAR> Search(vobsREQUEST request, char *catName, int kindOfReq, char *listForK);
  
  std::list<vobsCALIBRATOR_STAR> SearchOne(vobsREQUEST request, int numberOfRequest, char * listOne);
 protected:

 private:
  //vobsREQUEST request;
  std::list<vobsCALIBRATOR_STAR> listOfStar;
 
  vobsPARSER parser;
  char *_name;
  char *req;
  
  //char requete;
  //vobsCALIBARTOR_STAR_LIST liste;
};

#endif /*!vobsCATALOG_H*/


/*___oOo___*/
