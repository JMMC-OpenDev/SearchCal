#*******************************************************************************
# JMMC project
#
# "@(#) $Id: Makefile,v 1.23 2007-10-31 11:36:51 gzins Exp $"
#
# Makefile of sclsvr
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.22  2006/12/21 15:16:05  lafrasse
# Updated progression monitoring code (moved from static-based to instance-based).
#
# Revision 1.21  2006/07/17 09:10:36  scetre
# Added old scenario option
#
# Revision 1.20  2006/03/07 15:33:39  scetre
# Removed old scenario in band K
#
# Revision 1.19  2005/12/12 14:09:41  scetre
# Added old scenario with 2mass in primary request
#
# Revision 1.18  2005/11/29 13:04:42  scetre
# Used vobsSCENARIO instead of obsolete sclsvrSCEANRIO_CHECK
#
# Revision 1.17  2005/11/29 08:23:19  scetre
# Added check scenario for scenario faint K
#
# Revision 1.16  2005/11/23 08:39:29  scetre
# Added scenario for faint K
#
# Revision 1.15  2005/11/21 13:51:47  scetre
# Changed bad scenario name
# Added scenario for single star research -> updated getStar
#
# Revision 1.14  2005/11/15 15:01:19  scetre
# Updated with new scenario structure
#
# Revision 1.13  2005/06/20 14:42:54  scetre
# Removed filter to vobs unless visibility filter
#
# Revision 1.12  2005/06/01 14:18:54  scetre
# Added filters and filter list objects.
# Changed logExtDbg to logTrace
#
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
# Main server program 
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
				  sclsvrREQUEST.h        		\
				  sclsvrGETCAL_CMD.h        	\
				  sclsvrGETSTAR_CMD.h       	\
				  sclsvrVersion.h           	\
				  sclsvrSERVER.h				\
				  sclsvrVISIBILITY_FILTER.h		\
				  sclsvrSCENARIO_BRIGHT_K.h		\
				  sclsvrSCENARIO_BRIGHT_V.h		\
				  sclsvrSCENARIO_BRIGHT_N.h		\
				  sclsvrSCENARIO_FAINT_K.h		\
				  sclsvrSCENARIO_SINGLE_STAR.h 	\


#
# Libraries (public and local)
# ----------------------------
LIBRARIES       = sclsvr
LIBRARIES_L     =

#
# <brief description of lllll library>
sclsvr_OBJECTS   = sclsvrCALIBRATOR      		\
				   sclsvrCALIBRATOR_LIST 		\
				   sclsvrSERVER          		\
				   sclsvrREQUEST         		\
				   sclsvrGETCAL_CMD      		\
				   sclsvrGETSTAR_CMD     		\
				   sclsvrGetCalCB		 		\
				   sclsvrGetStarCB				\
				   sclsvrVISIBILITY_FILTER		\
				   sclsvrSCENARIO_BRIGHT_K		\
				   sclsvrSCENARIO_BRIGHT_V		\
				   sclsvrSCENARIO_BRIGHT_N		\
				   sclsvrSCENARIO_FAINT_K		\
				   sclsvrSCENARIO_SINGLE_STAR	\


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
