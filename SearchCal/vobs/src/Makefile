#*******************************************************************************
# JMMC project
#
# "@(#) $Id: Makefile,v 1.17 2005-06-13 10:21:22 scetre Exp $"
#
# Makefile of vobs module 
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.16  2005/02/15 15:20:21  gzins
# Removed unused target
#
# Revision 1.15  2005/02/11 14:10:43  gluck
# Added vobsLOCAL_CATALOG and vobsREMOTE_CATALOG classes
#
# Revision 1.14  2005/02/04 10:49:23  gzins
# Added installation of MIDI catalog
#
# Revision 1.13  2005/02/04 09:59:37  gzins
# Added vobsCATALOG_MIDI class
#
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
				  vobsREMOTE_CATALOG.h 			\
				  vobsLOCAL_CATALOG.h 			\
				  vobsCATALOG_ASCC.h 	   		\
				  vobsCATALOG_BSC.h 	   		\
				  vobsCATALOG_CHARM.h 	   		\
				  vobsCATALOG_CHARM2.h 	   		\
				  vobsCATALOG_CIO.h 	   		\
				  vobsCATALOG_DENIS.h 	   		\
				  vobsCATALOG_DENIS_JK.h   		\
				  vobsCATALOG_HIC.h 	   		\
				  vobsCATALOG_LBSI.h 	   		\
				  vobsCATALOG_MERAND.h 	   		\
				  vobsCATALOG_MASS.h 	   		\
				  vobsCATALOG_PHOTO.h 	   		\
				  vobsCATALOG_SBSC.h	   		\
				  vobsCATALOG_MIDI.h			\
				  vobsVIRTUAL_OBSERVATORY.h		\
				  vobsSCENARIO_ENTRY.h			\
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
				   vobsREMOTE_CATALOG			\
				   vobsLOCAL_CATALOG			\
				   vobsCATALOG_ASCC 			\
				   vobsCATALOG_BSC 				\
				   vobsCATALOG_CHARM 			\
				   vobsCATALOG_CHARM2 			\
				   vobsCATALOG_CIO 				\
				   vobsCATALOG_DENIS 			\
				   vobsCATALOG_DENIS_JK			\
				   vobsCATALOG_HIC 				\
				   vobsCATALOG_LBSI 			\
				   vobsCATALOG_MERAND 			\
				   vobsCATALOG_MASS 			\
				   vobsCATALOG_PHOTO 			\
				   vobsCATALOG_SBSC				\
				   vobsCATALOG_MIDI 			\
				   vobsVIRTUAL_OBSERVATORY		\
				   vobsSCENARIO_ENTRY			\
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
INSTALL_FILES = ../config/vobsMidiCatalog.cfg

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

man   : do_man 
	@echo " . . . man page(s) done"

install : install_all
	@echo " . . . installation done"

#___oOo___
