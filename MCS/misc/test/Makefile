#*******************************************************************************
# JMMC project
#
# "@(#) $Id: Makefile,v 1.11 2005-02-15 10:26:22 gzins Exp $"
#
# Makefile of misc Tests
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.10  2005/02/15 09:44:37  gzins
# Added CVS log as file modification history
#
# gzins     16-Jun-2004  created
# lafrasse  23-Jun-2004  added miscTestDate.c
#                        added miscTestString.c
#                        added miscTestFile.c
#                        removed miscTestUtils.c
#                        added miscTestDynBuf.c
# lafrasse  13-Jul-2004  added miscTestDynStr.c
# lafrasse  02-Aug-2004  Removed miscTestDynStr compilation, due to
#                        null-terminated string specific functions move from
#                        miscDynStr.h to miscDynBuf.h
# lafrasse  03-Aug-2004  Added miscTestNetwork.c
#
#*******************************************************************************
# REMARKS
#    Makefile based on the VLT Makefile
#
#-------------------------------------------------------------------------------

#
# user definable C-compilation flags
#USER_CFLAGS = 
#MAKE_PURIFY = ON
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
EXECUTABLES     = miscTestDate     \
				  miscTestDynBuf   \
				  miscTestFile     \
				  miscTestNetwork  \
				  miscTestString   \
				  miscTestHash

EXECUTABLES_L   = 

#
# <brief description of xxxxx program>
miscTestDate_OBJECTS   = miscTestDate
miscTestDate_LDFLAGS   = -lgdome
miscTestDate_LIBS      = mcs err log misc

#
# <brief description of xxxxx program>
miscTestDynBuf_OBJECTS   = miscTestDynBuf
miscTestDynBuf_LDFLAGS   = -lgdome
miscTestDynBuf_LIBS      = mcs err log misc

#
# <brief description of xxxxx program>
miscTestFile_OBJECTS   = miscTestFile
miscTestFile_LDFLAGS   = -lgdome
miscTestFile_LIBS      = mcs err log misc

#
# <brief description of xxxxx program>
miscTestNetwork_OBJECTS   = miscTestNetwork
miscTestNetwork_LDFLAGS   = -lgdome
miscTestNetwork_LIBS      = mcs err log misc

#
# <brief description of xxxxx program>
miscTestString_OBJECTS   = miscTestString
miscTestString_LDFLAGS   = -lgdome
miscTestString_LIBS      = mcs err log misc

#
# <brief description of xxxxx program>
miscTestHash_OBJECTS   = miscTestHash
miscTestHash_LDFLAGS   = -lgdome
miscTestHash_LIBS      = mcs err log misc

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
# <brief description of lllll library>
lllll_OBJECTS   =

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
