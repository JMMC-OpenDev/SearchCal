#*******************************************************************************
# JMMC project
#
# "@(#) $Id: Makefile,v 1.15 2005-02-13 08:54:00 gzins Exp $"
#
# Makefile for test of vobs module 
#
# History
# -------
# $Log: not supported by cvs2svn $
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
EXECUTABLES     = vobsTestVirtualObservatoryKBand 	\
				  vobsTestVirtualObservatoryVBand 	\
				  vobsTestParser vobsTestRequest 	\
				  vobsTestCriteria vobsTestStar 	\
				  vobsTestStarList 					\
				  vobsTestMidiCatalog
EXECUTABLES_L   = 

#
# List of test programs 
vobsTestVirtualObservatoryKBand_OBJECTS = vobsTestVirtualObservatoryKBand
vobsTestVirtualObservatoryKBand_LDFLAGS = 
vobsTestVirtualObservatoryKBand_LIBS    = MCS C++ vobs

vobsTestVirtualObservatoryVBand_OBJECTS = vobsTestVirtualObservatoryVBand
vobsTestVirtualObservatoryVBand_LDFLAGS = 
vobsTestVirtualObservatoryVBand_LIBS    = MCS C++ vobs

vobsTestParser_OBJECTS            		= vobsTestParser 
vobsTestParser_LDFLAGS            		= 
vobsTestParser_LIBS               		= MCS C++ vobs

vobsTestRequest_OBJECTS           		= vobsTestRequest 
vobsTestRequest_LDFLAGS           		= 
vobsTestRequest_LIBS              		= MCS C++ vobs

vobsTestCriteria_OBJECTS          		= vobsTestCriteria 
vobsTestCriteria_LDFLAGS          		= 
vobsTestCriteria_LIBS             		= MCS C++ vobs

vobsTestStar_OBJECTS              		= vobsTestStar
vobsTestStar_LDFLAGS              		= 
vobsTestStar_LIBS                 		= MCS C++ vobs

vobsTestStarList_OBJECTS          		= vobsTestStarList
vobsTestStarList_LDFLAGS          		= 
vobsTestStarList_LIBS             		= MCS C++ vobs

vobsTestMidiCatalog_OBJECTS       		= vobsTestMidiCatalog 
vobsTestMidiCatalog_LDFLAGS       		= 
vobsTestMidiCatalog_LIBS          		= MCS C++ vobs
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

clean_dist : clean_all clean_dist_all 
	@echo " . . . clean_dist done"

man   : do_man 
	@echo " . . . man page(s) done"

install : install_all
	@echo " . . . installation done"

db : db_all
	@echo " . . . ../DB done"


#___oOo___
