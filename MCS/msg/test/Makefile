#*******************************************************************************
# JMMC project
#
# "@(#) $Id: Makefile,v 1.9 2005-02-09 16:42:26 lafrasse Exp $"
#
# Makefile of msgTest
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.8  2005/02/04 15:57:06  lafrasse
# Massive documentation review an refinment (also added automatic CVS log inclusion in every files)
#
# gzins     08-Dec-2004  Removed msgTestClient and msgTestServer
#
# lafrasse  20-Nov-2004  Added C++ attributes to each executable LIBS to become
#                        compatible with the new C++ code in the library
# gzins     01-Oct-2004  Removed msgTestManagerSend
# lafrasse  11-Aug-2004  Created
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
EXECUTABLES     = 
EXECUTABLES_L   = msgTestSocketClient   \
                  msgTestSocketServer   \
                  msgTestManagerIFQueue

#
# <brief description of msgTestSocketClient program>
msgTestSocketClient_OBJECTS   = msgTestSocketClient
msgTestSocketClient_LDFLAGS   = -lgdome
msgTestSocketClient_LIBS      = mcs err log env cmd misc msg C++

#
# <brief description of msgTestSocketServer program>
msgTestSocketServer_OBJECTS   = msgTestSocketServer
msgTestSocketServer_LDFLAGS   = -lgdome
msgTestSocketServer_LIBS      = mcs err log env cmd misc msg C++

#
# <brief description of msgTestManagerIFQueue program>
msgTestManagerIFQueue_OBJECTS   = msgTestManagerIFQueue
msgTestManagerIFQueue_LDFLAGS   = -lgdome
msgTestManagerIFQueue_LIBS      = mcs err log cmd misc env msg C++

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

clean_dist : clean_all clean_dist_all 
	@echo " . . . clean_dist done"

man   : do_man 
	@echo " . . . man page(s) done"

install : install_all
	@echo " . . . installation done"

db : db_all
	@echo " . . . ../DB done"


#___oOo___
