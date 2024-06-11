#!/usr/bin/python
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************

#/**
# @file
# Generate a configuration file for batch querying using <cmdBatch> by mozaicing
# the whole sky in smaller boxes. Plus it generates an HTML map file to later
# browse results.
#
# @synopsis
# sclcatGenerateConfigByMozaicing <batchHeaderFile> <outputCompleteBatchFile> <outputHtmlMapFile>
# 
# @details
# This script generate the desired output file which is used for the calibrators
# research. It starts by inserting the given header file.
# Then, the sky is mozaiced in order to prepare each needed SearchCal requests.
# It also generate an HTML file with image mapping to provide downloadable
# result files to scientist.
# */

""" This script aims to generate a <cmdBatch> configuration file by mozaicing the whole sky
"""

import sys
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

raMinDeg = 0
raMin = raMinDeg * 60
raMaxDeg = 360
raMax = raMaxDeg * 60
decMinDeg = -90
decMin = decMinDeg * 60
decMaxDeg = 90
decMax = decMaxDeg * 60
imageXSize = 777 # Horizontal size of the 0-360 plotted area
imageXOffset = 68 # Horizontal offset from upper left to the 0-360 plotted area
imageYSize = 674 # Vertical size of the -90/90 plotted area
imageYOffset = 40 # Vertical offset from upper left to the -90/90 plotted area


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
        str += "objectName = toto\n"
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
    str += """WARNING : this page may take a while to load !\n"""
    str += """<br/>\n"""
    str += """<br/>\n"""
    str += """<img src="result/catalog.png" USEMAP="#boxes"/>\n"""
    str += """<map name="boxes">\n"""
    for box in boxList:
        ident = "box_RA%s_DEC%s"%(box[0], box[1])
        str += """<area shape="rect" coords="%d,%d,%d,%d" href="%s.vot"/>\n"""%(box[2], box[3], box[4], box[5], ident)
    str += """</map>\n"""
    str += """<br/>\n"""
    str += """<br/>\n"""
    str += """This catalog covers the sky from %d&deg; &le; RA &le; %d&deg; and %d&deg; &le; DEC &le; %d&deg;.<br/>\n"""%(raMinDeg, raMaxDeg, decMinDeg, decMaxDeg)
    return str

# Check parameter number
if len(sys.argv) != 4 :
    print 'Usage : sclcatGenerateConfigByMozaicing headerFilename outputFilename htmlFilename'
    sys.exit(1)
header = sys.argv[1]
headerFile = open(header)
output = sys.argv[2]
outputFile = open(output, "w")
html = sys.argv[3]
htmlFile = open(html, "w")

# Write date of creation
print "Generating configuration file into '%s'..."%outputFile.name

fileContent = "# File generated by sclcatGenerateConfigByMozaicing"
fileContent += "\n"

today=datetime.date.today()
fileContent += "# at %s"%today
fileContent += "\n"

# Append header file content
fileContent += headerFile.read()

raRange = 180 # 12 min
decRange = 60 # 1 degree
#boxes = computeSkyBoxes(raRange, decRange, 3, 2)
boxes=computeSkyBoxes(raRange, decRange)

fileContent+=writeConfigurationList(boxes)

# Write the resulting file content to disk
outputFile.write(fileContent)

# Generate HTML map file
print "Generating HTML map file into '%s'..."%htmlFile.name
htmlFile.write(writeHTMLMap(boxes))

print "DONE."

