#*******************************************************************************
# JMMC project
#
# "@(#) $Id: Makefile,v 1.10 2004-11-09 14:53:55 gzins Exp $"
#
# Makefile of misc Library
#
# who       when         what
# --------  -----------  -------------------------------------------------------
# lafrasse  24-Jun-2004  Created
# lafrasse  01-Jul-2004  Added miscDynBuf.h
# lafrasse  13-Jul-2004  Added miscDynStr.h
# lafrasse  02-Aug-2004  Removed miscDynStr compilation due to null-terminated
#                        string specific functions move from miscDynStr.h to
#                        miscDynBuf.h
# lafrasse  03-Aug-2004  Added miscNetwork stuff
# gzins     03-Aug-2004  Added installation of all include files 
# lafrasse  10-Aug-2004  Removed miscPrivate. installation
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
#USER_INC = 
#USER_LIB = 

#
# MODULE CODE DESCRIPTION:
# ------------------------
# As a general rule:  public file are "cleaned" and "installed"  
#                     local (_L) are not "installed".

#
# C programs (public and local)
# -----------------------------
EXECUTABLES     = miscLocateFile
EXECUTABLES_L   = 

#
# 
miscLocateFile_OBJECTS   = miscLocateFile
miscLocateFile_LDFLAGS   = -lgdome
miscLocateFile_LIBS      = mcs log err misc

#
# special compilation flags for single c sources
#yyyyy_CFLAGS   = 

#
# Includes (.h) files (public only)
# ---------------------------------
INCLUDES        = misc.h \
				  miscDate.h \
				  miscDynBuf.h \
				  miscFile.h \
				  miscNetwork.h \
				  miscString.h


#
# Libraries (public and local)
# ----------------------------
LIBRARIES       = misc 
LIBRARIES_L     =

#
# <brief description of lllll library>
misc_OBJECTS   = miscDate miscDynBuf miscFile miscNetwork miscString

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
MANSECTIONS = 1
MAN1 = miscLocateFile.c
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
