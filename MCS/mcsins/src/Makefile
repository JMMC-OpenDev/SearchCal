#*******************************************************************************
# JMMC project
#
# "@(#) $Id: Makefile,v 1.4 2004-12-09 06:13:54 gzins Exp $"
#
# Makefile for installation script of MCS 
#
# who       when         what
# --------  -----------  -------------------------------------------------------
# gzins     08-Dec-2004  Created
# gzins     08-Dec-2004  Fixed bug when installing script in $MCSROOT/bin 
# gzins     09-Dec-2004  Renamed mcsinsInstallMcs to mcsinsInstall
#*******************************************************************************
# REMARKS
#    Makefile based on the VLT Makefile
#
#-------------------------------------------------------------------------------

#
# Scripts (public and local)
# --------------------------
SCRIPTS         = mcsinsInstall

all:
	@echo " . . . 'all' done" 

clean :
	@echo " . . . clean done"

man   :
	@echo " . . . man page(s) done"

install :
	@if [ "$(INTROOT)" = "" ]; \
	then \
		echo "Installing installation script file: ";\
		if [ ! -d $(MCSROOT)/bin ]; \
		then \
			mkdir -p $(MCSROOT)/bin; \
		fi; \
		$(foreach file, $(SCRIPTS), echo "        `basename $(file)`" ;\
			cp -p -f $(file).sh $(MCSROOT)/bin/$(file); \
			chmod 755 $(MCSROOT)/bin/$(file);) \
	fi
	@echo " . . . installation done"


#___oOo___
