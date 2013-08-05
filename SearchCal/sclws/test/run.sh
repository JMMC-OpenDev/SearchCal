#!/bin/bash
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************
source env.sh

export SCLWS_PORT_NB=6666

# define dev flag:
export VOBS_DEV_FLAG="true"

echo "SCLWS CMD  = $SCLWS_CMD"
echo "SCLWS PORT = $SCLWS_PORT_NB"
echo "VOBS_DEV_FLAG = $VOBS_DEV_FLAG"

$SCLWS_CMD &> run.log &

