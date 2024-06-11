#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
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
EXECUTABLES     = alxTestVisibility          \
                  alxTestGalacticCoordinates \
                  alxTestResearchArea        \
                  alxTestDistance            \
		  alxTestAngularDiameter     \
                  alxTestMagnitude           \
		  alxDecodeSpectralType \
                  alxTestNewLD2UD

#                  alxTestLD2UD\

EXECUTABLES_L   = 

#
# <brief description of alxTestMagnitude program>
alxTestMagnitude_OBJECTS   = alxTestMagnitude
alxTestMagnitude_LDFLAGS   =
alxTestMagnitude_LIBS      = MCS C++ alx 

#
# <brief description of alxTestGalacticCoordinates program>
alxTestGalacticCoordinates_OBJECTS   = alxTestGalacticCoordinates
alxTestGalacticCoordinates_LDFLAGS   =
alxTestGalacticCoordinates_LIBS      = MCS C++ alx

#
# <brief description of alxTestAngularDiameter program>
alxTestAngularDiameter_OBJECTS   = alxTestAngularDiameter
alxTestAngularDiameter_LDFLAGS   =
alxTestAngularDiameter_LIBS      = MCS C++ alx

#
# <brief description of alxTestVisibility program>
alxTestVisibility_OBJECTS   = alxTestVisibility
alxTestVisibility_LDFLAGS   =
alxTestVisibility_LIBS      = MCS C++ alx

#
# <brief description of alxTestResearchArea program>
alxTestResearchArea_OBJECTS   = alxTestResearchArea
alxTestResearchArea_LDFLAGS   =
alxTestResearchArea_LIBS      = MCS C++ alx

#
# <brief description of alxTestDistance program>
alxTestDistance_OBJECTS   = alxTestDistance
alxTestDistance_LDFLAGS   =
alxTestDistance_LIBS      = MCS C++ alx

# <brief description of alxDecodeSpectralType program>
alxDecodeSpectralType_OBJECTS   = alxDecodeSpectralType
alxDecodeSpectralType_LDFLAGS   =
alxDecodeSpectralType_LIBS      = MCS C++ alx

#
# <brief description of alxTestLD2UD program>
#alxTestLD2UD_OBJECTS   = alxTestLD2UD
#alxTestLD2UD_LDFLAGS   =
#alxTestLD2UD_LIBS      = MCS C++ alx

# <brief description of alxTestNewLD2UD program>
alxTestNewLD2UD_OBJECTS   = alxTestNewLD2UD
alxTestNewLD2UD_LDFLAGS   =
alxTestNewLD2UD_LIBS      = MCS C++ alx

#
# special compilation flags for single c sources
#yyyyy_CFLAGS   = 

#
# Includes (.h) files (public only)
# ---------------------------------
INCLUDES        =

#
# Libraries (public and local)
# ----------------------------
LIBRARIES       =
LIBRARIES_L     =

#
# <brief description of lllll library>
lllll_OBJECTS   =

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
all   : do_all
	@echo " . . . 'all' done" 

clean : clean_all 
	@echo " . . . clean done"

man   : do_man 
	@echo " . . . man page(s) done"

install : install_all
	@echo " . . . installation done"

###########################################################
#
#   Makefile to create a test module for C and C++ library
#   with python
#
###########################################################

#### WARNING : test not available for the moment. alx.i file does not take into
# consideration the alxCONFIDENCE_INDEX enumeration.
#
# Test generation: make gentest => ok
# Test execution: python testPyAlxComputeMissingMagnitudeBrightObj.py => not ok
# Error is :
# Traceback (most recent call last):
#  File "testPyAlxComputeMissingMagnitudeBrightObj.py", line 140, in ?
#    confIdx = pyAlx.new_alxCONFIDENCE_INDEXp()
# AttributeError: 'module' object has no attribute 'new_alxCONFIDENCE_INDEXp'
#
# The following line is not generated in the alx.i file (if it is added in the
# alx.i file, it works)
# %pointer_functions(alxCONFIDENCE_INDEX,alxCONFIDENCE_INDEXp);
# Error may be in the xsl files located in $MCSROOT/config directory

XSL_SOURCE = $(MCSROOT)/config/

MODULE_NAME = pyAlx

HEADER_FILE = alx

SOURCE_FILE = 

OBJECT_FILE = 

HEADER_PATH =  -I../include -I$(MCSROOT)/include

LIBRARYtoPROCESS =  -lalx -lmcs -llog -lerr -lmisc -lgdome

LIBRARY_PATH =  -L../lib -L$(MCSROOT)/lib


gentest: $(MODULE_NAME).so

$(HEADER_FILE).i: ../include/$(HEADER_FILE).h
	swig -I$(HEADER_PATH) -includeall -ignoremissing -module $(MODULE_NAME) -xml -o $(HEADER_FILE).xml ../include/$(HEADER_FILE).h
	xsltproc $(XSL_SOURCE)/mkfSTKToInterface.xsl $(HEADER_FILE).xml > $(HEADER_FILE).i

$(HEADER_FILE)_wrap.c: $(HEADER_FILE).i
	swig $(HEADER_PATH) -includeall -ignoremissing -python $(HEADER_FILE).i
	xsltproc $(XSL_SOURCE)/mkfSTKToPythonHelp.xsl $(HEADER_FILE).xml >> $(MODULE_NAME).py

$(HEADER_FILE)_wrap.o: $(HEADER_FILE)_wrap.c
	gcc -I/usr/include/python2.4/ -c -fpic $(SOURCE_FILE) $(HEADER_FILE)_wrap.c $(HEADER_PATH)

$(MODULE_NAME).so: $(HEADER_FILE)_wrap.o 
	gcc -shared $(LIBRARYtoPROCESS) $(LIBRARY_PATH) $(OBJECT_FILE) $(HEADER_FILE)_wrap.o -o _$(MODULE_NAME).so
	xsltproc $(XSL_SOURCE)/mkfSTKToPythonTestForC.xsl $(HEADER_FILE).xml > test_$(MODULE_NAME).py

cleantest:
	rm -f *~ $(HEADER_FILE).i *_wrap.* $(HEADER_FILE).xml $(OBJECT_FILE) _$(MODULE_NAME).so $(MODULE_NAME).py* test_$(MODULE_NAME).py

#___oOo___
