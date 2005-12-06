#*******************************************************************************
# Jean-Marie Mariotti Center
#
# "@(#) $Id: Makefile,v 1.15 2005-12-06 07:08:33 gzins Exp $"
#
# Makefile of mcscfg module, which contains configuration files for MCS
# environment.
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.14  2005/12/02 12:21:28  mella
# mcsEnvList is copied as other files
#
# Revision 1.13  2005/12/02 12:19:15  mella
# Add mcscfgGenerateEnvList script
#
# Revision 1.12  2005/12/02 10:25:36  gzins
# Updated for new MCS directory structure; installed files in $MCSTOP/etc
#
# Revision 1.11  2005/03/03 10:07:26  gildas
# Fixed bug in install target
#
# Revision 1.10  2005/02/28 16:09:12  lafrasse
# Added 'mcsEnvFile'
#
# Revision 1.9  2005/02/16 09:26:56  gzins
# Added CVS log as modification file history
#
# gzins     25-Jun-2004  Created
# gzins     10-Dec-2004  Added ../config/emacs
#
#------------------------------------------------------------------------

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
SCRIPTS_L       = mcscfgGenerateEnvList

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
# Command Definition Files (CDF)
# ---------------------------------
CDF_FILES       =

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

MCSENVLIST = ../config/mcscfgEnvList

#
# TARGETS
# -------
all:	do_all
	@echo "== Generating: $(MCSENVLIST)"
	@../bin/mcscfgGenerateEnvList ../config/mcscfgEnvList.xml $(MCSENVLIST)
	@echo " . . . 'all' done" 

clean : clean_all 
	@$(RM) $(MCSENVLIST)
	@echo " . . . clean done"

man   : do_man 
	@echo " . . . man page(s) done"

install : 
	@echo "Installing into: $(MCSDATA)ma";
	@$(foreach file, $(MCSENVLIST), echo "        `basename $(file)`" ;\
		chmod 644 $(file) ; \
		cp -p -f $(file) $(MCSDATA);)
	@echo " . . . installation done"


#
# Configuration files to be installed
#----------------------------


#CFG_FILES = ../config/mcsEnvList
#
# TARGETS
# -------
oldall:
	@./mcscfgGenerateEnvList.sh ../config/mcsEnvList.xml ../config/mcsEnvList
	@echo " . . . 'all' done"

oldclean :
	rm ../config/mcsEnvList
	@echo " . . . clean done"


#___oOo___
