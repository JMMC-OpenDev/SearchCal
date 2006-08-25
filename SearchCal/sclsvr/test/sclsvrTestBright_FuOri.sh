#!/bin/bash 
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclsvrTestBright_FuOri.sh,v 1.1 2006-08-25 06:58:56 gzins Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.1  2006/04/10 15:20:56  gzins
# Added
#
#*******************************************************************************

sclsvrServer -v 3 -noDate -noFileLine GETCAL "-objectName FU_Ori -mag 4.65 -diffRa 1800 -diffDec 600 -band K -minMagRange 3.5 -maxMagRange 5.5 -ra 05:45:22.362 -dec 09:04:12.310 -baseMax 102.45 -wlen 2.2"
