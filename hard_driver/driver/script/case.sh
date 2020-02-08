#!/bin/bash
#Bash script  program test,  made by www.embedclub.com Hanson
#case

echo "Enter A,B,C"
read letter
case $letter in
	A|a) echo "You entered A.";;
	B|b) echo "You entered B.";;
	C|c) echo "You entered C.";;
	*)  echo "You entered not a,b,c"
esac
