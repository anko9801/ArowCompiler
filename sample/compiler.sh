#!/bin/bash

FILE=$1
FILE=${FILE:0:-5}

cat $FILE.arow
echo

~/ArowCompiler/bin/dcc -o $FILE.ll $FILE.arow

if [ -e $FILE.ll ]; then
	llvm-link $FILE.ll ../lib/lib.ll -S -o $FILE.ll
	if [ -e $FILE.ll ]; then
		llc -o $FILE.s $FILE.ll
		clang -o $FILE $FILE.s

		rm $FILE.s
	fi
fi

