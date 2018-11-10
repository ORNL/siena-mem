#!/bin/bash

if(($# < 3)) 
then
    echo "Usage:"
    echo "./generate_arch.sh arch_type=V|H device0 size0 [deviceN sizeN] [target_folder]"
    exit 0
fi
ARCH_TYPE=$1
if((($# - 3 )%2 == 1)) 
then
    d=$#
    TARGET_DIR=${!d}
    MAX=$(( $d - 1 ))
else
    TARGET_DIR=.
    MAX=$#
fi

echo "TARGET_DIR "${TARGET_DIR}" MAX "$MAX

FILE_NAME=${ARCH_TYPE}
BASEFILE_NAME=${ARCH_TYPE}
for ((i=2; i<=${MAX}; i+=2)); 
do
    DEVICE=${!i}
    d=$(( i + 1 ))
    SIZE=${!d}
    FILE_NAME=${FILE_NAME}_${DEVICE}_${SIZE}
    BASEFILE_NAME=${BASEFILE_NAME}_${DEVICE}_8192
done

FILE_NAME=${FILE_NAME}.aspen
BASEFILE_NAME=${BASEFILE_NAME}.aspen
echo "output "$BASEFILE_NAME" to "${FILE_NAME}
cp ../models/machine/${BASEFILE_NAME} ${FILE_NAME}


for ((i=2; i<=${MAX}; i+=2)); 
do
    DEVICE=${!i}
    d=$(( i + 1 ))
    SIZE=${!d}
    if(( $ARCH_TYPE == V )) && (( $i == 2 ))
    then
	oldline=cache" "L2" "{
    else
	oldline=memory" "$DEVICE
    fi

    newline="property capacity [${SIZE}*MB]"
    sed -i "/${oldline}/!b;n;c${newline}" ${FILE_NAME}
done

mv ${FILE_NAME} ${TARGET_DIR}

exit 0
