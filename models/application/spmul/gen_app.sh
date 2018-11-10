#!/bin/bash

declare -a NROW=(1508065  2097152  4690002  5363260  16777216  23947347  50912018)
declare -a NNZ=(27090195  182082942  20316253  79023142  100663202  57708624   108109320)
NUM_SET=${#NNZ[@]}

NUM_THREADS=32

for (( i=0; i<${NUM_SET}; i++ ))
do   
    CONFIG="spmul_NROW"${NROW[i]}"_NNZ"${NNZ[i]}"_t"$NUM_THREADS
    FILENAME=$CONFIG".aspen"
    
    A=${NROW[i]}
    B=${NNZ[i]}
    SPAR=$(( B / A + 1 ))
    NNZ=$(( SPAR * A  ))
    echo "generate "$FILENAME" SPAR"$SPAR" NNZ"$NNZ

    echo "model "$CONFIG"{" > $FILENAME
    echo "" >> $FILENAME
    echo "param nThreads="$NUM_THREADS  >> $FILENAME
    echo "param itr=1" >> $FILENAME
    echo "" >> $FILENAME
    echo "param nRows="${NROW[i]} >> $FILENAME
    echo "param NNZ="${NNZ[i]} >> $FILENAME
    echo "param global_control=nRows" >> $FILENAME
    echo "param dpsize = 8" >> $FILENAME
    echo "param spsize = 4" >> $FILENAME
    echo "" >> $FILENAME
    echo "data rowptr as Array(nRows+1, spsize) //nrows+1" >> $FILENAME
    echo "data colind as Array(NNZ,spsize)" >> $FILENAME
    echo "data values as Array(NNZ, dpsize)" >> $FILENAME
    echo "data x as Array(nRows,dpsize)" >> $FILENAME
    echo "data y as Array(nRows,dpsize)" >> $FILENAME
    echo "" >> $FILENAME
    echo "kernel spmul {" >> $FILENAME
    echo "   execute [nThreads] \"spmul\" {" >> $FILENAME
    echo "      loads [nRows] of size [spsize] from  rowptr  as stride">> $FILENAME
    echo "      loads [NNZ] of size [dpsize] from values     as stride,dep(rowptr)" >> $FILENAME
    echo "      loads [NNZ] of size [spsize] from colind     as stride,dep(rowptr)" >> $FILENAME
    echo "      loads [NNZ] of size [dpsize] from x          as random,dep(colind)"  >> $FILENAME
    echo "      flops [NNZ*2] as dp" >> $FILENAME
    echo "      stores[nRows] of size [dpsize] to y          as stride,dep(x,values)"  >> $FILENAME
    echo "}}" >> $FILENAME
    echo ""  >> $FILENAME
    echo "kernel main {"  >> $FILENAME
    echo "   call spmul //warm up"  >> $FILENAME
    echo "   iterate [itr] {"  >> $FILENAME
    echo "        call spmul"  >> $FILENAME
    echo "}}}"  >> $FILENAME

    for MACHINE in V_HBM128_DDR8 V_HBM256_DDR8 V_HBM512_DDR8 V_HBM1024_DDR8 V_HBM2048_DDR8 V_HBM4096_DDR8
    do
	echo "./main_release "$FILENAME"  ../machinemodels/"$MACHINE"/hms.aspen > out_"$CONFIG"_"$MACHINE" 2>&1 & " >> sweepjob.sh
    done
done

exit 0
