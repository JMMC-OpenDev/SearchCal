#*******************************************************************************
# Jean-Marie Mariotti Center
#
# "@(#) $Id: Makefile,v 1.4 2004-06-25 06:32:21 gzins Exp $"
#
# Makefile of mcscfg module, which contains configuration files for MCS
# environment. 
#
# who       when        what
# --------  ----------  ----------------------------------------------
# gzins     25/06/2004  created
#
#------------------------------------------------------------------------

#
# Configuration files to be installed
#----------------------------
CFG_FILES = ../config/bashrc \
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
		$(foreach file, $(CFG_FILES), echo "        `basename $(file)`" ;\
			cp -f $(file) $(MCSROOT)/etc;) \
	fi
	@echo " . . . installation done"

#___oOo___
