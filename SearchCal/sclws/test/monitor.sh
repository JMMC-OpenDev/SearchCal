#!/bin/sh
COMMAND="$1"
#Print command with headers.
ps -A -o vsize,rss,minflt,majflt,cmd | grep -e "$COMMAND" -e CMD | grep -v grep | grep -v "$0"
while (true)
do
	sleep 5 
	#Print command without headers.
	ps -A -o vsize,rss,minflt,majflt,cmd | grep -e "$COMMAND" | grep -v grep | grep -v "$0"
done
