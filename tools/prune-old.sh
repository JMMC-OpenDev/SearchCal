#!/bin/bash
# list files older than 14 days:
CMD="find . -mindepth 1 -mtime +14 -name '*.dat' -ls" 

echo "CMD: $CMD"
eval $CMD

echo "use '-delete' to remove such files"

