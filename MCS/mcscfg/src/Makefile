#*******************************************************************************
# Jean-Marie Mariotti Center
#
# "@(#) $Id: Makefile,v 1.9 2005-02-16 09:26:56 gzins Exp $"
#
# Makefile of mcscfg module, which contains configuration files for MCS
# environment. 
#
# History
# -------
# $Log: not supported by cvs2svn $
# gzins     25-Jun-2004  Created
# gzins     10-Dec-2004  Added ../config/emacs
#
#------------------------------------------------------------------------
#
# Configuration files to be installed
#----------------------------
CFG_FILES = ../config/mcs.sh \
			../config/emacs  \
			../config/vimrc  \
			../config/gvimrc

#
# TARGETS
# -------
all:
	@echo " . . . 'all' done" 

clean :
	@echo " . . . clean done"

man   :
	@echo " . . . man page(s) done"

install :
	@if [ "$(INTROOT)" = "" ]; \
	then \
		echo "Installing configuration files: ";\
		if [ ! -d $(MCSROOT)/etc ]; \
		then \
			mkdir $(MCSROOT)/etc; \
		fi; \
		$(foreach file, $(CFG_FILES), echo "        `basename $(file)`" ;\
			chmod 755 $(file) ; \
			cp -p -f $(file) $(MCSROOT)/etc;) \
	fi
	@echo " . . . installation done"

#___oOo___
