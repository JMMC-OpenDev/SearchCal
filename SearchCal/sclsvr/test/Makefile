#*******************************************************************************
# JMMC project
#
# "@(#) $Id: Makefile,v 1.4 2005-02-04 08:07:58 scetre Exp $"
#
# Makefile of sclsvr test module
#
# who       when         what
# --------  -----------  -------------------------------------------------------
# scetre    15-Sep-2004  Created
#
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
USER_INC = -I/usr/include/libxml2/ -I/usr/include/libgdome -I/usr/lib/glib-2.0/include/ -I/usr/include/glib-2.0
#USER_LIB = 

#
# MODULE CODE DESCRIPTION:
# ------------------------
# As a general rule:  public file are "cleaned" and "installed"  
#                     local (_L) are not "installed".

#
# C programs (public and local)
# -----------------------------
EXECUTABLES     = sclsvrTestCalibratorPack sclsvrTestCalibrator
EXECUTABLES_L   = 

#
# <brief description of xxxxx program>
#sclsvrTest_OBJECTS   = sclsvrTest
#sclsvrTest_LDFLAGS   = -lxml2 -lgdome -lstdc++
#sclsvrTest_LIBS      = MCS C++ sclsvr vobs

#sclsvrTestCommand_OBJECTS   = sclsvrTestCommand
#sclsvrTestCommand_LDFLAGS   = -lxml2 -lgdome -lstdc++
#sclsvrTestCommand_LIBS      = MCS C++ sclsvr vobs scitoo msg simcli

#sclsvrTestSearchCalibrators_OBJECTS   = sclsvrTestSearchCalibrators
#sclsvrTestSearchCalibrators_LDFLAGS   = -lxml2 -lgdome -lstdc++
#sclsvrTestSearchCalibrators_LIBS      = MCS C++ sclsvr vobs scitoo msg cmd simcli

#sclsvrSearchCalibrators_OBJECTS   = sclsvrSearchCalibrators
#sclsvrSearchCalibrators_LDFLAGS   = -lxml2 -lgdome -lstdc++
#sclsvrSearchCalibrators_LIBS      = MCS C++ sclsvr vobs alx msg simcli

#sclsvrTestSearchCalibratorsClient_OBJECTS   = sclsvrTestSearchCalibratorsClient
#sclsvrTestSearchCalibratorsClient_LDFLAGS   = -lxml2 -lgdome -lstdc++
#sclsvrTestSearchCalibratorsClient_LIBS      = MCS C++ sclsvr vobs scitoo msg gui simcli

#sclsvrTestCalibratorMethod_OBJECTS   = sclsvrTestCalibratorMethod
#sclsvrTestCalibratorMethod_LDFLAGS   = -lxml2 -lgdome -lstdc++
#sclsvrTestCalibratorMethod_LIBS      = MCS C++ sclsvr vobs alx msg simcli 

#sclsvrTestCalibratorPack_OBJECTS   = sclsvrTestCalibratorPack
#sclsvrTestCalibratorPack_LDFLAGS   = -lxml2 -lgdome -lstdc++
#sclsvrTestCalibratorPack_LIBS      = MCS C++ sclsvr vobs alx msg simcli

sclsvrTestCalibrator_OBJECTS   = sclsvrTestCalibrator
sclsvrTestCalibrator_LDFLAGS   = 
sclsvrTestCalibrator_LIBS      = MCS C++ sclsvr vobs alx simcli

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

db : db_all
	@echo " . . . ../DB done"


#___oOo___
