#*******************************************************************************
# JMMC project
#
# "@(#) $Id: Makefile,v 1.5 2005-01-25 11:07:48 gzins Exp $"
#
# Makefile of sclsvr
#
# who       when         what
# --------  -----------  -------------------------------------------------------
# scetre    13-Sep-2004  Created
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
USER_INC =  -I/usr/include/libxml2/ -I/usr/include/libgdome -I/usr/lib/glib-2.0/include/ -I/usr/include/glib-2.0
#USER_LIB = 

#
# MODULE CODE DESCRIPTION:
# ------------------------
# As a general rule:  public file are "cleaned" and "installed"  
#                     local (_L) are not "installed".

#
# C programs (public and local)
# -----------------------------
EXECUTABLES     = sclsvrServer
EXECUTABLES_L   = 

#
# <brief description of xxxxx program>
sclsvrServer_OBJECTS   = sclsvrServer
sclsvrServer_LDFLAGS   =
sclsvrServer_LIBS      = MCS C++ vobs alx sclsvr simcli

#
# special compilation flags for single c sources
#yyyyy_CFLAGS   = 

#
# Includes (.h) files (public only)
# ---------------------------------
INCLUDES        = sclsvr.h					\
				  sclsvrCALIBRATOR.h		\
				  sclsvrCALIBRATOR_LIST.h	\
				  sclsvrSEARCH_CALIBRATORS.h\
				  sclsvrGETCAL_CMD.h        \
				  sclsvrGETSTAR_CMD.h       \
				  sclsvrSERVER.h


#
# Libraries (public and local)
# ----------------------------
LIBRARIES       = sclsvr
LIBRARIES_L     =

#
# <brief description of lllll library>
sclsvr_OBJECTS   = sclsvrCALIBRATOR      \
				   sclsvrCALIBRATOR_LIST \
				   sclsvrSERVER          \
				   sclsvrGETCAL_CMD      \
				   sclsvrGETSTAR_CMD     \
				   sclsvrGetCalCB		 \
				   sclsvrGetStarCB

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
# Command Definition Files (CDF) 
#----------------------------
CDF_FILES     = sclsvrGETCAL sclsvrGETSTAR


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

clean_dist : clean_all clean_dist_all 
	@echo " . . . clean_dist done"

man   : do_man 
	@echo " . . . man page(s) done"

install : install_all
	@echo " . . . installation done"

db : db_all
	@echo " . . . ../DB done"


#___oOo___
