#!/bin/bash 
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclsvrTestBright_BetCap.sh,v 1.1 2006-08-25 06:58:56 gzins Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.1  2006/04/10 15:20:56  gzins
# Added
#
#*******************************************************************************

sclsvrServer -v 3 -noDate -noFileLine GETCAL "-objectName Bet_Cap -mag 0.96 -diffRa 3600 -diffDec 1200 -band K -minMagRange 1 -maxMagRange 2 -ra 20:21:00.6757 -dec -14:46:52.922 -baseMax 102.45 -wlen 2.2"
