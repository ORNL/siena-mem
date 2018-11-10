#!/bin/bash


if(($# != 2)) 
then
    echo "Usage:"
    echo "./generate_mapping.sh app.aspen mach.aspen"
    exit 0
fi

APP=$1
MACH=$2

DLIST=()
while read -r line; do
    var1=$(echo $line | awk '{print $2} ')
    DLIST+=($var1)
done < <(grep data ${APP})
nData=${#DLIST[@]} 
echo ${nData}" Data Structures: "${DLIST[@]}


MLIST=()
while read -r line; do
    var1=$(echo $line | awk '{print $2} ')
    MLIST+=($var1)
done < <(grep 'memory.*{$' ${MACH})
nMem=${#MLIST[@]}
echo ${nMem}" Memories: "${MLIST[@]}


mid=0
nMapping=$(( nMem ** nData ))
echo "nMapping "$nMapping
for((;mid<nMapping;mid++ ))
do
    OUTPUT=mapping${mid}.aspen
    echo "model $APP{" > $OUTPUT
    echo "" >> $OUTPUT
    echo "param n = 1" >> $OUTPUT
    echo "" >> $OUTPUT
    echo "//Map Data to Machine Components with resource load/store" >> $OUTPUT
    echo "data {" >> $OUTPUT
    echo "" >> $OUTPUT
    
    for((i=0;i<nData;i++ ))
    do
	j=$(( nData - i - 1 ))
	k=$(( nMem ** j ))
	m=$(( mid /  k ))
	m=$(( m %  nMem ))
	echo "i="$i" j="$j" k="$k" m="$m
	echo "   map "${DLIST[$did]}" to "${MLIST[m]} >> $OUTPUT
    done

    echo "}" >> $OUTPUT
    echo "" >> $OUTPUT
    echo "// Map Kernel Execution to Machine Components with resource flops" >> $OUTPUT
    echo "kernel{" >> $OUTPUT
    echo "}" >> $OUTPUT
    echo "}" >> $OUTPUT
done
exit 0



