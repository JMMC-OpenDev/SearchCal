#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************
# REMARKS
#    Makefile based on the VLT Makefile
#
#-------------------------------------------------------------------------------


#
# user definable C-compilation flags
USER_CFLAGS = -D_GNU_SOURCE

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
EXECUTABLES     = miscLocateFile miscLocateDir
EXECUTABLES_L   = 

#
# 
miscLocateFile_OBJECTS   = miscLocateFile
miscLocateFile_LDFLAGS   = $(MCSSTD_LIBLIST)
miscLocateFile_LIBS      = misc err log mcs

#
# 
miscLocateDir_OBJECTS   = miscLocateDir
miscLocateDir_LDFLAGS   = $(MCSSTD_LIBLIST)
miscLocateDir_LIBS      = misc err log mcs

#
# special compilation flags for single c sources
#yyyyy_CFLAGS   = 

#
# Includes (.h) files (public only)
# ---------------------------------
INCLUDES        = misc.h        \
				  miscDate.h    \
				  miscDynBuf.h  \
				  miscFile.h    \
				  miscNetwork.h \
				  miscString.h  \
				  miscHash.h


#
# Libraries (public and local)
# ----------------------------
LIBRARIES       = misc 
LIBRARIES_L     =

#
# <brief description of lllll library>
misc_OBJECTS   = miscDate    \
				 miscDynBuf  \
				 miscFile    \
				 miscNetwork \
				 miscString  \
				 miscHash

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
MAN1 = miscLocateFile.c miscLocateDir.c
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
