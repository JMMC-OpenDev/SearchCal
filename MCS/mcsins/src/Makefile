#*******************************************************************************
# JMMC project
#
# "@(#) $Id: Makefile,v 1.8 2006-10-14 10:38:37 gzins Exp $"
#
# Makefile for installation script of MCS 
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.7  2005/12/06 06:38:06  gzins
# Added installation of MCS configuration files
#
# Revision 1.6  2005/12/02 09:50:38  gzins
# Just copied installation script in ../.. directory instead of $MCSROOT or $INTROOT
#
# Revision 1.5  2005/01/29 13:44:04  gzins
# Added CVS log as modification history
#
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
SCRIPT = mcsinsInstall
SHELL  = /bin/bash
PWD    := $(shell echo ${PWD%/*/*})

#
# Configuration files to be installed
#----------------------------
CFG_FILES = ../config/mcs.sh      \
			../config/emacs       \
			../config/vimrc       \
			../config/gvimrc

all: 
	@echo " . . . all done"

clean :
	@echo " . . . clean done"

man   :
	@echo " . . . man page(s) done"

install :
	@echo "Checking MCS environment: "
	@if [ "$(MCSTOP)" = "" ]; \
	then \
		echo "MCSTOP is not set properly; exit"; \
		exit 1; \
	fi;
	@echo "Installing configuration files: ";
	@if [ ! -d $(MCSTOP)/etc ]; \
	then \
		mkdir $(MCSTOP)/etc; \
	fi;
	@$(foreach file, $(CFG_FILES), echo "        `basename $(file)`" ;\
		chmod 755 $(file) ; \
		cp -p -f $(file) $(MCSTOP)/etc;)
	@$(foreach file, $(OPT_CFG_FILES), \
		if [ ! -f $(MCSTOP)/etc/`basename $(file)` ]; \
		then \
			echo "        `basename $(file)`" ; \
			chmod 755 $(file) ; \
			cp -p -f $(file) $(MCSTOP)/etc; \
		fi;)
	@echo "Copying installation script file ...";
	@cp -p -f $(SCRIPT).sh ../../$(SCRIPT);
	@chmod 755 ../../$(SCRIPT);
	@echo "To start MCS intallation, just do the following:";
	@echo "   % cd ../..";
	@echo "   % ./mcsinsInstall";

#___oOo___
