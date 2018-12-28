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

../bin/dcc -o $FILE.ll $FILE.arow

if [ -e $FILE.ll ]; then
	llvm-link $FILE.ll ../lib/lib.ll -S -o $FILE.ll
	if [ -e $FILE.ll ]; then
		if [ -z $2 ]; then
			llc -o $FILE.s $FILE.ll -filetype=asm
			gcc -o $FILE $FILE.s
		else
			llc -o $FILE-diffarch.o $FILE.ll -filetype=obj -mtriple=$2 #armv4t-unknown-linux-gnueabihf
			gcc -c -o $FILE.o $FILE.o ../lib/blink.o
			gcc -o $FILE $FILE.o
		fi

		#rm $FILE.s
	fi
fi

