#*******************************************************************************
# JMMC project
#
# "@(#) $Id: Makefile,v 1.4 2005-02-15 10:10:02 gzins Exp $"
#
# Makefile of mkf test programs 
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.3  2005/02/15 09:18:22  gzins
# Minor chages
#
# Revision 1.2  2005/02/15 09:12:26  gzins
# Added CVS log as file modification history
#
# gzins     09-Sep-2004  Created
#
#*******************************************************************************
# REMARKS
#    Makefile based on the VLT/ALMA Makefile
#
#-------------------------------------------------------------------------------

#
# user definable C-compilation flags
#USER_CFLAGS = 

#
# user definable javac compilation flags
#USER_JFLAGS = 

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
EXECUTABLES     = mkfTestMain
EXECUTABLES_L   = 

#
# <brief description of xxxxx program>
mkfTestMain_OBJECTS   = mkfTestMain
mkfTestMain_LDFLAGS   =
mkfTestMain_LIBS      =

#
# special compilation flags for single c sources
#yyyyy_CFLAGS   = 

#
# Includes (.h) files (public only)
# ---------------------------------
INCLUDES        = mkfMakefile.all

#
# Libraries (public and local)
# ----------------------------
LIBRARIES       = mkfTest
LIBRARIES_L     =

#
# <brief description of lllll library>
mkfTest_OBJECTS = mkfTest

#
# Scripts (public and local)
# --------------------------
SCRIPTS         = mkfTest
SCRIPTS_L       =

#
# TCL scripts (public and local)
# ------------------------------
TCL_SCRIPTS     = mkfTestTcl
TCL_SCRIPTS_L   =

#
# <brief description of tttttt tcl-script>
mkfTestTcl_OBJECTS  = mkfTestTcl
mkfTestTcl_TCLSH    = tclsh 
mkfTestTcl_LIBS     = mkfTestTclLib

#
# TCL libraries (public and local)
# --------------------------------
TCL_LIBRARIES   = mkfTestTclLib
TCL_LIBRARIES_L =

#
# <brief description of tttlll library>
mkfTestTclLib_OBJECTS  = mkfTestTclLib



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
# local man pages
# ---------------
MANl =

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
