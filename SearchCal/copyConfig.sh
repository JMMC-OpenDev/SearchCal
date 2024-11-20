#!/bin/bash

find . -name "config"

echo "MCSROOT: ${MCSROOT}"

cp -v ./alx/config/*    ${MCSROOT}/config/
cp -v ./vobs/config/*   ${MCSROOT}/config/
cp -v ./simcli/config/* ${MCSROOT}/config/
cp -v ./sclsvr/config/* ${MCSROOT}/config/
cp -v ./sclws/config/*  ${MCSROOT}/config/

echo "unzip files..."
cd ${MCSROOT}/config/
gzip -v -d *.gz


