#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************
# REMARKS
#    Makefile based on the VLT/ALMA Makefile
#
#-------------------------------------------------------------------------------


# Disable DEBUG directive to disable gSOAP DEBUG logs
ifdef DEBUG
DEBUG=
endif

ifndef OPTIMIZE
# Force optimization to disable DEBUG directive in CFLAGS (defaults)
OPTIMIZE=1
endif

#
# user definable C-compilation flags
USER_CFLAGS = -D_GNU_SOURCE -rdynamic 

#
# user definable javac compilation flags
#USER_JFLAGS = 

#
# additional include and library search paths
#USER_INC = 
#USER_LIB = 

#
# MODULE CODE DESCRIPTION:
# ------------------------
# As a general rule:  public file are "cleaned" and "installed"  
#                     local (_L) are not "installed".

#
# C programs (public and local)
# -----------------------------
EXECUTABLES     = sclwsServer
EXECUTABLES_L   = 

#
# Web service server
sclwsServer_OBJECTS   = sclwsServer
sclwsServer_LDFLAGS   = ${MCSSTD_LIBLIST} -lpthread
sclwsServer_LIBS      = MCS C++ sclws sclsvr vobs alx simcli gsoap++ pthread uuid

#
#
# special compilation flags for single c sources
#yyyyy_CFLAGS   = 

#
# Includes (.h) files (public only)
# ---------------------------------
INCLUDES        = sclws.h sclws.wsdl

#
# Libraries (public and local)
# ----------------------------
LIBRARIES       = sclws
LIBRARIES_L     = 

#
# <brief description of sclws library>
sclws_OBJECTS   = sclwsWS 		\
                  soapC 		\
                  soapServer	\
                  soapClient

#
# Scripts (public and local)
# --------------------------
SCRIPTS         = sclwsManager sclwsStatUsage
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
# Command Definition Files (CDF)
# ---------------------------------
CDF_FILES       =

#
# man pages to be done
# --------------------
MANSECTIONS = 1
MAN1 = $(addsuffix .c, $(EXECUTABLES))
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
all: do_soap do_all
	@echo " . . . 'all' done" 

clean : clean_all
	@echo " . . . clean done"

man   : do_man
	@echo " . . . man page(s) done"

install : install_proxy install_all
	@echo " . . . installation done"

# Specific targets

# Generate code for SOAP access to sclwsWS
# -n : use service name to rename service functions and namespace table
# -p : generated filename prefix = soap
# -L : don't generate soapClientLib/soapServerLib
# -x : don't generate sample XML message files
# -d : generated file output path = ../include/
do_soap:
	@echo "Synthetizing SOAP code . . ."
	soapcpp2 -n -psoap -L -x -d../include/ ../include/sclwsWS.h
	mv ../include/*.cpp ./
	@echo " . . . done"

# Copy PHP proxy script in current user's public_html directory
install_proxy:
	@if [ -e ~/public_html/sclwsProxy.php ] ; then \
		echo "WARNING: using sclwsProxy.php already installed in ~/public_html/ to keep specific modifications (remove it to update)..." ; \
	else \
		echo "Installing sclwsProxy.php in ~/public_html/ ..." ; \
		cp sclwsProxy.php ~/public_html/ ; \
	fi
	@if [ -e ~/public_html/getstar/sclwsGetStarProxy.php ] ; then \
		echo "WARNING: using sclwsGetStarProxy.php already installed in ~/public_html/getstar/ to keep specific modifications (remove it to update)..." ; \
	else \
		echo "Installing sclwsGetStarProxy.php in ~/public_html/getstar/ ..." ; \
		mkdir ~/public_html/getstar/ ; \
		cp index.html ~/public_html/getstar/ ; \
		cp sclwsGetStarProxy.php ~/public_html/getstar/ ; \
		cp getstarVOTableToHTML.xsl ~/public_html/getstar/ ; \
	fi

# proxy log is in the home directory:
	touch ~/sclwsProxy.log
	chmod g+w ~/sclwsProxy.log
	@echo " . . . done"

#___oOo___
