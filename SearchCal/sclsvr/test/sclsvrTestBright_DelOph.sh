#!/bin/bash 
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclsvrTestBright_DelOph.sh,v 1.1 2006-08-25 06:58:56 gzins Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.1  2006/04/10 15:20:56  gzins
# Added
#
#*******************************************************************************

sclsvrServer -v 3 -noDate -noFileLine GETCAL "-objectName del_Oph -mag -1.26 -diffRa 3600 -diffDec 1200 -band K -minMagRange -1 -maxMagRange 2 -ra 16:14:20.740 -dec -03:41:39.564 -baseMax 102.45 -wlen 2.2"
