#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclsvrTestFaint_Elia_2-64.sh,v 1.1 2006-08-25 06:58:10 gzins Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
#*******************************************************************************

sclsvrServer -v 3 -noDate -noFileLine GETCAL "-objectName Elia_2-64 -ra 16 23 14.40 -dec -24 39 24.00 -mag 4.0 -band K -minMagRange 5 -maxMagRange 8 -wlen 2.2 -baseMax 102.45 -bright false"

#___oOo___
