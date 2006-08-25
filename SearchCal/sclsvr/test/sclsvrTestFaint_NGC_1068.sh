#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclsvrTestFaint_NGC_1068.sh,v 1.1 2006-08-25 06:58:10 gzins Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
#*******************************************************************************

sclsvrServer -v 3 -noDate -noFileLine GETCAL "-objectName NGC_1068 -ra 02 42 40.83 -dec -00 00 48.40 -mag 1.8 -band K -minMagRange 5 -maxMagRange 8 -wlen 2.2 -baseMax 102.45 -bright false"

#___oOo___
