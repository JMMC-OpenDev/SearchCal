#*******************************************************************************
# JMMC project
#
# "@(#) $Id: Makefile,v 1.12 2004-11-26 13:11:28 lafrasse Exp $"
#
# Makefile of msg
#
# who       when         what
# --------  -----------  -------------------------------------------------------
# lafrasse  10-Aug-2004  Created
# gzins     01-Oct-2004  Added msgSendCommand
# lafrasse  19-Nov-2004  Added msgMANAGER_IF.cpp and msgMESSAGE.cpp compilation
#                        directives
# lafrasse  23-Nov-2004  Added all the socket-related header
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
EXECUTABLES     = msgManager msgSendCommand
EXECUTABLES_L   = 

#
# <brief description of msgManager program>
msgManager_OBJECTS           = msgManager            \
                               msgManagerForward     \
                               msgManagerHandleCmd 	 \
                               msgManagerProcess     \
                               msgManagerProcessList
msgManager_LDFLAGS           = -lgdome
msgManager_LIBS              = mcs log err cmd misc msg C++

msgSendCommand_OBJECTS       = msgSendCommand
msgSendCommand_LDFLAGS       = -lgdome
msgSendCommand_LIBS          = mcs log err cmd misc msg C++

#
# special compilation flags for single c sources
#yyyyy_CFLAGS   = 

#
# Includes (.h) files (public only)
# ---------------------------------
INCLUDES        = msg.h              \
				  msgMESSAGE.h       \
				  msgMANAGER_IF.h    \
				  msgSOCKET.h        \
                  msgSOCKET_CLIENT.h \
				  msgSOCKET_SERVER.h

#
# Libraries (public and local)
# ----------------------------
LIBRARIES       = msg
LIBRARIES_L     =

#
# <brief description of msg library>
msg_OBJECTS   = msg                \
				msgConnect         \
				msgDisconnect      \
				msgReceive         \
				msgSend            \
				msgSocket          \
				msgMANAGER_IF      \
                msgMESSAGE         \
                msgSOCKET		   \
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
