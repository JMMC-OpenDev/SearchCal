#!/usr/bin/python
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclcatESOGenerateConfig.py,v 1.6 2008-11-26 10:22:56 lafrasse Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.5  2008/11/04 09:16:18  lafrasse
# Enhanced code documentation and lisibility.
# Removed uneeded generated parameters.
#
# Revision 1.4  2008/10/03 12:44:12  lafrasse
# Added preliminary support for HTML Map.
#
# Revision 1.3  2008/07/17 15:23:22  lafrasse
# Removed unneeded files open.
#
# Revision 1.2  2008/07/17 15:17:38  lafrasse
# Added conversion functions (arcmin to HMS & DMS) from ESO.
# Added parameters to computeSkyBoxes() to restrict queried sky zone (for test
# purpose).
# Polished code source.
#
# Revision 1.1  2008/07/11 12:56:55  lafrasse
# Added ESO Calibrator catalog generation scripts.
#
#*******************************************************************************

#/**
# @file
# Generate the configuration file for the ESO calibrators research, and the HTML
# map file to browse results. 
#
# @synopsis
# sclcatESOGenerateConfig
# 
# @details
# This script generate the sclcatESO.cfg file which is used for the calibrators
# research. It starts by inserting the static header file sclcatESOHeader.cfg .
# Then, the sky is mosaiced in order to prepare each needed SearchCal requests.
# It also generate an HTML file with image mapping to provide downloadable
# result files to scientist.
#
# @usedfiles
# @filename <b>../config/sclcatESOHeader.cfg :</b> SearchCal parameters file
# description which is the future header of the sclcatESO.cfg file
# @filename <b>../tmp/tailBatch.cfg :</b> temporary file for calling SearchCal
# by batch for several objects. It is the body of the sclcatESO.cfg
# @filename <b>../config/sclcatESO.cfg :</b>  ESO configuration file
# (sclcatESOHeader.cfg + ../tmp/tailBatch)
# @filename <b>../tmp/command.dat :</b>  temporary file where are store
# generated body of the final sclcatESO.cfg file
# @filename <b>../config/sclcatESOMap.html :</b> generated HTML Map file
#
# */

""" This script aims to generate the ESO calibrator catalog configuration file
"""

import datetime
import math

################################################################################
# From : http://www.sc.eso.org/~sbaumont/VLTscripts/HTML/astroTools.py
################################################################################
# Convert RA (deg) to H.M.S:
def deg2HMS( RAin ):

   if(RAin<0):
      sign = -1
      ra   = -RAin
   else:
      sign = 1
      ra   = RAin

   h = int( ra/15. )
   ra -= h*15.
   m = int( ra*4.)
   ra -= m/4.
   s = ra*240.

   if(sign == -1):
      out = '-%02d:%02d:%06.3f'%(h,m,s)
   else: out = '+%02d:%02d:%06.3f'%(h,m,s)
   
   return out
   
# Convert Decl. (deg) to D.M.S:
def deg2DMS( Decin ):

   if(Decin<0):
      sign = -1
      dec  = -Decin
   else:
      sign = 1
      dec  = Decin

   d = int( dec )
   dec -= d
   dec *= 100.
   m = int( dec*3./5. )
   dec -= m*5./3.
   s = dec*180./5.

   if(sign == -1):
      out = '-%02d:%02d:%06.3f'%(d,m,s)
   else: out = '+%02d:%02d:%06.3f'%(d,m,s)

   return out
################################################################################

raMin = 0
raMax = 360 * 60
decMin = -90 * 60
decMax = 90 * 60
imageXSize = 1113
imageXOffset = 66
imageYSize = 594
imageYOffset = 17


def ArcmintoHMS(arcmin):
    """ Converts RA arcmin to Hours Minutes Seconds h:m:s ."""
    degrees = arcmin / 60.0
    hms = deg2HMS(degrees)
    return hms

def ArcmintoDMS(arcmin):
    """ Converts DEC arcmin to Degrees Minutes Seconds d:m:s ."""
    degrees = arcmin / 60.0
    dms = deg2DMS(degrees)
    return dms

def ArcmintoXPixel(arcmin, imageXSize, leftOffset):
    """ Converts RA arcmin to X pixel coordinates."""
    global raMax
    xPixel = ((arcmin * imageXSize) / raMax) + leftOffset
    return xPixel

def ArcmintoYPixel(arcmin, imageYSize, upwardOffset):
    """ Converts DEC arcmin to Y pixel coordinates."""
    global decMax
    yPixel = ((((-1 * arcmin) + decMax) * imageYSize) / (2 * decMax)) + upwardOffset
    return yPixel

