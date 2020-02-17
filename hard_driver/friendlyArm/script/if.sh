#!/bin/bash
#Bash script  program test,  made by www.embedclub.com Hanson
#if elif else fi

echo 'word1:'
read word1
echo 'word2:'
read word2
echo 'word3:'
read word3
if [ "$word1" = "$word2" -a "$word2" = "$word3" ]; then
	echo 'match:words1,2&3'
elif [ "$word1" = "$word2" ]; then
	echo 'match:words1&2'
elif [ "$word1" = "$word3" ]; then
	echo 'match:words1&3'
elif [ "$word2" = "$word3" ]; then
	echo 'match:words2&3'
else
	echo 'no match'
fi