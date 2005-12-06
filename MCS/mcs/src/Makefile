#*******************************************************************************
# Jean-Marie Mariotti Center 
#
# "@(#) $Id: Makefile,v 1.7 2005-12-06 07:46:35 gzins Exp $" 
#
# Makefile for MCS module 
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.6  2005/12/02 14:18:08  swmgr
# Replaced by mcsRelease
#
# Revision 1.5  2005/03/11 14:16:22  mella
# Add mcsRoot.sh that should be sourced
#
# Revision 1.4  2005/01/28 18:50:52  gzins
# Added CVS log as modification history
#
# gzins     11/04/04  Created
#

#*******************************************************************************
# This Makefile follows VLT Standards (see Makefile manpage documentation).
#
# REMARK: this should be the first module to be installed.
#         For such a reason, in few cases, it needs to work from /src.
#*******************************************************************************

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

