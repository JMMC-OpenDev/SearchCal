#*******************************************************************************
# JMMC project
#
# "@(#) $Id: Makefile,v 1.7 2005-01-29 15:17:02 gzins Exp $"
#
# Makefile of evh library
#
# History 
# -------
# $Log: not supported by cvs2svn $
# Revision 1.6  2005/01/26 18:25:00  gzins
# Added files related to state/sub-state handling
#
# sccmgr    22-Sep-2004  Created
# gzins     17-Nov-2004  Added evhTASK and evhSERVER class
# gzins     22-Dec-2004  Added evhVERSION, evhDEBUG and evhVERSION CDF
#                        Added evhHelpCB
# gzins     07-Jan-2005  Added evhCMD_REPLY_KEY and evhINTERFACE 
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
EXECUTABLES     = 
EXECUTABLES_L   = 

#
# special compilation flags for single c sources
#yyyyy_CFLAGS   = 

#
# Includes (.h) files (public only)
# ---------------------------------
INCLUDES        =  evhCALLBACK.h          \
				   evhCALLBACK_LIST.h     \
				   evhCMD_CALLBACK.h      \
				   evhIOSTREAM_CALLBACK.h \
				   evhKEY.h               \
				   evhCMD_KEY.h           \
				   evhCMD_REPLY_KEY.h     \
				   evhIOSTREAM_KEY.h      \
				   evhINTERFACE.h         \
				   evhErrors.h            \
				   evh.h                  \
				   evhHANDLER.h           \
				   evhSERVER.h            \
				   evhStates.h            \
				   evhTASK.h


#
# Libraries (public and local)
# ----------------------------
LIBRARIES       = evh
LIBRARIES_L     =

#
# <brief description of lllll library>
evh_OBJECTS   = evhHANDLER           \
				evhCALLBACK          \
				evhCMD_CALLBACK      \
				evhIOSTREAM_CALLBACK \
				evhCALLBACK_LIST     \
				evhKEY               \
				evhCMD_KEY           \
				evhCMD_REPLY_KEY     \
				evhTASK              \
				evhINTERFACE         \
				evhSERVER            \
				evhIOSTREAM_KEY      \
				evhHELP_CMD          \
				evhSTATE_CMD         \
				evhVERSION_CMD       \
				evhDEBUG_CMD         \
				evhStateCB           \
				evhDebugCB           \
				evhHelpCB            \
				evhVersionCB
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
# Python scripts (public and local)
# ---------------------------------
PY_SCRIPTS     =
PY_SCRIPTS_L   =

#
# Jarfiles and their directories
# ---------------------------------
JARFILES        =

#
# <brief description of jjjfff jar files>
jjjfff_DIRS     =
jjjfff_EXTRAS   = 

#
# Java programs
# ---------------------------------
JAVA_EXECUTABLES=

#
# <brief description of jjjjjj java program>
jjjjjj_CLASS    =

#
# Command Definition Files
# ---------------------------------
CDF_FILES       = evhDEBUG    \
				  evhHELP     \
				  evhSTATE    \
				  evhVERSION

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

db : db_all
	@echo " . . . ../DB done"


#___oOo___
