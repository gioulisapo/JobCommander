#!/bin/bash

for i in ${*}
	do
	while read line           
		do           
   		output="./jobCommander issuejob $line"
		eval $output  		         
		done <$i
	done
echo
	