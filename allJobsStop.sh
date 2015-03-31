#!/bin/bash

var="$(./jobCommander poll running)"
rem="Processes running in system"
var=${var#$rem}
i=0
for word in $var
do
    if [ $((i%2)) -eq 0 ];
	then
    		output="./jobCommander stop $word"
		eval $output	
	fi
	i=$((i+1))
done
