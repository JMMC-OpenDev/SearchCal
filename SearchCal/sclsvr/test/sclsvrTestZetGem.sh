#!/bin/bash 
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclsvrTestZetGem.sh,v 1.1 2006-04-10 15:20:56 gzins Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
#*******************************************************************************

sclsvrServer -v 3 -noDate -noFileLine GETCAL "-objectName zet_Gem -mag 2.12 -diffRa 1800 -diffDec 600 -band K -minMagRange 1 -maxMagRange 3 -ra 07:04:06.5318 -dec 20:34:13.069 -baseMax 102.45 -wlen 2.2"
