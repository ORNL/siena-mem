#!/bin/bash

NUM_THREADS=32
echo "" >  sweepjob.sh
for N1 in 2000 4000 6000 8000 10000 12000
do   
    for N2 in 7
    do
	CONFIG="conv_N"$N1"_F"$N2"_t"$NUM_THREADS
	FILENAME=$CONFIG".aspen"
	echo "generate file:" $FILENAME
	
	echo "model "$CONFIG"{" > $FILENAME
	echo "" >> $FILENAME
	echo "param nThreads="$NUM_THREADS  >> $FILENAME
	echo "param numItr=1" >> $FILENAME
	echo "param n1="$N1  >> $FILENAME
	echo "param n2="$N2  >> $FILENAME
	echo "param dpsize=8">> $FILENAME
	echo ""  >> $FILENAME
	echo "data A as Array(n1, n1, dpsize)"  >> $FILENAME
	echo "data filter as Array(n2, n2, dpsize)"  >> $FILENAME
	echo ""  >> $FILENAME
	echo "kernel conv{"  >> $FILENAME
	echo "    execute [nThreads] \"conv\" {"  >> $FILENAME
	echo "            loads [(n1-n2+1)*(n1-n2+1)] of size [dpsize] from A as conv(n2,n2)">> $FILENAME
	echo "            loads [(n1-n2+1)*(n1-n2+1)*n2*n2] of size [dpsize] from filter as stride">> $FILENAME
	echo "            flops [(n1-n2+1)*(n1-n2+1)*n2*n2] as dp " >> $FILENAME
	echo "            stores[(n1-n2+1)*(n1-n2+1)] of size [dpsize] to A as stride,offset(n2/2) "  >> $FILENAME
	echo "}}"  >> $FILENAME
	echo ""  >> $FILENAME
	echo "kernel main {"  >> $FILENAME
	echo "   call conv //warm up"  >> $FILENAME
	echo "   iterate [numItr] {"  >> $FILENAME
	echo "        call conv"  >> $FILENAME
	echo "}}}"  >> $FILENAME

	for MACHINE in V_HBM128_DDR8 V_HBM256_DDR8 V_HBM512_DDR8 V_HBM1024_DDR8 V_HBM2048_DDR8 V_HBM4096_DDR8
	do
	    echo "./main_release "$FILENAME"  ../machinemodels/"$MACHINE"/hms.aspen > out_"$CONFIG"_"$MACHINE" 2>&1 & " >> sweepjob.sh
	done
    done
done

exit 0
