#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclsvrTestFaint_AzV_266.sh,v 1.1 2006-08-25 06:58:10 gzins Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
#*******************************************************************************

sclsvrServer -v 3 -noDate -noFileLine GETCAL "-objectName AzV_266 -ra 01 01 09.42 -dec -72 27 28.40 -mag 12.9 -band K -minMagRange 10 -maxMagRange 13 -wlen 2.2 -baseMax 102.45 -bright false"

#___oOo___
