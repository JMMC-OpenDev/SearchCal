#*******************************************************************************
# JMMC Project
#
# "@(#) $Id: Makefile,v 1.10 2005-03-07 13:30:36 mella Exp $"
#
# Makefile of log 
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.9  2005/02/15 10:23:44  gzins
# Removed unused target
#
# Revision 1.8  2005/02/15 08:27:12  gzins
# Added CVS log as file modification history
#
# mella     12-May-2004  Created
# lafrasse  03-Aug-2004  Added logManager and logPrivate stuff
#                        Removed cross-dependencies with err and misc
# 
#*******************************************************************************
# This Makefile follows VLT Standards (see Makefile(5) for more).
#*******************************************************************************
# REMARKS
#    None
#------------------------------------------------------------------------

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
EXECUTABLES     = logManager
EXECUTABLES_L   = 

#
# <brief description of xxxxx program>
logManager_OBJECTS   = logManager
logManager_LDFLAGS   = 
logManager_LIBS      = mcs log

#
# special compilation flags for single c sources
#yyyyy_CFLAGS   = 

#
# Includes (.h) files (public only)
# ---------------------------------
INCLUDES        = log.h

#
# Libraries (public and local)
# ----------------------------
LIBRARIES       = log
LIBRARIES_L     =

#
# <brief description of lllll library>
log_OBJECTS   = log logPrivate

#
# Scripts (public and local)
# ----------------------------
SCRIPTS         = logInit
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
# ------------------------------
TCL_LIBRARIES   =
TCL_LIBRARIES_L =

#
# <brief description of tttlll library>
tttlll_OBJECTS  = 

#
# man pages to be done
# --------------------
MANSECTIONS = 1
MAN1 = $(addsuffix .c, $(EXECUTABLES))
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
# ------------------------------
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
