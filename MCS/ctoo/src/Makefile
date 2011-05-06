#*******************************************************************************
# JMMC project
#
# "@(#) $Id: Makefile,v 1.21 2005-02-15 10:22:28 gzins Exp $"
#
# Makefile of ctoo module
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.20  2005/02/15 09:21:36  gzins
# Remove db_all target
#
# Revision 1.19  2005/01/24 15:47:51  gluck
# Bug correction for log message automatic insertion ($Log: not supported by cvs2svn $
# Bug correction for log message automatic insertion (Revision 1.20  2005/02/15 09:21:36  gzins
# Bug correction for log message automatic insertion (Remove db_all target
# Bug correction for log message automatic insertion ()
#
# gluck     18-May-2004 Created
# gzins     09-Jun-2004 Added templates for doxygen
# mella     24-Jun-2004 Add buildDocIndex related
# lafrasse  29-Jul-2004 Added ctooRename & ctooReplace (forked the VLT Soft)
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
SCRIPTS         = ctooGetTemplate \
				  ctooGetDirectoryStructure \
				  ctooGetTemplateForDirectoryStructure \
				  ctooGetCode \
				  ctooGetTemplateForCoding \
				  ctooGetTemplateFile \
				  ctooGetSpecificHeaderFile \
				  ctooGetModuleName \
                  ctooRename \
                  ctooReplace \
                  ctooCleanStandardHeader
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
MAN1 = $(addsuffix .sh, $(SCRIPTS))
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

# define basic paths to MCSROOT:
ifdef MCSROOT
    TEMPLATES  = $(MCSROOT)/templates
endif
# if INTROOT is defined, override or complete the search paths:
ifdef INTROOT
    TEMPLATES  = $(INTROOT)/templates
endif
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
	-$(AT) cp -r ../templates/* ${TEMPLATES}
	-$(AT) rm -f ${TEMPLATES}/*/*~
	-$(AT) rm -f ${TEMPLATES}/*/.*.sw*
	-$(AT) rm -rf ${TEMPLATES}/*/.svn
	@echo " . . . installation done"

#___oOo___
