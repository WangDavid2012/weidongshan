#!/bin/bash
#Bash script  program test,  made by www.embedclub.com Hanson
#while  do ... done

number=0
while [ $number -lt 10 ]
do
	echo $number
	number=`expr $number + 1`
done
