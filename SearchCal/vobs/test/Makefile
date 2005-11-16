#*******************************************************************************
# JMMC project
#
# "@(#) $Id: Makefile,v 1.19 2005-11-16 10:45:14 scetre Exp $"
#
# Makefile for test of vobs module 
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.18  2005/11/15 15:02:07  scetre
# Added catalog list test
#
# Revision 1.17  2005/02/15 15:20:21  gzins
# Removed unused target
#
# Revision 1.16  2005/02/13 09:07:50  gzins
# Renamed vobsTestVirtualObservtary... to vobsTestVOIn...
#
# Revision 1.15  2005/02/13 08:54:00  gzins
# Updated after vobs classes changes
# Added CVS log as modifification history
#
# Revision 1.14  2005/02/04 10:06:40  gzins
# Added test for vobsCATALOG_MIDI
#
# scetre    16-Jun-2004  created
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
USER_INC =
#USER_LIB = 

#
# MODULE CODE DESCRIPTION:
# ------------------------
# As a general rule:  public file are "cleaned" and "installed"  
#                     local (_L) are not "installed".

#
# C programs (public and local)
# -----------------------------
EXECUTABLES     = vobsTestParser vobsTestRequest 	\
				  vobsTestCriteria vobsTestStar 	\
				  vobsTestStarList 					\
				  vobsTestMidiCatalog				\
				  vobsTestCatalogList
EXECUTABLES_L   = 

#
# List of test programs 
vobsTestParser_OBJECTS      = vobsTestParser 
vobsTestParser_LDFLAGS      = 
vobsTestParser_LIBS         = MCS C++ vobs

vobsTestRequest_OBJECTS     = vobsTestRequest 
vobsTestRequest_LDFLAGS     = 
vobsTestRequest_LIBS        = MCS C++ vobs

vobsTestCriteria_OBJECTS    = vobsTestCriteria 
vobsTestCriteria_LDFLAGS    = 
vobsTestCriteria_LIBS       = MCS C++ vobs

vobsTestStar_OBJECTS        = vobsTestStar
vobsTestStar_LDFLAGS        = 
vobsTestStar_LIBS           = MCS C++ vobs

vobsTestStarList_OBJECTS    = vobsTestStarList
vobsTestStarList_LDFLAGS    = 
vobsTestStarList_LIBS       = MCS C++ vobs

vobsTestMidiCatalog_OBJECTS = vobsTestMidiCatalog 
vobsTestMidiCatalog_LDFLAGS = 
vobsTestMidiCatalog_LIBS    = MCS C++ vobs

vobsTestCatalogList_OBJECTS = vobsTestCatalogList 
vobsTestCatalogList_LDFLAGS = 
vobsTestCatalogList_LIBS    = MCS C++ vobs
#
# special compilation flags for single c sources
#yyyyy_CFLAGS   = 

#
# Includes (.h) files (public only)
# ---------------------------------
INCLUDES        =

#
# Libraries (public and local)
# ----------------------------
LIBRARIES       = 
LIBRARIES_L     =

#
# <brief description of vobs library>
vobs_OBJECTS   =   

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

man   : do_man 
	@echo " . . . man page(s) done"

install : install_all
	@echo " . . . installation done"

#___oOo___
