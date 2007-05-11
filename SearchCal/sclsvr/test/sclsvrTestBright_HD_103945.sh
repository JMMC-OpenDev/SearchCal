#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclsvrTestBright_HD_103945.sh,v 1.1 2007-05-11 15:52:58 gzins Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
#*******************************************************************************

sclsvrServer -v 3 -noDate -noFileLine GETCAL "-objectName HD_103945 -ra 11 58 13.93 -dec +03 28 55.20 -mag 2.1 -diffRa 3600 -diffDec 1200 -band K -minMagRange 5 -maxMagRange 8 -wlen 2.2 -baseMax 102.45"

#___oOo___
