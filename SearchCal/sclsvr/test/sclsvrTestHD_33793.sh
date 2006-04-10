#!/bin/bash 
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclsvrTestHD_33793.sh,v 1.1 2006-04-10 15:20:56 gzins Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
#*******************************************************************************

sclsvrServer -v 3 -noDate -noFileLine GETCAL "-objectName HD_33793 -mag 5 -radius 0 -band K -minMagRange 5.0 -maxMagRange 10.0 -ra 05:11:40.578 -dec -45:01:06.26 -baseMax 100 -wlen 2.2 -bright false"
