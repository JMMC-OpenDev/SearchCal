#!/bin/bash 
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclsvrTestEpsSgr.sh,v 1.1 2006-04-10 15:20:56 gzins Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
#*******************************************************************************

sclsvrServer -v 3 -noDate -noFileLine GETCAL "-objectName eps_Sgr -mag 1.73 -diffRa 3600 -diffDec 1200 -band K -minMagRange 1.5 -maxMagRange 2.5 -ra 18:24:10.318 -dec -34:23:04.6180 -baseMax 102.45 -wlen 2.2"
