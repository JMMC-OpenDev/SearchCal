#*******************************************************************************
# JMMC project
#
# "@(#) $Id: Makefile,v 1.9 2008-12-03 10:07:01 lafrasse Exp $"
#
# Makefile of sclcat module
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.8  2008/09/30 14:54:18  mella
# use g95 instead of ifc
#
# Revision 1.7  2008/07/23 22:35:44  lafrasse
# Added sclcatRetryFailedQueries to SCRIPTS_L section.
#
# Revision 1.6  2008/07/11 12:56:55  lafrasse
# Added ESO Calibrator catalog generation scripts.
#
# Revision 1.5  2007/03/27 14:55:42  scetre
# Remuved unused fortran program
#
# Revision 1.4  2007/01/31 10:03:05  mella
# Add new programs
#
# Revision 1.3  2006/11/27 10:31:19  scetre
# Added Fortrant treatment in the catalogue file generation
#
# Revision 1.2  2006/10/31 14:23:26  gzins
# Added sclcatPrima local script
#
# Revision 1.1.1.1  2006/10/31 09:56:24  sccmgr
# Creation
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
SCRIPTS_L       = sclcatRetryFailedQueries \
				  sclcatPrima \
				  sclcatPrimaGenerateConfig \
				  sclcatPrimaParseResult \
                  sclcatESO \
                  sclcatESOParseResult \
                  sclcatESOFilterResult \


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
PY_SCRIPTS_L   = sclcatPrimaPrepareBatchFile \
                 sclcatESOGenerateConfig


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

#
# TARGETS
# -------
all:	do_fortran do_all
	@echo " . . . 'all' done" 

clean : clean_fortran clean_all 
	@echo " . . . clean done"

man   : do_man 
	@echo " . . . man page(s) done"

install : install_all
	@echo " . . . installation done"

# Build fortran executables
EXECUTABLES_FORTRAN = sclcatPrimaFilter

do_fortran:
	@for file in $(EXECUTABLES_FORTRAN) ; do \
		echo "== FORTRAN Compiling: $${file}.f"; \
		echo "Building executable::  ../bin/$${file}"; \
		g95 -o ../bin/$${file} $${file}.f; \
	done

clean_fortran:
	@for file in $(EXECUTABLES_FORTRAN) ; do \
		$(RM) -f ../bin/$${file}; \
	done

#___oOo___
