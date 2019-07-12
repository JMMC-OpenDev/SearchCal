#! /bin/bash
if test $# -ne 1
then
        echo usage: $0 NAME
	echo where NAME is the xls version of the JMDC.
        exit 1
fi
NAME=`basename $1 .xls`
##FLAGS='-stderr /dev/null'
##use openoffice to convert xls in csv
LANG=en_US # force US locale to format numerical values
soffice --headless --convert-to csv --outdir /tmp ${NAME}.xls # JMDC.xls
if [[ ! -e /tmp/${NAME}.csv ]]; then echo "CSV conversion not done -- openoffice probably active on this session (close all openoffice windows), or filename error" ; exit; fi
#fix non-ascii characters:
iconv -c -f utf-8 -t ascii -o /tmp/${NAME}-ascii.csv /tmp/${NAME}.csv

echo "JMDC.csv output: /tmp/${NAME}-ascii.csv"
