#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclsvrTestFaint_HD_33793.sh,v 1.1 2006-08-25 06:58:10 gzins Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
#*******************************************************************************

sclsvrServer -v 3 -noDate -noFileLine GETCAL "-objectName HD_33793 -ra 05 11 40.58 -dec -45 01 06.25 -mag 5.0 -band K -minMagRange 5 -maxMagRange 8 -wlen 2.2 -baseMax 102.45 -bright false"

#___oOo___
