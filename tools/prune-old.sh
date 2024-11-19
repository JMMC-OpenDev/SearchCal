#!/bin/bash
# list files older than 14 days:
CMD="find . -mindepth 1 -mtime +14 -ls" 

echo "CMD: $CMD"
eval $CMD

echo "use arg '-delete' to remove such files"


