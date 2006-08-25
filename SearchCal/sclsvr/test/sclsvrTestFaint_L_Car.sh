#!/bin/bash 
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclsvrTestFaint_L_Car.sh,v 1.1 2006-08-25 06:58:10 gzins Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.1  2006/04/19 12:10:09  gzins
# Added
#
# Revision 1.1  2006/04/10 15:20:56  gzins
# Added
#
#*******************************************************************************

sclsvrServer -v 3 -noDate -noFileLine GETCAL "-objectName l_Car -ra 09:45:14.811 -dec -62:30:28.451 -mag 6.5 -band K -minMagRange 5.5 -maxMagRange 7.5 -wlen 2.2 -baseMax 130.2306 -bright false"

#___oOo___
