#!/bin/bash


#CONSTANTS
LS_STRING="ls -l"
COMMA_STRING_REGEX="^\,(\-?[0-9]*|\,*)$"
DOT_STRING_REGEX="^\.(\-?[0-9]*|\.*)$"

#TODO: Get system specific separator
#For now, assuming Unix /
separator="/"



#Check if directory is valid or not
ChkDir() {
	if [ -d "$1" ]; then
		return 0
	else
		return 1
	fi
}


#Check args for a path and cd to it
path=$1

if [ ! -d "$path" ]; then
	path=$(pwd)
fi

cd $path 2>/dev/null

echo "$PWD"


if ChkDir "$1" ; then
	echo "Yeah"
fi