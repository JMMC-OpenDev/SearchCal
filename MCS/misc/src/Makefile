#*******************************************************************************
# JMMC project
#
# "@(#) $Id: Makefile,v 1.16 2008-02-13 07:17:18 mella Exp $"
#
# Makefile of misc Library
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.15  2005/10/10 12:00:38  lafrasse
# Added miscLocateDir
#
# Revision 1.14  2005/02/15 10:26:22  gzins
# Removed unused target
#
# Revision 1.13  2005/02/15 09:40:34  gzins
# Added CVS log as file modification history
#
# lafrasse  24-Jun-2004  Created
# lafrasse  01-Jul-2004  Added miscDynBuf.h
# lafrasse  13-Jul-2004  Added miscDynStr.h
# lafrasse  02-Aug-2004  Removed miscDynStr compilation due to null-terminated
#                        string specific functions move from miscDynStr.h to
#                        miscDynBuf.h
# lafrasse  03-Aug-2004  Added miscNetwork stuff
# gzins     03-Aug-2004  Added installation of all include files 
# lafrasse  10-Aug-2004  Removed miscPrivate. installation
# gzins     09-Nov-2004  Added miscLocateFile compilation directives 
# gzins     17-Dec-2004  Added miscHash
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
EXECUTABLES     = miscLocateFile miscLocateDir
EXECUTABLES_L   = 

#
# 
miscLocateFile_OBJECTS   = miscLocateFile
miscLocateFile_LDFLAGS   = $(MCSSTD_LIBLIST)
miscLocateFile_LIBS      = mcs log err misc

#
# 
miscLocateDir_OBJECTS   = miscLocateDir
miscLocateDir_LDFLAGS   = $(MCSSTD_LIBLIST)
miscLocateDir_LIBS      = mcs log err misc

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
