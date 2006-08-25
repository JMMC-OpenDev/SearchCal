#!/bin/bash 
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclsvrTestBright_Waql.sh,v 1.1 2006-08-25 06:58:56 gzins Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.1  2006/04/10 15:20:56  gzins
# Added
#
#*******************************************************************************

sclsvrServer -v 3 -noDate -noFileLine GETCAL "-objectName W_Aql -mag 0.8 -diffRa 3600 -diffDec 600 -band K -minMagRange 1 -maxMagRange 2 -ra 19:15:23.440 -dec -07:02:49.901 -baseMax 102.45 -wlen 2.2"
