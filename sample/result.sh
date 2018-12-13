#!/bin/bash

for f in *.arow ; do
	FILE="${f%.*}"
	echo "$FILE.arow"
	cat $FILE.arow
	echo
	echo "result:"
	./${FILE}
	echo "--------------------------------------------------------------------------------------------------------------------"
done
