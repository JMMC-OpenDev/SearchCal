#*******************************************************************************
# Jean-Marie Mariotti Center
#
# "@(#) $Id: Makefile,v 1.10 2005-02-28 16:09:12 lafrasse Exp $"
#
# Makefile of mcscfg module, which contains configuration files for MCS
# environment.
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.9  2005/02/16 09:26:56  gzins
# Added CVS log as modification file history
#
# gzins     25-Jun-2004  Created
# gzins     10-Dec-2004  Added ../config/emacs
#
#------------------------------------------------------------------------
#
# Configuration files to be installed
#----------------------------
CFG_FILES = ../config/mcs.sh      \
			../config/emacs       \
			../config/vimrc       \
			../config/gvimrc
#
# Configuration files to be installed but not over-written
#----------------------------
OPT_CFG_FILES = ../config/mcsEnvList

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
	@if [ "$(INTROOT)" = "/home/users/lafrasse/Dev/INTROOT" ]; \
	then \
		echo "Installing configuration files: ";\
		if [ ! -d $(MCSROOT)/etc ]; \
		then \
			mkdir $(MCSROOT)/etc; \
		fi; \
		$(foreach file, $(CFG_FILES), echo "        `basename $(file)`" ;\
			chmod 755 $(file) ; \
			cp -p -f $(file) $(MCSROOT)/etc;) \
		$(foreach file, $(OPT_CFG_FILES), \
			if [ ! -f $(MCSROOT)/etc/`basename $(file)` ]; \
			then \
				echo "        `basename $(file)`" ; \
				chmod 755 $(file) ; \
				cp -p -f $(file) $(MCSROOT)/etc; \
			fi;) \
	fi
	@echo " . . . installation done"

#___oOo___
