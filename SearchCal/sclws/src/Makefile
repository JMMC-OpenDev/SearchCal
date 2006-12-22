#*******************************************************************************
# JMMC project
#
# "@(#) $Id: Makefile,v 1.2 2006-12-22 15:17:50 lafrasse Exp $"
#
# Makefile of sclws module
#
# History
# -------
# $Log: not supported by cvs2svn $
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
#USER_INC = 
#USER_LIB = 

#
# MODULE CODE DESCRIPTION:
# ------------------------
# As a general rule:  public file are "cleaned" and "installed"  
#                     local (_L) are not "installed".


# Generate code for SAOP access to sclwsWS
# -n : use service name to rename service functions and namespace table
# -p : generated filename prefix = soap
# -L : don't generate soapClientLib/soapServerLib
# -x : don't generate sample XML message files
# -d : generated file output path = ../include/
soapbuild:
	/usr/local/bin/soapcpp2 -n -psoap -L -x -d../include/ ../include/sclwsWS.h
	mv ../include/*.cpp ./

# Remove any SOAP-generatdd file
soapclean:
	rm -f ../include/soap* ../include/*.xml ../include/*.xsd ../include/*.wsdl ../include/ns.*
	rm -f soap*


#
# C programs (public and local)
# -----------------------------
EXECUTABLES     = sclwsServer sclwsGetCal
EXECUTABLES_L   = 

#
# <brief description of sclwsServer program>
sclwsServer_OBJECTS   = sclwsServer \
                        sclwsWS \
                        soapC \
                        soapServer
sclwsServer_LDFLAGS   = 
sclwsServer_LIBS      = MCS C++ gsoap++ pthread uuid sclsvr vobs alx simcli thrd

#
# <brief description of sclwsGetCal program>
sclwsGetCal_OBJECTS   = sclwsGetCal \
                        sclwsWS \
                        soapC \
                        soapClient
sclwsGetCal_LDFLAGS   = 
sclwsGetCal_LIBS      = MCS C++ gsoap++ pthread uuid sclsvr vobs alx simcli thrd

#
# special compilation flags for single c sources
#yyyyy_CFLAGS   = 

#
# Includes (.h) files (public only)
# ---------------------------------
INCLUDES        = sclws.h

#
# Libraries (public and local)
# ----------------------------
LIBRARIES       = 
LIBRARIES_L     =

#
# <brief description of sclws library>
sclws_OBJECTS   = 

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
# Command Definition Files (CDF)
# ---------------------------------
CDF_FILES       =

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
