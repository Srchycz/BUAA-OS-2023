#!/bin/bash

if [ $# -eq 1 ];
then
    # Your code here. (1/4)
    grep -E "^[0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2} (WARN)|(ERROR) " $1 >bug.txt

else
    case $2 in
    "--latest")
        # Your code here. (2/4)
	tail --line=5 $1
    ;;
    "--find")
        # Your code here. (3/4)
	grep $3 $1 > $3.txt
    ;;
    "--diff")
        # Your code here. (4/4)
	diff $1 $3 > /dev/null
	if [ $? -gt 0 ]
	then
		echo different
	else
		echo same
	fi	
    ;;
    esac
fi
