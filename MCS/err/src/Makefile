#*******************************************************************************
# JMMC project
#
# "@(#) $Id: Makefile,v 1.9 2004-09-10 17:08:24 gzins Exp $"
#
# Makefile of err module 
#
# who       when		 what
# --------  -----------	 -------------------------------------------------------
# sccmgr    02-Jun-2004  created
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
USER_INC = -I/usr/include/libgdome \
		   -I/usr/lib/glib-2.0/include/ \
		   -I/usr/include/glib-2.0
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
INCLUDES        = err.h

#
# Libraries (public and local)
# ----------------------------
LIBRARIES       = err
LIBRARIES_L     =

#
# <brief description of lllll library>
err_OBJECTS   = errCloseLocalStack_L \
				errResetLocalStack_L \
				errDisplayLocalStack_L \
				errIsInLocalStack_L \
				errLocalStackIsEmpty_L \
				errGetLocalStackSize_L \
				errPackLocalStack_L \
				errUnpackLocalStack_L \
				errPushInLocalStack_L \
				errAddInLocalStack_L \
				errCloseStack \
				errResetStack \
				errDisplayStack \
				errIsInStack \
				errStackIsEmpty \
				errGetStackSize \
				errPackStack \
				errUnpackStack \
				errAddInStack

#
# Scripts (public and local)
# --------------------------
SCRIPTS         = errXmlToH
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
JARFILES        = err

#
# <brief description of jjjfff jar files>
err_DIRS     	= jmmc
err_EXTRAS   	= 

#
# Java programs
# ---------------------------------
JAVA_EXECUTABLES=errEditor

#
# <brief description of jjjjjj java program>
errEditor_CLASS    = jmmc.mcs.err.ErrMenu

#
# man pages to be done
# --------------------
MANSECTIONS = 3
MAN1 =
MAN3 =$(addsuffix .c, $(err_OBJECTS))
MAN5 =
MAN7 =
MAN8 =

#
# other files to be installed
#----------------------------
INSTALL_FILES = ../config/errXmlToH.xsd \
				../config/errXmlToH.xsl

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

JAVA_JAR = xercesImpl.jar  xmlParserAPIs.jar

#
# INCLUDE STANDARDS
# -----------------
MAKEDIR = $(MCSROOT)/include
include $(MAKEDIR)/mkfMakefile

#
# TARGETS
# -------
all: move_scripts copy_local_jar do_all
	@echo " . . . 'all' done"

copy_local_jar:
	@echo "Copy local JAR files into ../lib"
	@$(foreach file, $(JAVA_JAR), echo "        $(file)" ;\
              cp -f ../jar/$(file) ../lib;)
	@echo ""

# when installing for the first time, the script errXmlToH is created locally,
# to allow generation of error header file used for test 
move_scripts:
	@$(AT)  for scripts in $(SCRIPTS) ; \
	do \
		if [ ! -f ../bin/$${scripts} ] ; \
		then \
			mkfMakeScript $${scripts} ; \
		fi; \
	done

clean : clean_all
	@echo " . . . clean done"

man   : do_man
	@echo " . . . man page(s) done"

install : install_local_jar install_all
	@echo " . . . installation done"

install_local_jar:
	@echo "Copy local JAR files into $(INSTALL_ROOT)/lib"
	@$(foreach file, $(JAVA_JAR), echo "        $(file)" ;\
              cp -f ../lib/$(file) $(INSTALL_ROOT)/lib;)
	@echo ""

db : db_all
	@echo " . . . ../DB done"


#___oOo___
