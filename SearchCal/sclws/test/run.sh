#!/bin/bash
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************
source env.sh

export SCLWS_PORT_NB=6666

echo "SCLWS CMD  = $SCLWS_CMD"
echo "SCLWS PORT = $SCLWS_PORT_NB"

$SCLWS_CMD &> run.log &

