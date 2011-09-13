#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
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
			../config/gvimrc       \
			../config/keystore

all: 
	@echo " . . . all done"

clean :
	@echo " . . . clean done"

man   :
	@echo " . . . man page(s) done"

# try to install if possible
install :
	@echo "Checking MCS environment: "
	@if [ "$(MCSTOP)" = "" ]; \
	then \
		echo "MCSTOP is not set properly; exit"; \
		exit 1; \
	fi;
	@if [ ! -d $(MCSTOP)/etc ]; \
	then \
		mkdir -p $(MCSTOP)/etc; \
	fi; 
	@if [ -w "$(MCSTOP)" ] ; then echo "Installing configuration files: " ; fi
	@$(foreach file, $(CFG_FILES), if [ -w "$(MCSTOP)" ] ; then echo "        `basename $(file)`" ;\
		chmod 755 $(file) ; \
		cp -p -f $(file) $(MCSTOP)/etc; fi; )
	@$(foreach file, $(OPT_CFG_FILES), \
		if [ ! -f $(MCSTOP)/etc/`basename $(file)` -a -w "$(MCSTOP)"]; \
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
