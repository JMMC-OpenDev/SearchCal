#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclsvrTestFaint_HD_111688.sh,v 1.1 2006-08-25 06:58:10 gzins Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
#*******************************************************************************

sclsvrServer -v 3 -noDate -noFileLine GETCAL "-objectName HD_111688 -ra 12 51 58.53 -dec -63 08 31.90 -mag 8.2 -band K -minMagRange 5 -maxMagRange 12 -wlen 2.2 -baseMax 102.45 -bright false"

#___oOo___
