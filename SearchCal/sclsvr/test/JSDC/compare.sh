#!/bin/sh
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************

PREV=20140222
NEW=20140422

PREVCAT="jsdc-SVN$PREV.fits"
LASTCAT="jsdc-SVN$NEW.fits"

echo "copying final.fits to jsdc-SVN$NEW.fits"

cp final.fits output/$LASTCAT

# compare script does not work if catalogs are not in the current directory !
cd output/

../../../../sclcat/bin/sclcatCompareCat $PREVCAT $LASTCAT 

../../../../sclcat/bin/sclcatCompareCat JSDC_V1.fits $LASTCAT 

