#!/bin/bash

FILE=$1
FILE="${FILE%.*}"

cat $FILE.arow
echo

if [ -e $FILE.ll ]; then
	rm $FILE.ll
fi
if [ -e $FILE ]; then
	rm $FILE
fi

../bin/dcc -o $FILE.ll $FILE.arow -n

if [ -e $FILE.ll ]; then
	if [ -z $2 ]; then
		llc -o $FILE.s $FILE.ll -filetype=asm
		gcc -o $FILE $FILE.s
		rm $FILE.s
	else
		llc -o $FILE-diffarch.s $FILE.ll -filetype=asm -mtriple=$2-unknown-linux-gnueabihf
		scp ./$FILE-diffarch.s pi@raspberrypi.local:~/GPIO/
	fi
fi
