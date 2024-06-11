#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************
# REMARKS
#    Adapted from the VLT/ALMA program
#
#-------------------------------------------------------------------------------

#/**
# @file
# Prog used by tatTestDriver to spawn bg processes
#
# @synopsis
#   tatTestSpawner arg .....
# 
# @details
# This program creates a new process group id, so that all its descendent
# processes will be in this process group id.\n
# In this way it is easy to kill all the generated child processes.
#   
# All the given arguments are considered as a command string to be esecuted
# as a background task.
#
# When the command has been spawned, the process esit with SUCCESS (0)
#
# */

id process group set

eval exec $argv &

exit 0

#
# ___oOo___
