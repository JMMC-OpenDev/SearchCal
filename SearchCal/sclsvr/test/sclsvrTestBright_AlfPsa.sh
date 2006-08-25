#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclsvrTestBright_AlfPsa.sh,v 1.1 2006-08-25 06:58:56 gzins Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.1  2006/04/10 15:20:56  gzins
# Added
#
#*******************************************************************************

sclsvrServer -v 3 -noDate -noFileLine GETCAL "-objectName alf_PsA -mag 0.83 -diffRa 3600 -diffDec 1200 -band K -minMagRange 0.5 -maxMagRange 2 -ra 22:57:39.047 -dec -29:37:20.050 -baseMax 102.45 -wlen 2.2"
