#!/bin/bash

if [ $# != 1 ]
then
    echo "Usage: $0 commandDefinitionFile.cdf"
    exit 1
fi

echo $0 is Verifying your Command Definition File [$1]

xmllint --schema ../config/cmdDefinitionFile.xsd  $1

