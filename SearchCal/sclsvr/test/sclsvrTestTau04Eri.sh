#!/bin/bash 
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclsvrTestTau04Eri.sh,v 1.1 2006-04-10 15:20:56 gzins Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
#*******************************************************************************

sclsvrServer -v 3 -noDate -noFileLine GETCAL "-objectName tau04_Eri -mag -1.17 -diffRa 3600 -diffDec 1200 -band K -minMagRange -1 -maxMagRange 2 -ra 03:19:31.002 -dec -21:45:28.310 -baseMax 102.45 -wlen 2.2"
