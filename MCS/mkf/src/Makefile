#*******************************************************************************
# JMMC project
#
# "@(#) $Id: Makefile,v 1.12 2005-02-15 10:09:35 gzins Exp $"
#
# Makefile for MCS customization of make
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.11  2005/02/15 08:40:15  gzins
# Added CVS log as file modification history
#
# sccmgr    25-Jun-2004  Created
# gzins     26-Aug-2004  Adapted from VLT
# gzins     03-Dec-2004  Added mkfMakeInstallCmdDefFiles script
# gzins     04-Dec-2004  Added mkfUpdateMcs script
# gzins     04-Dec-2004  Changed mkfUpdateMcs to mkfInstallMcs
# gzins     08-Dec-2004  Moved mkfInstallMcs to mcsins
# gzins     09-Dec-2004  Removed mkfMakeErrorFileDependencies and added
#                        mkfMakeGeneratedFilesDependencies
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
# Scripts (public and local)
# --------------------------
SCRIPTS         = mkfBuildDocIndex \
                  mkfCreateInstallRootDirs \
                  mkfDoxygen \
                  mkfDoManPages \
                  mkfMakeCopySources \
				  mkfMakeGeneratedFilesDependencies \
                  mkfMakeJavaClasspath \
                  mkfMakeExecutableDependencies \
                  mkfMakeJavaExecutable \
                  mkfMakeJavaDependencies \
                  mkfMakeJarDependencies \
                  mkfMakeLibraryDependencies \
                  mkfMakeInstallCmdDefFiles \
                  mkfMakeInstallDocFiles \
                  mkfMakeInstallErrorFiles \
                  mkfMakeInstallFiles \
                  mkfFilterForDoxygen \
                  mkfMakeTclLibDependencies \
                  mkfMakeTclLib \
                  mkfMakeTclScriptDependencies \
                  mkfMakeTclScript \
                  mkfMakeScriptDependencies \
                  mkfMakeScript
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
# local man pages
# ---------------
MANl =

#
# other files to be installed
#----------------------------
INSTALL_FILES = ../include/mkfJavaMakefile                  \
                ../include/mkfMakefile                      \
                ../include/mkfMakefile.all                  \
                ../include/mkfPythonMakefile                \
                ../config/mkfBuildDocIndex2.xsl             \
                ../config/mkfBuildDocIndex.xsl              \
                ../config/mkfSTKToInterface.xsl             \
                ../config/mkfSTKToPythonHelp.xsl            \
                ../config/mkfSTKToPythonTestForCpp.xsl      \
                ../config/mkfSTKToPythonTestForC.xsl        \
                ../config/mkfSTK_WriteArrayName.xsl         \
                ../config/mkfSTK_WriteFunctionParam.xsl     \
                ../config/mkfSTK_WriteFunctionPrototype.xsl \
                ../config/mkfSTK_WritePointerType.xsl       \
                ../config/mkfSTK_WriteStruct.xsl            \
                ../config/mkfSTK_WriteTypedef.xsl           \
                ../config/mkfSTK_WriteType.xsl


#
# list of all possible C-sources (used to create automatic dependencies)
# ----------------------------------------------------------------------
CSOURCENAMES = \
	$(foreach exe, $(EXECUTABLES) $(EXECUTABLES_L), $($(exe)_OBJECTS)) \
	$(foreach lib, $(LIBRARIES) $(LIBRARIES_L), $($(lib)_OBJECTS))

#
#>>>>> END OF standard rules

# define basic paths to MCSROOT:
ifdef MCSROOT
    TEMPLATES  = $(MCSROOT)/templates
endif
# if INTROOT is defined, override or complete the search paths:
ifdef INTROOT
    TEMPLATES  = $(INTROOT)/templates
endif

#
# CHOSE PLATFORM
# --------------
# Default is UNIX, for VxVorks application next line MUST BE UNCOMMENTED
#MAKE_VXWORKS = on   

#
# INCLUDE STANDARDS
# -----------------
MAKEDIR = ../include
include $(MAKEDIR)/mkfMakefile

#
# TARGETS
# -------
all:	move_scripts do_all
	@echo " . . . 'all' done" 

# when installing for the first time, the file is created locally, so an
# explicit cp is given to allow make to find them in ../bin 
move_scripts:
	@$(AT)  for scripts in $(SCRIPTS) ; \
	do \
		if [ ! -f ../bin/$${scripts} ] ; \
		then \
			. ./mkfMakeScript.sh $${scripts} ; \
		fi; \
	done

clean : clean_all 
	@echo " . . . clean done"

man   : do_man 
	@echo " . . . man page(s) done"

install : install_all
	-$(AT) cp -r ../templates/* ${TEMPLATES}
	-$(AT) rm -f ${TEMPLATES}/*/*~
	-$(AT) rm -f ${TEMPLATES}/*/.*.sw*
	@echo " . . . installation done"

#___oOo___
