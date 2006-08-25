#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclsvrTestFaint_V615_Ori.sh,v 1.1 2006-08-25 06:58:10 gzins Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
#*******************************************************************************

sclsvrServer -v 3 -noDate -noFileLine GETCAL "-objectName V615_Ori -ra 05 41 24.30 -dec -02 22 38.90 -mag 7.4 -band K -minMagRange 5 -maxMagRange 10 -wlen 2.2 -baseMax 102.45 -bright false"

#___oOo___
