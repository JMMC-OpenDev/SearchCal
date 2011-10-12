#ifndef vobsCATALOG_H
#define vobsCATALOG_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Declaration of vobsCATALOG class.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#define vobsNO_CATALOG_ID           "NO CATALOG"
#define vobsCATALOG_AKARI_ID        "II/297/irc"
#define vobsCATALOG_ASCC_ID         "I/280"
#define vobsCATALOG_ASCC_LOCAL_ID   "I/280B"
#define vobsCATALOG_BSC_ID          "V/50/catalog"
#define vobsCATALOG_CIO_ID          "II/225/catalog"
#define vobsCATALOG_DENIS_ID        "B/denis"
#define vobsCATALOG_DENIS_JK_ID     "J/A+A/413/1037/table1"
#define vobsCATALOG_HIC_ID          "I/196/main"
#define vobsCATALOG_LBSI_ID         "J/A+A/393/183/catalog"
#define vobsCATALOG_MASS_ID         "II/246/out"
#define vobsCATALOG_MERAND_ID       "J/A+A/433/1155"
#define vobsCATALOG_MIDI_ID         "MIDI"
#define vobsCATALOG_PHOTO_ID        "II/7A/catalog"
#define vobsCATALOG_SBSC_ID         "V/36B/bsc4s"
#define vobsCATALOG_SB9_ID          "B/sb9/main"
#define vobsCATALOG_UNSO_ID         "I/284"
#define vobsCATALOG_WDS_ID          "B/wds/wds"

/*
 * MCS header
 */


/*
 * Local header
 */
#include "vobsREQUEST.h"
#include "vobsSTAR_LIST.h"


/*
 * Class declaration
 */

/**
 * vobsCATALOG is a class which caracterise a catalog.
 * 
 * vobsCATALOG methods allow to find a star list in a catalog
 * 
 */
class vobsCATALOG
{
public:
   // Constructor
   vobsCATALOG(const char* name);

   // Destructor
   virtual ~vobsCATALOG();

    /**
     * Get the catalog name as string literal
     *
     * @return catalog name or NULL if not set.
     */
    inline const char* GetName() const __attribute__((always_inline))
    {
        return _name;
    }

   /**
    * Get the option as string literal
    *
    * @return option or "" if not set.
    */
   inline const char* GetOption() const  __attribute__((always_inline))
   {
       if (_option == NULL)
       {
           return "";
       }
       return _option;
   }

   virtual mcsCOMPL_STAT SetOption(const char* option)
   {
        _option = option;

        return mcsSUCCESS;
   }

   // Method to get a  star list from the catalog
   virtual mcsCOMPL_STAT Search(vobsREQUEST &request, vobsSTAR_LIST &list, mcsLOGICAL logResult = mcsFALSE) = 0;
   
private:
   // Declaration of assignment operator as private
   // method, in order to hide them from the users.
   vobsCATALOG& operator=(const vobsCATALOG&);
   vobsCATALOG (const vobsCATALOG&);

   // Name of the catalog
   const char* _name;
 
   // options for the query string:
   const char* _option;

};

#endif /*!vobsCATALOG_H*/


/*___oOo___*/
