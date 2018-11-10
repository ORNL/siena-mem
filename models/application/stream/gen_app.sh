#!/bin/bash

    NUM_THREADS=1 #32
    N=8 #107493783

    CONFIG="stream_N"$N"_t"$NUM_THREADS
    FILENAME=$CONFIG".aspen"

    echo "generate file:" $FILENAME
    
    echo "model "$CONFIG"{" > $FILENAME
    echo "" >> $FILENAME
    echo "param nThreads="$NUM_THREADS  >> $FILENAME
    echo "param nItr=1" >> $FILENAME
    echo "param N="$N >> $FILENAME
    echo "param dpsize=8"  >> $FILENAME
    echo ""  >> $FILENAME
    echo "data a as Array(N, dpsize)"  >> $FILENAME
    echo "data b as Array(N, dpsize)"  >> $FILENAME
    echo "data c as Array(N, dpsize)"  >> $FILENAME
    echo ""  >> $FILENAME
    echo "kernel triad {"  >> $FILENAME
    echo "    execute [nThreads] \"triad\" {"  >> $FILENAME
    echo "            loads [N] of size [dpsize] from a as stride "  >> $FILENAME
    echo "            loads [N] of size [dpsize] from b as stride "  >> $FILENAME
    echo "            flops [N*2] as dp "  >> $FILENAME
    echo "            stores[N] of size [dpsize] to   c as stride,dep(a,b) "  >> $FILENAME
    echo "}}"  >> $FILENAME
    echo ""  >> $FILENAME
    echo "kernel main {"  >> $FILENAME
    echo "   call triad //warm up"  >> $FILENAME
    echo "   iterate [nItr] {"  >> $FILENAME
    echo "        call triad"  >> $FILENAME
    echo "}}}"  >> $FILENAME


BIN=../../bin/main_release
MACHDIR=../machine

    for MACHINE in V_HBM128_DDR8 V_HBM256_DDR8 V_HBM512_DDR8 V_HBM1024_DDR8 V_HBM2048_DDR8 V_HBM4096_DDR8
    do
	echo "./main_release "$FILENAME"  ../machinemodels/"$MACHINE"/hms.aspen > out_"$CONFIG"_"$MACHINE" 2>&1 & " >> sweepjob.sh
    done

    N=$(( N + N/2 ))
done

exit 0
