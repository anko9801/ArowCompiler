#!/bin/bash

FILE=$1
FILE="${FILE%.*}"

if [ -e $FILE.ll ]; then
	rm $FILE.ll
fi
if [ -e $FILE ]; then
	rm $FILE
fi

../bin/dcc -o $FILE.ll $FILE.arow
