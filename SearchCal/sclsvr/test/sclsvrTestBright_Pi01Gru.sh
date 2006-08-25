#!/bin/bash 
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclsvrTestBright_Pi01Gru.sh,v 1.1 2006-08-25 06:58:56 gzins Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.1  2006/04/10 15:20:56  gzins
# Added
#
#*******************************************************************************

sclsvrServer -v 3 -noDate -noFileLine GETCAL "-objectName pi01_Gru -mag -2.1 -diffRa 3600 -diffDec 1200 -band K -minMagRange -2 -maxMagRange 2 -ra 22:22:44.205 -dec -45:56:52.598 -baseMax 102.45 -wlen 2.2"
