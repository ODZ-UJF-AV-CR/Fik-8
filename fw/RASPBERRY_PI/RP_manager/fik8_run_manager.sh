#!/bin/bash

frameCount=$1
frameTime=$2
outDir=$3
i=1

while [ 1 ]
	do
		outFile=$outDir/data_${i}.clog
		#echo $frameCount $frameTime $outFile
		LD_LIBRARY_PATH=. /home/pi/Documents/Fik8/scripts/RP_manager/fik8 $frameCount $frameTime $outFile
		((i=i+1))
	done
