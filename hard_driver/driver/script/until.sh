#!/bin/bash
#Bash script  program test,  made by www.embedclub.com Hanson
#until  do ... done

number=0
until [ $number -gt 10 ] ; do
	echo $number
	number=`expr $number + 1`
done

