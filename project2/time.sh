#! /bin/bash

STARTTIME=$(date +%s)
	make
	gcc -Wall -o test1 test1.c -fopenmp -O2
	gcc -Wall -o test2 test2.c -fopenmp -O2
	for((i=0;i<10;i++)); do
		./test1 3000 10
		./test2 3000 10
		done
	ENDTIME=$(date +%s)
	echo "It takes $(($ENDTIME - $STARTTIME)) seconds to complete this task..."
