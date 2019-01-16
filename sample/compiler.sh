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

if [ -e $FILE.ll ]; then
	if [ -z $2 ]; then
		llc -o $FILE.o $FILE.ll -filetype=obj
		gcc $FILE.o lib.o -o $FILE
		rm $FILE.o
	else
		llc -o $FILE-diffarch.o $FILE.ll -filetype=obj -mtriple=$2-unknown-linux-gnueabihf
		scp ./$FILE-diffarch.o pi@raspberrypi.local:~/GPIO/
	fi
fi
