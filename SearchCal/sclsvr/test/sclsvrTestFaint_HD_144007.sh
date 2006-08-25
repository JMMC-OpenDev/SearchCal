#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclsvrTestFaint_HD_144007.sh,v 1.1 2006-08-25 06:58:10 gzins Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
#*******************************************************************************

sclsvrServer -v 3 -noDate -noFileLine GETCAL "-objectName HD_144007 -ra 16 05 15.26 -dec -40 19 10.34 -mag 5.0 -band K -minMagRange 5 -maxMagRange 8 -wlen 2.2 -baseMax 102.45 -bright false"

#___oOo___
