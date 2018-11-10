#!/bin/bash

NUM_TXN=10000000
N=$(( NUM_TXN / 10  ))
NUM_MIL=$((NUM_TXN/1000000))

for NUM_THREADS in 1 2 4 8 16 32 64 128
do   
    for PER_HBM in 100 90 80 70 60 50 40 30 20 10 0
    do
       PER_DDR=$(( 100 - PER_HBM ))
       R0=$(( PER_HBM / 10  ))
       R1=$(( 10 - R0 ))

       CONFIG="memdist"$NUM_MIL"M_HPM"$PER_HBM"_DDR"$PER_DDR"_t"$NUM_THREADS
       FILENAME=$CONFIG".aspen"

       echo "generate file:" $FILENAME
       echo "model "$CONFIG"{" > $FILENAME
       echo "" >> $FILENAME
       echo "param N = "$N >> $FILENAME
       echo "param accessize = 64 " >> $FILENAME
       echo "param nThreads = "$NUM_THREADS >> $FILENAME
       echo "param nItr = 1"  >> $FILENAME
       echo "param GB = 1073741824"  >> $FILENAME
       echo ""  >> $FILENAME
       echo "data a0 as Array(GB/accessize, accessize)"  >> $FILENAME
       echo "data a1 as Array(GB/accessize, accessize)"  >> $FILENAME
       echo "data a2 as Array(GB/accessize, accessize)"  >> $FILENAME
       echo ""  >> $FILENAME
       echo "kernel K0{"  >> $FILENAME
       echo "    execute [nThreads] \"K0\" {"  >> $FILENAME
       echo "              loads [N*"$R0"] of size [accessize] from a0 as stride "  >> $FILENAME
       echo "              loads [N*"$R1"] of size [accessize] from a1 as stride "  >> $FILENAME
       echo "    }"  >> $FILENAME
       echo "}"  >> $FILENAME

       echo ""  >> $FILENAME
       echo "kernel main {"  >> $FILENAME
       echo "   //warm up">> $FILENAME
       echo "   call K0"  >> $FILENAME
       echo "   iterate [nItr] {">> $FILENAME
       echo "      call K0"  >> $FILENAME
       echo "   }"  >> $FILENAME
       echo "}"  >> $FILENAME
       echo "}"  >> $FILENAME

       #echo "../main ../"$FILENAME" ../../machine_models/hms.aspen mapping.aspen > out_"$CONFIG" & " >> sweepjob.sh
    done
done

exit 0
