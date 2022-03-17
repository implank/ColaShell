#!/bin/bash
gcc -Wall $1 -o test 2> warning.txt
a=$?
grep warning:  warning.txt | sed 's/warning: //'  > result.txt
if [ $a -eq 0 ]
then
	b=1
	while [ $b -le $2 ]
	do
		echo $b > input
		./test 0<input 1>>result.txt
		b=$[$b+1]
	done
fi
pwd >> result.txt
