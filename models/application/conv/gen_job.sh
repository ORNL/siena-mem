#!/bin/bash

echo "" > sweepjob.sh
NUM_THREADS=32
x=0
for N1 in 2000 4000 6000 8000 10000 12000
do  
    for N2 in 3
    do
	CONFIG="conv_N"$N1"_F"$N2"_t"$NUM_THREADS
	FILENAME=$CONFIG".aspen"
	
	#for MACHINE in V_HMC1024_DDR8 V_HMC128_DDR8 V_HMC2048_DDR8 V_HMC256_DDR8 V_HMC4096_DDR8 V_HMC512_DDR8  
	for MACHINE in V_HBM1024_PCM8 V_HBM128_PCM8 V_HBM2048_PCM8 V_HBM256_PCM8 V_HBM4096_PCM8 V_HBM512_PCM8 
	do
	    echo "./main_release "$FILENAME"  ../machinemodels/"$MACHINE"/hms.aspen > out_"$CONFIG"_"$MACHINE" 2>&1 & " >> sweepjob.sh
	    x=$(( x + 1 ))
	    if [ "$x" -gt "9" ];
	    then
		x=0
		echo "" >> sweepjob.sh
		echo "wait" >> sweepjob.sh
		echo "" >> sweepjob.sh
	    fi
	done
    done
done    
echo "" >> sweepjob.sh
echo "wait" >> sweepjob.sh
echo "" >> sweepjob.sh
exit 0
