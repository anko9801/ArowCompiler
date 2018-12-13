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
	llvm-link $FILE.ll ../lib/lib.ll -S -o $FILE.ll
	if [ -e $FILE.ll ]; then
		if [ "$2" = "arm" ]; then
			llc -o $FILE.s $FILE.ll -filetype=asm -mtriple=armv4t-unknown-linux-gnueabihf
		else
			llc -o $FILE.s $FILE.ll -filetype=asm
			clang -o $FILE $FILE.s
		fi

		#rm $FILE.s
	fi
fi

