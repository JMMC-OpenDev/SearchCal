#!/bin/bash 
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclsvrTestEtaTau.sh,v 1.1 2006-04-10 15:20:56 gzins Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
#*******************************************************************************

sclsvrServer -v 3 -noDate -noFileLine GETCAL "-objectName ETA_TAU -mag 2.96 -diffRa 1800 -diffDec 600 -band K -minMagRange 2 -maxMagRange 4 -ra 03:47:29.076 -dec 24:06:18.494 -baseMax 102.45 -wlen 2.2"
