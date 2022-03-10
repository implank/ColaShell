#!bin/bash
#First you can use grep (-n) to find the number of lines of string.
grep $2 -n $1 | awk -F: '{print $1}' > $3
#Then you can use awk to separate the answer.
