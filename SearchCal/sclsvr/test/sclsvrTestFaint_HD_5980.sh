#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclsvrTestFaint_HD_5980.sh,v 1.1 2006-08-25 06:58:10 gzins Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
#*******************************************************************************

sclsvrServer -v 3 -noDate -noFileLine GETCAL "-objectName HD_5980 -ra 00 59 26.50 -dec -72 09 53.90 -mag 10.8 -band K -minMagRange 8 -maxMagRange 12 -wlen 2.2 -baseMax 102.45 -bright false"

#___oOo___
