#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************
# REMARKS
#    Makefile based on the VLT/ALMA Makefile
#
#-------------------------------------------------------------------------------


#
# user definable C-compilation flags
USER_CFLAGS = -D_GNU_SOURCE

#
# additional include and library search paths
#USER_INC = 
#USER_LIB = 

#
# MODULE CODE DESCRIPTION:
# ------------------------
#
# C programs (public and local)
# -----------------------------
EXECUTABLES     = 
EXECUTABLES_L   = 

#
# to set the shell to be used by panels
xxxx_OBJECTS    = 
xxxx_LDFLAGS    =
xxxx_LIBS       =


#
# Includes (.h) files (public only)
# ---------------------------------
INCLUDES        = mcs.h 

#
# Libraries (public and local)
# ----------------------------
LIBRARIES       = mcs
LIBRARIES_L     =

#
# <brief description of msg library>
mcs_OBJECTS     = mcs
mcs_LDFLAGS     = -lpthread

#
# Scripts (public and local)
SCRIPTS         =

#
# man pages to be done
# --------------------
MANSECTIONS = 
MAN1 = 
MAN5 = 
MAN7 = 

#
# other files to be installed
# ---------------------------
INSTALL_FILES = 

#
# list of all possible C-sources (used to create automatic dependencies)
# ------------------------------
CSOURCENAMES =\
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
# 
# REMARK: the local file (in /src) is used, so it can work also for
# installation on an empty system.
#
MAKEDIR = $(MCSROOT)/include
include $(MAKEDIR)/mkfMakefile

#
# TARGETS
# -------
all: do_all 
	@echo " . . . 'all' done" 

clean : clean_all
	@echo " . . . clean done"

man   : do_man
	@echo " . . . man page(s) done"

install : install_all
	@echo " . . . installation done"

#
#___oOo___

