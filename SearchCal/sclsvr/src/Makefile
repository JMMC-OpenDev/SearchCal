#*******************************************************************************
# JMMC project
#
# "@(#) $Id: Makefile,v 1.12 2005-06-01 14:18:54 scetre Exp $"
#
# Makefile of sclsvr
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.11  2005/03/06 20:28:50  gzins
# Added sclsvrVersion.h
#
# Revision 1.10  2005/02/15 15:52:39  gzins
# Removed unused target
#
# Revision 1.9  2005/02/15 15:52:00  gzins
# Added CVS log as file modification history
#
# scetre    13-Sep-2004  Created
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
INCLUDES        = sclsvr.h						\
				  sclsvrCALIBRATOR.h			\
				  sclsvrCALIBRATOR_LIST.h		\
				  sclsvrFILTER.h				\
				  sclsvrFILTER_LIST.h			\
				  sclsvrDISTANCE_FILTER.h		\
				  sclsvrMAGNITUDE_FILTER.h		\
				  sclsvrVARIABILITY_FILTER.h	\
				  sclsvrMULTIPLICITY_FILTER.h	\
				  sclsvrVISIBILITY_FILTER.h		\
				  sclsvrSPECTRAL_TYPE_FILTER.h	\
				  sclsvrLUMINOSITY_FILTER.h		\
				  sclsvrREQUEST.h        		\
				  sclsvrGETCAL_CMD.h        	\
				  sclsvrGETSTAR_CMD.h       	\
				  sclsvrVersion.h           	\
				  sclsvrSERVER.h


#
# Libraries (public and local)
# ----------------------------
LIBRARIES       = sclsvr
LIBRARIES_L     =

#
# <brief description of lllll library>
sclsvr_OBJECTS   = sclsvrCALIBRATOR      		\
				   sclsvrCALIBRATOR_LIST 		\
				   sclsvrFILTER			 		\
				   sclsvrFILTER_LIST	 		\
				   sclsvrDISTANCE_FILTER 		\
				   sclsvrMAGNITUDE_FILTER		\
				   sclsvrVARIABILITY_FILTER		\
				   sclsvrMULTIPLICITY_FILTER	\
				   sclsvrVISIBILITY_FILTER		\
				   sclsvrSPECTRAL_TYPE_FILTER	\
				   sclsvrLUMINOSITY_FILTER		\
				   sclsvrSERVER          		\
				   sclsvrREQUEST         		\
				   sclsvrGETCAL_CMD      		\
				   sclsvrGETSTAR_CMD     		\
				   sclsvrGetCalCB		 		\
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

man   : do_man 
	@echo " . . . man page(s) done"

install : install_all
	@echo " . . . installation done"

#___oOo___
