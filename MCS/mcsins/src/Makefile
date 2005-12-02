#*******************************************************************************
# JMMC project
#
# "@(#) $Id: Makefile,v 1.6 2005-12-02 09:50:38 gzins Exp $"
#
# Makefile for installation script of MCS 
#
# History
# -------
# $Log: not supported by cvs2svn $
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

all: install

clean :
	@echo " . . . clean done"

man   :
	@echo " . . . man page(s) done"

install :
	@echo "Copying installation script file ...";
	@cp -p -f $(SCRIPT).sh ../../$(SCRIPT);
	@chmod 755 ../../$(SCRIPT);
	@echo "To start MCS intallation, just do the following:";
	@echo "   % cd ../..";
	@echo "   % ./mcsinsInstall";

#___oOo___
