#!/bin/bash

echo "" >  sweepjob.sh

NUM_THREADS=32
x=1
for N in 1000 2000 3000 4000 5000 6000 7000 8000
do   
    CONFIG="laplace2d_N"$N"_t"$NUM_THREADS
    FILENAME=$CONFIG".aspen"
    #for MACHINE in V_HMC128_DDR8 V_HMC256_DDR8 V_HMC512_DDR8 V_HMC1024_DDR8 V_HMC2048_DDR8 V_HMC4096_DDR8  
    #for MACHINE in V_HBM128_DDR8 V_HBM256_DDR8 V_HBM512_DDR8 V_HBM1024_DDR8 V_HBM2048_DDR8 V_HBM4096_DDR8
    for MACHINE in V_HBM128_PCM8 V_HBM256_PCM8 V_HBM512_PCM8 V_HBM1024_PCM8 V_HBM2048_PCM8 V_HBM4096_PCM8  
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
            echo "" >> sweepjob.sh
            echo "wait" >> sweepjob.sh
            echo "" >> sweepjob.sh
exit 0