def computeSkyBoxes(raBoxSize, decBoxSize, nbOfRaBoxes = 0, nbOfDecBoxes = 0):
    """ Returns a list of coordinates of boxes of the given size
    -  raBoxSize    arcmin
    -  decBoxSize   arcmin
    -  nbOfRaBoxes  Desired number of RA boxes (0 for the whole sky, by default)
    -  nbOfDecBoxes Desired number of DEC boxes (0 for the whole sky, by default)
    """
    global raMin
    global raMax
    global decMin
    global decMax
    global imageXSize
    global imageYSize
    global imageXOffset
    global imageYOffset
    # Result list
    boxList = []

    # RA range computation
    if nbOfRaBoxes > 0: # If not running on the whole sky
        # Compute the seeked RA region min & max coordinates according to:
        #  - the number of boxes wanted;
        #  - the size of those boxes.
        raRangeCenter = (raMin + raMax) / 2
        raHalfSize = (nbOfRaBoxes / 2.0) * raBoxSize
        raMin = int(raRangeCenter - raHalfSize)
        raMax = int(raRangeCenter + raHalfSize)
    else:
        # Compute the total number of RA boxes to cover the whole sky according to its RA size
        nbOfRaBoxes = (raMax - raMin) / raBoxSize

    #print "[%d < ra < %d] / %d = %d"%(raMin, raMax, raBoxSize, nbOfRaBoxes)

    # DEC range computation
    if nbOfDecBoxes > 0: # If not running on the whole sky
        # Compute the seeked DEC region min & max coordinates according to:
        #  - the number of boxes wanted;
        #  - the size of those boxes.
        decRangeCenter = (decMin + decMax) / 2
        decHalfSize = (nbOfDecBoxes / 2.0) * decBoxSize
        decMin = int(decRangeCenter - decHalfSize)
        decMax = int(decRangeCenter + decHalfSize)
    else:
        # Compute the total number of DEC boxes to cover the whole sky according to its DEC size
        nbOfDecBoxes = (decMax - decMin) / decBoxSize

    # Compute the number of pixel for each box for the HTML map
    raBoxPixelSize = imageXSize / nbOfRaBoxes
    decBoxPixelSize = imageYSize / nbOfDecBoxes

    #print "[%d < dec < %d] / %d = %d"%(decMin, decMax, decBoxSize, nbOfDecBoxes)

    print "Total number of queries: %d"%(nbOfRaBoxes * nbOfDecBoxes)

    # Generate coordinates list
    for ra in range(raMin, raMax, raBoxSize):
        for dec in range(decMin, decMax, decBoxSize):
            #print "Adding [%s , %s]"%(ArcmintoHMS(ra), ArcmintoDMS(dec))
            raPX = ArcmintoXPixel(ra, imageXSize, imageXOffset)
            decPX = ArcmintoYPixel(dec, imageYSize, imageYOffset)
            #print "RADEC[%s , %s] => PIXEL[%d , %d]"%(ArcmintoHMS(ra), ArcmintoDMS(dec), raPX, decPX)
            upperLeftX = int(raPX - (raBoxPixelSize / 2))
            upperLeftY = int(decPX + (decBoxPixelSize / 2))
            lowerRightX = int(raPX + (raBoxPixelSize / 2))
            lowerRightY = int(decPX - (decBoxPixelSize / 2))
            boxList.append([ArcmintoHMS(ra), ArcmintoDMS(dec), upperLeftX, upperLeftY, lowerRightX, lowerRightY])
    return boxList

def writeConfigurationList(boxList):
    """ Write SerachCal configuration list with coordinates of boxes of the given size
    -  boxList list of [ra, dec]
    """
    str = ""
    for box in boxList:
        ident = "box_RA%s_DEC%s"%(box[0], box[1])
        str += "[%s]\n"%ident
        str += "file = %s.vot\n"%ident
        str += "objectName = %s\n"%ident
        str += "diffRa = %d\n"%raRange
        str += "diffDec = %d\n"%decRange
        str += "ra = %s\n"%box[0]
        str += "dec = %s\n"%box[1]
        str += "\n"
    return str

def writeHTMLMap(boxList):
    """ Write SerachCal HTML map with coordinates of boxes of the given size
    -  boxList list of [ra, dec]
    """
    str = ""
    str += """<html>\n"""
    str += """<body>\n"""
    str += """<br/>\n"""
    str += """ATTENTION : le chargement de cette page est long, soyez patients !\n"""
    str += """<br/>\n"""
    str += """<br/>\n"""
    str += """<img src="catalog.png" USEMAP="#boxes"/>\n"""
    str += """<map name="boxes">\n"""
    for box in boxList:
        ident = "box_RA%s_DEC%s"%(box[0], box[1])
        str += """<area shape="rect" coords="%d,%d,%d,%d" href="../%s.vot"/>\n"""%(box[2], box[3], box[4], box[5], ident)
    str += "</map>\n"
    str += """<br/>\n"""
    str += """<a href="catalog.vot">Catalogue complet en VOTable</a> - environ 700Mo<br/>\n"""
    str += """<a href="filtre.vot">Catalogue filtre en VOTable</a> - environ 200Mo<br/>\n"""
    str += """<a href="catalog.fits">Catalogue complet en FITS</a> - environ 350Mo<br/>\n"""
    str += """<a href="filtre.fits">Catalogue filtre en FITS</a> - environ 50Mo<br/>\n"""
    str += """<br/>\n"""
    str += """<br/>\n"""
    str += """Utiliser le "clic-droit - Enregistrer sous..." pour telecharger les donnees.\n"""
    str += """</body>\n"""
    str += """</html>"""
    return str

headerFile = open("../config/sclcatESOHeader.cfg")
sclcatESO = open("../config/sclcatESO.cfg", "w")

print "Generate configuration file into '%s'..."%sclcatESO.name

# Write date of creation

fileContent = "# File generated by sclcatESOGenerateConfig.sh"
fileContent += "\n"

today=datetime.date.today()
fileContent += "# at %s"%today
fileContent += "\n"

# Append header file content
fileContent += headerFile.read()

#raRange = 3600 # arcmin -> 4 hour
#decRange = 1800 # arcmin -> 30 degrees
#raRange = 360 # 24 min
raRange = 180 # 12 min
# decRange = 120 # 2 degrees
decRange = 60 # 1 degree
boxes = computeSkyBoxes(raRange, decRange, 3, 2)
#boxes=computeSkyBoxes(raRange, decRange)

fileContent+=writeConfigurationList(boxes)

# Write the resulting file content to disk
sclcatESO.write(fileContent)

# Generate HTML map file
htmlMapFile = open("../config/sclcatESOMap.html", "w")
htmlMapFile.write(writeHTMLMap(boxes))

print "Done."

