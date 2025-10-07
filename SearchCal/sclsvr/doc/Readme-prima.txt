#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************

#/**
# @file
# brief description of the procedure used to achieve a batched query for prima.
# */

* How to run the batch ?
Just type following command:
$ cmdBatch primaBatch

* How to build the batch ?
First generate the header using next command and cdf:
$ cmdCdfToTestScript sclsvrGETCAL.cdf

Then adjust:
command field with 'sclsvrServer GETCAL'
and default or optional values of the command

Finally include one section per request to be batched
eg:
[HD 183263]
file = HD 183263.vot
objectName = HD 183263
radius = 1.01
ra = 19:28:24
mag = 0.00
dec = 8:21:28
 
#___oOo___
