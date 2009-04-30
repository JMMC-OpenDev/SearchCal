#!/usr/bin/python
#******************************************************************************
# JMMC project
#
#
#************************************************************************
""" This script aims to return the computed orbit according formuale given by H. Beust"""

import os
import sys
import math
from optparse import OptionParser

def getRadius(pmra, pmdec, timespan):
    """ Returns radius to the given proper motion and timespan
    -  pmra    mas/yr (*cos(delta))
    -  pmdec   mas/yr
    -  timespan yr
    """
    vitsky=math.sqrt(pmra**2+pmdec**2)/1000.0/60.0
    dkal=vitsky*timespan+1.0
    return dkal

def latexValue(number,dim=2):
    """ Returns the best frm that we can accept as latex number value"""
    s=str(number)
    # shorten too long decimals
    if len(s)>5:
        format="%." + "%d" % dim + "g"
        s=format%number
    # shorten too long integers
    if not( "." in s ):
        s="%g"%number
    if "e" in s:
        array=s.split("e")
        format="\ee{%." + "%d" % (dim-1) + "f}{%d}"
        s=format%(float(array[0]),int(array[1]))
    return s
    


if __name__ == '__main__':
    usage="""usage: %prog [options] STAR_MASS STAR_DIST PL_MASS PL_SEMIAXIS PL_ECC [PL_MASS PL_SEMIAXIS PL_ECC] """
    parser = OptionParser(usage=usage)
    parser.add_option("-t", "--test", dest="test", metavar="OUT",  
            help="Test latexValue function")

    (options, args) = parser.parse_args()
  
    if options.test:
        idx=0;
        for i in [105,1050,10500,105000,1050000, 1.2,1.00,1.0001,1.0001001, 0.1, .12,.000001,.00000123,.00000167]:
            idx=idx+1
            print(idx)
            print("inputValue: ")
            print(i)
            print("latexValue: " + latexValue(i))
            print("")
            print("")
        sys.exit(0)

    if len(args) < 5:
        parser.print_help(file=sys.stderr)
        sys.exit(1)

    St_Mass=float(args[0])     
    St_Dist=float(args[1])     
    St_Parax=1/St_Dist

    PlArgs=args[2:]
    if len(PlArgs)%3 != 0 :
        parser.print_help()
        sys.exit(1)

    o=0
    for i in range(len(PlArgs)/3):
        Pl_Mass=float(PlArgs[3*i+0])
        Pl_SemiAxis=float(PlArgs[3*i+1])
        Pl_Ecc=float(PlArgs[3*i+2])
        o+=Pl_Mass/St_Mass*1/1.047355*St_Parax*Pl_SemiAxis*(1+Pl_Ecc)
   

    print ("""<orbit latex="%s">%.2E</orbit>""" % (latexValue(o),o))
