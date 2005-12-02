#*******************************************************************************
# Jean-Marie Mariotti Center
#
# "@(#) $Id: Makefile,v 1.13 2005-12-02 12:19:15 mella Exp $"
#
# Makefile of mcscfg module, which contains configuration files for MCS
# environment.
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.12  2005/12/02 10:25:36  gzins
# Updated for new MCS directory structure; installed files in $MCSTOP/etc
#
# Revision 1.11  2005/03/03 10:07:26  gildas
# Fixed bug in install target
#
# Revision 1.10  2005/02/28 16:09:12  lafrasse
# Added 'mcsEnvFile'
#
# Revision 1.9  2005/02/16 09:26:56  gzins
# Added CVS log as modification file history
#
# gzins     25-Jun-2004  Created
# gzins     10-Dec-2004  Added ../config/emacs
#
#------------------------------------------------------------------------

#
# Scripts
# --------------------------
SCRIPTS         = mcscfgGenerateEnvList.sh

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
	@./mcscfgGenerateEnvList.sh ../config/mcsEnvList.xml ../config/mcsEnvList
	@echo " . . . 'all' done"

clean :
	rm ../config/mcsEnvList
	@echo " . . . clean done"

man   :
	@echo " . . . man page(s) done"

install :
	@if [ "$(INTROOT)" = "" ]; \
	then \
		echo "Checking MCS environment: ";\
		if [ "$(MCSTOP)" = "" ]; \
		then \
			echo "MCSTOP is not set properly; exit"; \
			exit 1; \
		fi; \
		echo "Installing configuration files: ";\
		if [ ! -d $(MCSTOP)/etc ]; \
		then \
			mkdir $(MCSTOP)/etc; \
		fi; \
		$(foreach file, $(CFG_FILES), echo "        `basename $(file)`" ;\
			chmod 755 $(file) ; \
			cp -p -f $(file) $(MCSTOP)/etc;) \
		$(foreach file, $(OPT_CFG_FILES), \
			if [ ! -f $(MCSTOP)/etc/`basename $(file)` ]; \
			then \
				echo "        `basename $(file)`" ; \
				chmod 755 $(file) ; \
				cp -p -f $(file) $(MCSTOP)/etc; \
			fi;) \
	fi
	@echo " . . . installation done"

#___oOo___
