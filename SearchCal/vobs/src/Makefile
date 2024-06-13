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
EXECUTABLES     =  vobsStarSeparation
EXECUTABLES_L   =

#
# <brief description of vobsStarSeparation program>
vobsStarSeparation_OBJECTS   = vobsStarSeparation
vobsStarSeparation_LDFLAGS   =
vobsStarSeparation_LIBS      = MCS C++ vobs alx

#
# special compilation flags for single c sources
#yyyyy_CFLAGS   =

#
# Includes (.h) files (public only)
# ---------------------------------
INCLUDES        = vobs.h						\
                                  vobsSTAR_PROPERTY_META.h              \
				  vobsSTAR.h 			   	\
				  vobsSTAR_PROPERTY.h 			\
				  vobsSTAR_LIST.h 		   	\
				  vobsREQUEST.h 		   	\
				  vobsCDATA.h			   	\
				  vobsPARSER.h			   	\
				  vobsCATALOG.h 		   	\
				  vobsCATALOG_COLUMN.h 		   	\
				  vobsCATALOG_META.h 		   	\
				  vobsREMOTE_CATALOG.h 			\
				  vobsLOCAL_CATALOG.h 			\
				  vobsCATALOG_ASCC_LOCAL.h 	   	\
				  vobsCATALOG_JSDC_FAINT_LOCAL.h  	\
				  vobsCATALOG_LIST.h			\
				  vobsVIRTUAL_OBSERVATORY.h		\
				  vobsSCENARIO_ENTRY.h			\
				  vobsSCENARIO_RUNTIME.h                \
				  vobsSCENARIO.h	       		\
				  vobsSTAR_COMP_CRITERIA_LIST.h		\
				  vobsGENERIC_FILTER.h			\
				  vobsFILTER.h				\
				  vobsFILTER_LIST.h			\
				  vobsMAGNITUDE_FILTER.h		\
				  vobsDISTANCE_FILTER.h			\
				  vobsORIGIN_FILTER.h			\
				  vobsVOTABLE.h
#
# Libraries (public and local)
# ----------------------------
LIBRARIES       = vobs
LIBRARIES_L     =

#
# <brief description of vobs library>
vobs_OBJECTS   =   vobsSTAR						\
                                   vobsSTAR_PROPERTY_META               \
				   vobsSTAR_PROPERTY			\
				   vobsSTAR_LIST 			\
				   vobsREQUEST 				\
				   vobsCDATA				\
				   vobsPARSER				\
				   vobsCATALOG 				\
				   vobsREMOTE_CATALOG			\
				   vobsLOCAL_CATALOG			\
				   vobsCATALOG_ASCC_LOCAL 		\
				   vobsCATALOG_JSDC_FAINT_LOCAL		\
				   vobsCATALOG_LIST 			\
				   vobsVIRTUAL_OBSERVATORY		\
				   vobsSCENARIO_ENTRY			\
				   vobsSCENARIO				\
				   vobsSTAR_COMP_CRITERIA_LIST		\
				   vobsFILTER				\
				   vobsGENERIC_FILTER			\
				   vobsFILTER_LIST			\
				   vobsMAGNITUDE_FILTER			\
				   vobsDISTANCE_FILTER			\
				   vobsORIGIN_FILTER			\
				   vobsVOTABLE
#
# Scripts (public and local)
# --------------------------
SCRIPTS         = vobsCheckVOTable \
		  vobsCopyVotParamsAsFields
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
MANSECTIONS =
MAN1 =
MAN3 =
MAN5 =
MAN7 =
MAN8 =

#
# other files to be installed
#----------------------------
INSTALL_FILES = ../config/*.cfg \
				../config/*.xs*

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

unzip_cfg:
	gzip -f -d -k ../config/*.cfg.gz

install : unzip_cfg install_all
	@echo " . . . installation done"

#___oOo___
