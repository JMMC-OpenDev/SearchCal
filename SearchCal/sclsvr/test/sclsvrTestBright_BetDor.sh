#!/bin/bash 
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclsvrTestBright_BetDor.sh,v 1.1 2006-08-25 06:58:56 gzins Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.1  2006/04/10 15:20:56  gzins
# Added
#
#*******************************************************************************

sclsvrServer -v 3 -noDate -noFileLine GETCAL "-objectName bet_Dor -mag 1.83 -diffRa 3600 -diffDec 600 -band K -minMagRange 1 -maxMagRange 3 -ra 05:33:37.518 -dec -62:29:23.371 -baseMax 102.45 -wlen 2.2"
