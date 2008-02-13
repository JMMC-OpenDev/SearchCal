#*******************************************************************************
# JMMC project
#
# "@(#) $Id: Makefile,v 1.6 2008-02-13 08:00:03 mella Exp $"
#
# Makefile of ........
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.5  2006/03/31 13:19:55  gzins
# Added envGet program
#
# Revision 1.4  2005/02/13 16:53:13  gzins
# Added CVS log as modification history
#
# lafrasse  07-Dec-2004  Created
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
EXECUTABLES     = envShow    \
				  envGet
EXECUTABLES_L   = 

#
# envShow displays the list of environments 
envShow_OBJECTS   = envShow
envShow_LDFLAGS   = $(MCSSTD_LIBLIST)
envShow_LIBS      = mcs log err misc env C++

#
# envGet displays information related to the given environment
envGet_OBJECTS   = envGet
envGet_LDFLAGS   = $(MCSSTD_LIBLIST)
envGet_LIBS      = mcs log err misc env C++

#
# special compilation flags for single c sources
#yyyyy_CFLAGS   = 

#
# Includes (.h) files (public only)
# ---------------------------------
INCLUDES        = env.h     \
                  envLIST.h

#
# Libraries (public and local)
# ----------------------------
LIBRARIES       = env
LIBRARIES_L     =

#
# <brief description of lllll library>
env_OBJECTS   = envLIST

#
# Scripts (public and local)
# --------------------------
SCRIPTS         = envCheck envStart envStop
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

#___oOo___
