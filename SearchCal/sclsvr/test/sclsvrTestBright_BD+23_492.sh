#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclsvrTestBright_BD+23_492.sh,v 1.1 2007-05-11 15:52:58 gzins Exp $"
#
# History
# -------
#*******************************************************************************

sclsvrServer -v 3 -noDate -noFileLine GETCAL "-objectName BD+23_492 -ra 03 42 45.95 -dec +24 16 16.50 -mag 7.4 -diffRa 3600 -diffDec 1200 -band K -minMagRange 5 -maxMagRange 8 -wlen 2.2 -baseMax 102.45"

#___oOo___
