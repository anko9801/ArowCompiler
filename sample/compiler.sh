#!/bin/bash

FILE=$1
FILE=${FILE:0:-3}

cat $FILE.dc
echo

~/ArowCompiler/bin/dcc -o $FILE.ll $FILE.dc

if [ -e $FILE.ll ]; then
	llvm-link $FILE.ll ../lib/printnum.ll -S -o $FILE.ll
	llvm-link $FILE.ll ../lib/sleep.ll -S -o $FILE.ll
	if [ -e $FILE.ll ]; then
		llc -o $FILE.s $FILE.ll
		clang -o $FILE $FILE.s

		rm $FILE.s
	fi
fi

