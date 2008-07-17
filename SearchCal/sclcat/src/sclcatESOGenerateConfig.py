#!/usr/bin/python
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclcatESOGenerateConfig.py,v 1.2 2008-07-17 15:17:38 lafrasse Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.1  2008/07/11 12:56:55  lafrasse
# Added ESO Calibrator catalog generation scripts.
#
#*******************************************************************************

#/**
# @file
# Generate the configuration file for the ESO calibrators research. 
#
# @synopsis
# sclcatESOGenerateConfig
# 
# @details
# This script generate the sclcatESO.cfg file which is used for the calibrators
# research. It starts by inserting the static header file sclcatESOHeader.cfg .
# Then, the sky is mosaiced in order to prepare each needed SearchCal requests.
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

def computeSkyBoxes(raBoxSize, decBoxSize, nbOfRaBoxes = 0, nbOfDecBoxes = 0):
    """ Returns a list of coordinates of boxes of the given size
    -  raBoxSize    arcmin
    -  decBoxSize   arcmin
    -  nbOfRaBoxes  Desired number of RA boxes (0 for the whole sky, by default)
    -  nbOfDecBoxes Desired number of DEC boxes (0 for the whole sky, by default)
    """
    # Result list
    boxList = []
    # RA range computation
    raMin = 0
    raMax = 360 * 60
    if nbOfRaBoxes > 0:
        raMin2 = ((raMin + raMax) / 2) - ((nbOfRaBoxes / 2.0) * raBoxSize)
        raMax2 = ((raMin + raMax) / 2) + ((nbOfRaBoxes / 2.0) * raBoxSize)
        raMin = int(raMin2)
        raMax = int(raMax2)
    nbOfRaBoxes = (raMax - raMin) / raBoxSize
    #print "[%d < ra < %d] / %d = %d"%(raMin, raMax, raBoxSize, nbOfRaBoxes)
    # DEC range computation
    decMin = -90 * 60
    decMax = 90 * 60
    if nbOfDecBoxes > 0:
        decMin2 = ((decMin + decMax) / 2) - ((nbOfDecBoxes / 2.0) * decBoxSize)
        decMax2 = ((decMin + decMax) / 2) + ((nbOfDecBoxes / 2.0) * decBoxSize)
        decMin = int(decMin2)
        decMax = int(decMax2)
    nbOfDecBoxes = (decMax - decMin) / decBoxSize
    #print "[%d < dec < %d] / %d = %d"%(decMin, decMax, decBoxSize, nbOfDecBoxes)
    print "Total number of queries: %d"%(nbOfRaBoxes * nbOfDecBoxes)
    # Generate coordinates list
    for ra in range(raMin, raMax, raBoxSize):
        for dec in range(decMin, decMax, decBoxSize):
            #print "Adding [%s , %s]"%(ArcmintoHMS(ra), ArcmintoDMS(dec))
            boxList.append([ArcmintoHMS(ra), ArcmintoDMS(dec)])
    return boxList

def writeConfigurationList(boxList):
    """ Write SerachCal configuration list with coordinates of boxes of the given size
    -  boxList list of [ra, dec]
    """
    str = ""
    i = 0
    for box in boxList:
        ident = "box_RA%s_DEC%s"%(box[0], box[1])
        str += "[%s]\n"%ident
        str += "file = %s.vot\n"%ident
        str += "objectName = %s\n"%ident
        str += "file = %s.vot\n"%ident
        str += "diffRa = %d\n"%raRange
        str += "diffDec = %d\n"%decRange
        str += "ra = %s\n"%box[0]
        str += "dec = %s\n"%box[1]
        str += "mag = 6\n"
        str += "\n"
    return str

headerFile = open("../config/sclcatESOHeader.cfg")
contentFile = open("../tmp/command.dat")
tailFile = "../tmp/tailBatch.cfg"
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
raRange = 360 # 24 min
decRange = 120 # 3 degrees
#boxes=computeSkyBoxes(raRange, decRange, 3, 3)
boxes=computeSkyBoxes(raRange, decRange)

fileContent+=writeConfigurationList(boxes)

# Write the resulting file content to disk
sclcatESO.write(fileContent)

print "Done."

