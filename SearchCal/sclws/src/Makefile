#*******************************************************************************
# JMMC project
#
# "@(#) $Id: Makefile,v 1.9 2010-11-04 15:59:48 lafrasse Exp $"
#
# Makefile of sclws module
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.8  2008/08/25 12:51:40  lafrasse
# Added sclwsManager file installation (automated replicat needs it to start it automatically).
#
# Revision 1.7  2008/08/25 12:48:16  lafrasse
# Added commom MCS LDFLAG.
#
# Revision 1.6  2007/10/31 11:50:24  gzins
# Removed sclwsGetCal program (moved to test directory)
# Created library
# Re-organized targets
#
# Revision 1.5  2007/05/14 08:38:22  gzins
# Removed full path when invoking soapcpp2 tool
#
# Revision 1.4  2007/02/14 13:40:26  lafrasse
# Added export of the WSDL file.
#
# Revision 1.3  2007/02/14 10:25:03  lafrasse
# Added and integrated SOAP targets.
#
# Revision 1.2  2006/12/22 15:17:50  lafrasse
# Creation
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
sclwsServer_LDFLAGS   = ${MCSSTD_LIBLIST}
sclwsServer_LIBS      = MCS C++ gsoap++ pthread uuid sclws sclsvr vobs alx simcli thrd

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
SCRIPTS         = sclwsManager
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
all: do_soap do_all
	@echo " . . . 'all' done" 

clean : clean_soap clean_all
	@echo " . . . clean done"

man   : do_man
	@echo " . . . man page(s) done"

install : install_proxy install_all
	@echo " . . . installation done"

# Specific targets

# Generate code for SAOP access to sclwsWS
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

# Remove any SOAP-generatdd file
clean_soap:
	@echo "Cleaning SOAP code . . ."
	rm -f ../include/soap* ../include/*.xml ../include/*.xsd ../include/*.wsdl ../include/ns.*
	rm -f soap*
	@echo " . . . done"

# Copy PHP proxy script in current user's public_html directory
install_proxy:
	@echo "Installing PHP proxy in ~/public_html . . ."
	cp sclws-proxy.php ~/public_html/
	@echo " . . . done"

#___oOo___
