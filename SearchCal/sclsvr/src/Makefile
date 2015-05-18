#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************
# REMARKS
#    Makefile based on the VLT Makefile
#
#-------------------------------------------------------------------------------

#
# user definable C-compilation flags (C99 NAN support)
USER_CFLAGS = -D_GNU_SOURCE

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
sclsvrServer_LIBS      = C++ MCS sclsvr simcli alx vobs MCS

#
# special compilation flags for single c sources
#yyyyy_CFLAGS   =

#
# Includes (.h) files (public only)
# ---------------------------------
INCLUDES        =	sclsvr.h			    \
			sclsvrCALIBRATOR.h		    \
			sclsvrCALIBRATOR_LIST.h		    \
			sclsvrREQUEST.h			    \
			sclsvrGETCAL_CMD.h		    \
			sclsvrGETSTAR_CMD.h		    \
			sclsvrVersion.h			    \
			sclsvrSERVER.h			    \
			sclsvrSCENARIO_BRIGHT_K.h	    \
			sclsvrSCENARIO_JSDC.h		    \
			sclsvrSCENARIO_JSDC_FAINT.h	    \
			sclsvrSCENARIO_JSDC_QUERY.h	    \
			sclsvrSCENARIO_BRIGHT_V.h	    \
			sclsvrSCENARIO_FAINT_K.h	    \
			sclsvrSCENARIO_SINGLE_STAR.h
#
# Libraries (public and local)
# ----------------------------
LIBRARIES       = sclsvr
LIBRARIES_L     =

#
# <brief description of lllll library>
sclsvr_OBJECTS   =	sclsvrCALIBRATOR		    \
			sclsvrCALIBRATOR_LIST		    \
			sclsvrSERVER			    \
			sclsvrREQUEST			    \
			sclsvrGETCAL_CMD		    \
			sclsvrGETSTAR_CMD		    \
			sclsvrGetCalCB			    \
			sclsvrGetStarCB			    \
			sclsvrSCENARIO_BRIGHT_K		    \
			sclsvrSCENARIO_JSDC		    \
			sclsvrSCENARIO_JSDC_FAINT	    \
			sclsvrSCENARIO_JSDC_QUERY	    \
			sclsvrSCENARIO_BRIGHT_V		    \
			sclsvrSCENARIO_FAINT_K		    \
			sclsvrSCENARIO_SINGLE_STAR
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
