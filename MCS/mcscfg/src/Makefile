#*******************************************************************************
# Jean-Marie Mariotti Center
#
# "@(#) $Id: Makefile,v 1.7 2004-12-10 09:19:22 gzins Exp $"
#
# Makefile of mcscfg module, which contains configuration files for MCS
# environment. 
#
# who       when         what
# --------  -----------  ----------------------------------------------
# gzins     25-Jun-2004  Created
# gzins     10-Dec-2004  Added ../config/emacs
#
#------------------------------------------------------------------------

#
# Configuration files to be installed
#----------------------------
CFG_FILES = ../config/bashrc \
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
