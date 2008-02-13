#*******************************************************************************
# JMMC project
#
# "@(#) $Id: Makefile,v 1.22 2008-02-13 08:07:44 mella Exp $"
#
# Makefile of msg
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.21  2005/02/15 11:55:05  gzins
# Removed unused target
#
# Revision 1.20  2005/02/09 16:42:26  lafrasse
# Added msgMESSAGE_FILTER class to manage message queues
#
# Revision 1.19  2005/02/04 15:57:06  lafrasse
# Massive documentation review an refinment (also added automatic CVS log inclusion in every files)
#
# Revision 1.18  2005/01/29 20:05:27  gzins
# Added PROCLIST command
#
# gzins     14-Dec-2004  Added CDF and class for DEBUG command
# gzins     08-Dec-2004  Removed msgMCS_ENVS and added env library 
# gzins     07-Dec-2004  Updated after C++ conversion 
# gzins     06-Dec-2004  Renamed msgMCS_ENV to msgMCS_ENVS
# lafrasse  03-Dec-2004  Added msgMCS_ENV compilation directives
# lafrasse  23-Nov-2004  Added all the socket-related header
# lafrasse  19-Nov-2004  Added msgMANAGER_IF.cpp and msgMESSAGE.cpp compilation
#                        directives
# lafrasse  10-Aug-2004  Created
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
EXECUTABLES     = msgManager msgSendCommand
EXECUTABLES_L   = 

#
# <brief description of msgManager program>
msgManager_OBJECTS           = msgManager        \
							   msgPROCESS        \
							   msgPROCESS_LIST   \
							   msgMANAGER
msgManager_LDFLAGS           = $(MCSSTD_LIBLIST)
msgManager_LIBS              = mcs log err cmd misc env msg C++

msgSendCommand_OBJECTS       = msgSendCommand
msgSendCommand_LDFLAGS       = $(MCSSTD_LIBLIST)
msgSendCommand_LIBS          = mcs log err cmd misc env msg C++

#
# special compilation flags for single c sources
#yyyyy_CFLAGS   = 

#
# Includes (.h) files (public only)
# ---------------------------------
INCLUDES        = msg.h                \
				  msgMESSAGE.h         \
				  msgMESSAGE_FILTER.h  \
				  msgMANAGER_IF.h      \
				  msgSOCKET.h          \
				  msgDEBUG_CMD.h       \
				  msgSOCKET_CLIENT.h   \
				  msgSOCKET_SERVER.h

#
# Libraries (public and local)
# ----------------------------
LIBRARIES       = msg
LIBRARIES_L     =

#
# <brief description of msg library>
msg_OBJECTS   = msgMANAGER_IF      \
				msgMESSAGE         \
				msgMESSAGE_FILTER  \
				msgSOCKET          \
				msgDEBUG_CMD       \
				msgPROCLIST_CMD    \
				msgSOCKET_CLIENT   \
				msgSOCKET_SERVER

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
# Command Definition Files (CDF) 
#----------------------------
CDF_FILES       = msgDEBUG msgPROCLIST

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
