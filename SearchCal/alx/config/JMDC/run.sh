#!/bin/bash
source env.sh

# JMDC used by JSDC2 (2017), renamed columns to be used with latest idl code:
#JMDC="JMDC_final_lddUpdated_ren.fits"

# 2024 re-processed 2017 JMDC subset (1238 rows)
JMDC="JMDC_201703_final_ren_lddUpdated.fits"
JMDC="JMDC_final_lddUpdated_ren_1238_rows.fits"
#JMDC="DataBaseUsed_jsdc_17.fits"

JMDC="JMDC_202406_final_lddUpdated.fits"


# JMDC 2023.02 for JSDC3 (2023 ?):
#JMDC="JMDC_OK_Jan2023-ascii_final_lddUpdated.fits"

# filtered groups (more recent measurement per object):
#JMDC="JMDC_OK_Jan2023-ascii_final_lddUpdated_sorted_groupID_bibcode.fits"

#JMDC="JMDC_OK_Jan2023-ascii_final_lddUpdated_filtered_groupID.fits"

echo "Using JMDC: '${JMDC}'"

OPTS="/NOPAUSE,/NOCATALOG"
OPTS="/NOCATALOG"
#OPTS="/TEST"

set -eux;

echo "make_jsdc_script_simple,\"$JMDC\",$OPTS"
gdl -e "make_jsdc_script_simple,\"$JMDC\",$OPTS " | tee run.log

