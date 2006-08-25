#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclsvrTestFaint_HD_103945.sh,v 1.1 2006-08-25 06:58:10 gzins Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
#*******************************************************************************

sclsvrServer -v 3 -noDate -noFileLine GETCAL "-objectName HD_103945 -ra 11 58 13.93 -dec +03 28 55.20 -mag 2.1 -band K -minMagRange 5 -maxMagRange 8 -wlen 2.2 -baseMax 102.45 -bright false"

#___oOo___
