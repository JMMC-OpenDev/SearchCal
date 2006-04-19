#!/bin/bash 
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclsvrTestL_Car.sh,v 1.1 2006-04-19 12:10:09 gzins Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.1  2006/04/10 15:20:56  gzins
# Added
#
#*******************************************************************************

sclsvrServer -v 3 -noDate -noFileLine GETCAL "-objectName l_Car -mag 6.5 -diffRa 1800 -diffDec 600 -radius 0 -band K -minMagRange 5.5 -maxMagRange 7.5 -ra 09:45:14.811 -dec -62:30:28.451 -baseMax 130.2306 -wlen 2.2 -bright false"
