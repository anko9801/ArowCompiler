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

~/ArowCompiler/bin/dcc -o $FILE.ll $FILE.arow

if [ -e $FILE.ll ]; then
	llvm-link ../lib/lib.ll $FILE.ll -S -o $FILE.ll
	if [ -e $FILE.ll ]; then
		if [ -z $2 ]; then
			llc -o $FILE.s $FILE.ll -filetype=asm
			clang -o $FILE $FILE.s
		else
			llc -o $FILE-diffarch.s $FILE.ll -filetype=asm -mtriple=$2 #armv4t-unknown-linux-gnueabihf
		fi

		#rm $FILE.s
	fi
fi

