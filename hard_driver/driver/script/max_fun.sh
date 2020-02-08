#!/bin/bash
#Bash script program, made by www.embedclub.com Hanson
#fun() take two parameters
#max(): return the max value.

E_PARAM_ERR=-1
EQUAL=0

max()
{
	if [ -z "$2" ] ; then
		return $E_PARAM_ERR
	fi

	if [ "$1" -eq "$2" ] ; then
		return $EQUAL
	else
		if [ "$1" -gt "$2" ]; then
			return $1
		else
			return $2
		fi
	fi
}

#call max funtion with two params
max $1 $2
#max 20 40
return_val=$?

if [ "$return_val" -eq $E_PARAM_ERR ] ; then
	echo "Need to pass two params to the function"
elif [ "$return_val" -eq $EQUAL ] ; then
	echo "The two values are equal."
else
	echo "The larger of the two values is $return_val."
fi

exit 0
