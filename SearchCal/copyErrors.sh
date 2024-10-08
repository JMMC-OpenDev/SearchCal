#!/bin/bash

find . -name "errors"

echo "MCSROOT: ${MCSROOT}"

cp -v ./alx/errors/*    ${MCSROOT}/errors/
cp -v ./vobs/errors/*   ${MCSROOT}/errors/
cp -v ./simcli/errors/* ${MCSROOT}/errors/
cp -v ./sclsvr/errors/* ${MCSROOT}/errors/
cp -v ./sclws/errors/*  ${MCSROOT}/errors/

