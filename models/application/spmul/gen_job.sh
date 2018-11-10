#!/bin/bash

declare -a NROW=(1508065  2097152  4690002  5363260  16777216  23947347  50912018)
declare -a NNZ=(27090195  182082942  20316253  79023142  100663202  57708624   108109320)
NUM_SET=${#NNZ[@]}

NUM_THREADS=32

echo "" > sweepjob.sh

for (( i=0; i<${NUM_SET}; i++ ))
do   
    CONFIG="spmul_NROW"${NROW[i]}"_NNZ"${NNZ[i]}"_t"$NUM_THREADS
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

exit 0
