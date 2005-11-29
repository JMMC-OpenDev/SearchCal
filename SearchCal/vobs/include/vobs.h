#ifndef vobs_H
#define vobs_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobs.h,v 1.11 2005-11-29 10:29:48 gzins Exp $"
*
* History
* -------
* $Log: not supported by cvs2svn $
* Revision 1.10  2005/11/24 08:14:37  scetre
* Changed mother class of filter from vobsFILTER to vobsFILTER
*
* Revision 1.9  2005/11/15 14:57:56  scetre
* Added new scenario structure
* Added possibility to query merand and borde as primary catalog
*
* Revision 1.8  2005/06/20 11:31:53  scetre
* Added filter class
*
* Revision 1.7  2005/02/04 09:57:15  gzins
* Added vobsCATALOG_MIDI header file
*
* Revision 1.6  2005/01/26 08:10:32  scetre
* change history
*
* scetre    15-Jul-2004  Created
*
*
*******************************************************************************/

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif


#include "vobsErrors.h"
#include "vobsSTAR.h"
#include "vobsSTAR_LIST.h"
#include "vobsCATALOG.h"
#include "vobsCDATA.h"
#include "vobsPARSER.h"
#include "vobsREQUEST.h"
#include "vobsVIRTUAL_OBSERVATORY.h"
#include "vobsCATALOG_ASCC.h"
#include "vobsCATALOG_BSC.h"
#include "vobsCATALOG_CHARM.h"
#include "vobsCATALOG_CIO.h"
#include "vobsCATALOG_DENIS.h"
#include "vobsCATALOG.h"
#include "vobsCATALOG_HIC.h"
#include "vobsCATALOG_LBSI.h"
#include "vobsCATALOG_MASS.h"
#include "vobsCATALOG_PHOTO.h"
#include "vobsCATALOG_SBSC.h"
#include "vobsCATALOG_MIDI.h"
#include "vobsCATALOG_MIDI.h"
#include "vobsCATALOG_LIST.h"
#include "vobsSCENARIO.h"
#include "vobsFILTER.h"
#include "vobsGENERIC_FILTER.h"
#include "vobsFILTER_LIST.h"
#include "vobsMAGNITUDE_FILTER.h"
#include "vobsDISTANCE_FILTER.h"
#include "vobsVARIABILITY_FILTER.h"
#include "vobsMULTIPLICITY_FILTER.h"
#include "vobsLUMINOSITY_FILTER.h"
#include "vobsSPECTRAL_TYPE_FILTER.h"
#include "vobsORIGIN_FILTER.h"
#include "vobsBV_FILTER.h"

#endif /*!vobs_H*/


/*___oOo___*/
