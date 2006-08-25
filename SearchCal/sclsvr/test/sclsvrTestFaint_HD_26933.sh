#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclsvrTestFaint_HD_26933.sh,v 1.1 2006-08-25 06:58:10 gzins Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
#*******************************************************************************

sclsvrServer -v 3 -noDate -noFileLine GETCAL "-objectName HD_26933 -ra 05 18 19.27 -dec -69 11 40.60 -mag 11.3 -band K -minMagRange 10 -maxMagRange 12 -wlen 2.2 -baseMax 102.45 -bright false"

#___oOo___
