#*******************************************************************************
# JMMC project
#
# "@(#) $Id: Makefile,v 1.12 2005-12-16 15:35:52 mella Exp $"
#
# Makefile of cmd
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.11  2005/12/14 12:41:32  mella
# Add CdfToTestScript
#
# Revision 1.10  2005/06/01 05:50:22  mella
# Add cmdCdfToCB.xsl for install
#
# Revision 1.9  2005/05/31 14:57:36  mella
# cmdCdfToCppCB.sh added
#
# Revision 1.8  2005/02/15 11:04:33  gzins
# Removed unused target
#
# Revision 1.7  2005/02/15 10:58:58  gzins
# Added CVS log as file modification history
#
# gzins     17-Jun-2004  Created
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
# <brief description of xxxxx program>
xxxxx_OBJECTS   =
xxxxx_LDFLAGS   =
xxxxx_LIBS      =

#
# special compilation flags for single c sources
#yyyyy_CFLAGS   = 

#
# Includes (.h) files (public only)
# ---------------------------------
INCLUDES        = cmd.h cmdCOMMAND.h cmdPARAM.h

#
# Libraries (public and local)
# ----------------------------
LIBRARIES       = cmd
LIBRARIES_L     =

#
# <brief description of lllll library>
cmd_OBJECTS   = cmdCOMMAND cmdPARAM

#
# Scripts (public and local)
# --------------------------
SCRIPTS         = cmdCdfToCppClass \
				  cmdIsCdfValid \
				  cmdCdfToTestScript \
				  cmdCdfToCppCB
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
# Python scripts (public and local)
# ---------------------------------
PY_SCRIPTS     = cmdBatch
PY_SCRIPTS_L   =


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
INSTALL_FILES = ../config/cmdDefinitionFile.xsd 	\
				../config/cmdCdfToCppClass.xsl		\
				../config/cmdCdfToTestScript.xsl		\
				../config/cmdCdfToCppCB.xsl

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
