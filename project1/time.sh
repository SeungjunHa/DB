#! /bin/bash

STARTTIME=$(date +%s)
	for((i=0;i<100;i++)); do
		./project1 input1.txt 16
		./project1 input2.txt 16
		./project1 input3.txt 16
		./project1 input4.txt 16
		./project1 input5.txt 16
		./project1 input6.txt 16
		./project1 input7.txt 16
		./project1 input8.txt 16
		./project1 input9.txt 16
		./project1 input10.txt 16
		./project1 input11.txt 16
		./project1 input12.txt 16
		./project1 input13.txt 16
		./project1 input14.txt 16
		done
	ENDTIME=$(date +%s)
	echo "It takes $(($ENDTIME - $STARTTIME)) seconds to complete this task..."
