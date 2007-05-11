#*******************************************************************************
# JMMC project
#
# "@(#) $Id: Makefile,v 1.11 2007-05-11 15:52:23 gzins Exp $"
#
# Makefile of sclsvr test module
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.10  2006/08/25 06:58:56  gzins
# New name for tests related to 'bright' scenario
#
# Revision 1.9  2006/04/19 12:09:31  gzins
# Added L_Car test script
#
# Revision 1.8  2006/04/10 14:56:58  gzins
# Added test scripts
#
# Revision 1.7  2005/11/25 13:14:45  scetre
# *** empty log message ***
#
# Revision 1.6  2005/02/15 15:52:00  gzins
# Added CVS log as file modification history
#
# scetre    15-Sep-2004  Created
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
#USER_LIB = 

#
# MODULE CODE DESCRIPTION:
# ------------------------
# As a general rule:  public file are "cleaned" and "installed"  
#                     local (_L) are not "installed".

#
# C programs (public and local)
# -----------------------------
EXECUTABLES     = sclsvrTestCalibrator 
EXECUTABLES_L   = 

#
# <brief description of xxxxx program>

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
SCRIPTS_L       = sclsvrTestBright_AlfPsa    \
                  sclsvrTestBright_BetCap    \
                  sclsvrTestBright_BetDor    \
                  sclsvrTestBright_DelOph    \
                  sclsvrTestBright_EpsSgr    \
                  sclsvrTestBright_EtaTau    \
                  sclsvrTestBright_FuOri     \
                  sclsvrTestBright_Pi01Gru   \
                  sclsvrTestBright_Tau04Eri  \
                  sclsvrTestBright_Waql      \
				  sclsvrTestBright_ZetGem    \
				  sclsvrTestBright_HD_103945 \
				  sclsvrTestBright_BD+23_492 \
				  \
				  sclsvrTestFaint_HD_5980    \
				  sclsvrTestFaint_AzV_266    \
				  sclsvrTestFaint_NGC_1068   \
				  sclsvrTestFaint_BD+23_492  \
				  sclsvrTestFaint_HD_26933   \
				  sclsvrTestFaint_HD_33793   \
				  sclsvrTestFaint_V615_Ori   \
				  sclsvrTestFaint_FU_Ori     \
				  sclsvrTestFaint_TW_Hya     \
				  sclsvrTestFaint_HD_103945  \
				  sclsvrTestFaint_HD_111688  \
				  sclsvrTestFaint_U_Mus      \
				  sclsvrTestFaint_HD_144007  \
				  sclsvrTestFaint_Elia_2-64  \
				  sclsvrTestFaint_S_Cra      \
				  sclsvrTestFaint_L_Car
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
