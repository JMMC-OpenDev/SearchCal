#*******************************************************************************
# JMMC project
#
# "@(#) $Id: Makefile,v 1.11 2004-12-20 09:40:24 scetre Exp $"
#
# Makefile of vobs module 
#
# who       when		 what
# --------  -----------	 -------------------------------------------------------
# scetre    16-Jun-2004  created
#
#
#*******************************************************************************
# REMARKS
#    Makefile based on the VLT Makefile
#
#-------------------------------------------------------------------------------

#
# user definable C-compilation flags
#USER_CFLAGS = 

#
# additional include and library search paths
USER_INC = -I/usr/include/libxml2/ -I/usr/include/libgdome -I/usr/lib/glib-2.0/include/ -I/usr/include/glib-2.0
#USER_LIB = 

#
# MODULE CODE DESCRIPTION:
# ------------------------
# As a general rule:  public file are "cleaned" and "installed"  
#                     local (_L) are not "installed".

#
# C programs (public and local)
# -----------------------------
EXECUTABLES     =  
EXECUTABLES_L   = 

#
# <brief description of vobsServer program>
vobsServer_OBJECTS   = vobsSERVER 
vobsServer_LDFLAGS   = 
vobsServer_LIBS      = MCS C++ vobs

#
# special compilation flags for single c sources
#yyyyy_CFLAGS   = 

#
# Includes (.h) files (public only)
# ---------------------------------
INCLUDES        = vobs.h						\
				  vobsSTAR.h 			   		\
				  vobsSTAR_PROPERTY.h 			\
				  vobsSTAR_LIST.h 		   		\
				  vobsREQUEST.h 		   		\
				  vobsCDATA.h			   		\
				  vobsPARSER.h			   		\
				  vobsCATALOG.h 		   		\
				  vobsCATALOG_ASCC.h 	   		\
				  vobsCATALOG_BSC.h 	   		\
				  vobsCATALOG_CHARM.h 	   		\
				  vobsCATALOG_CIO.h 	   		\
				  vobsCATALOG_DENIS.h 	   		\
				  vobsCATALOG_HIC.h 	   		\
				  vobsCATALOG_LBSI.h 	   		\
				  vobsCATALOG_MASS.h 	   		\
				  vobsCATALOG_PHOTO.h 	   		\
				  vobsCATALOG_SBSC.h	   		\
				  vobsVIRTUAL_OBSERVATORY.h		\
				  vobsSCENARIO.h	       		\
				  vobsSTAR_COMP_CRITERIA_LIST.h	\
#
# Libraries (public and local)
# ----------------------------
LIBRARIES       = vobs
LIBRARIES_L     =

#
# <brief description of vobs library>
vobs_OBJECTS   =   vobsSTAR 					\
				   vobsSTAR_PROPERTY			\
				   vobsSTAR_LIST 				\
				   vobsREQUEST 					\
				   vobsCDATA					\
				   vobsPARSER					\
				   vobsCATALOG 					\
				   vobsCATALOG_ASCC 			\
				   vobsCATALOG_BSC 				\
				   vobsCATALOG_CHARM 			\
				   vobsCATALOG_CIO 				\
				   vobsCATALOG_DENIS 			\
				   vobsCATALOG_HIC 				\
				   vobsCATALOG_LBSI 			\
				   vobsCATALOG_MASS 			\
				   vobsCATALOG_PHOTO 			\
				   vobsCATALOG_SBSC				\
				   vobsVIRTUAL_OBSERVATORY		\
				   vobsSCENARIO             	\
				   vobsSTAR_COMP_CRITERIA_LIST	\
#
# Scripts (public and local)
# --------------------------
SCRIPTS         =
SCRIPTS_L       =

#
# TCL scripts (public and local)
# ------------------------------
TCL_SCRIPTS     =
TCL_SCRIPTS_L   =

#
# <brief description of tttttt tcl-script>
tttttt_OBJECTS  =
tttttt_TCLSH    = 
tttttt_LIBS     = 

#
# TCL libraries (public and local)
# --------------------------------
TCL_LIBRARIES   =
TCL_LIBRARIES_L =

#
# <brief description of tttlll library>
tttlll_OBJECTS  = 



#
# man pages to be done
# --------------------
MANSECTIONS =
MAN1 =
MAN3 =
MAN5 =
MAN7 =
MAN8 =

#
# other files to be installed
#----------------------------
INSTALL_FILES =

#
# list of all possible C-sources (used to create automatic dependencies)
# ----------------------------------------------------------------------
CSOURCENAMES = \
	$(foreach exe, $(EXECUTABLES) $(EXECUTABLES_L), $($(exe)_OBJECTS)) \
	$(foreach lib, $(LIBRARIES) $(LIBRARIES_L), $($(lib)_OBJECTS))

#
#>>>>> END OF standard rules

#
# CHOSE PLATFORM
# --------------
# Default is UNIX, for VxVorks application next line MUST BE UNCOMMENTED
#MAKE_VXWORKS = on   

#
# INCLUDE STANDARDS
# -----------------
MAKEDIR = $(MCSROOT)/include
include $(MAKEDIR)/mkfMakefile

#
# TARGETS
# -------
all:	do_all
	@echo " . . . 'all' done" 

clean : clean_all 
	@echo " . . . clean done"

clean_dist : clean_all clean_dist_all 
	@echo " . . . clean_dist done"

man   : do_man 
	@echo " . . . man page(s) done"

install : install_all
	@echo " . . . installation done"

db : db_all
	@echo " . . . ../DB done"


#___oOo___
