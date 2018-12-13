#!/bin/bash

for f in *.arow ; do
	FILE="${f%.*}"
	echo "${FILE}.arow"
	bash compiler.sh ${FILE}.arow
	echo "--------------------------------------------------------------------------------------------------------------------"
done
