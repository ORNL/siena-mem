#!/bin/bash

APP_MODEL=$1
MIN_SIZE=$2
MAX_SIZE=$3
NUM_THREADS=32
TARGET_DIR=.
if(($# < 3)) 
then
    echo "Usage:"
    echo "./generate_app.sh app_name min_size max_size [target_folder]"
    exit 0
fi

if(($# == 4)) 
then
    TARGET_DIR=$4
fi

PREFIX=${APP_MODEL}_t${NUM_THREADS}

for ((i=${MIN_SIZE}; i<=${MAX_SIZE}; i+=i/2)); 
do
    FILE_NAME=${PREFIX}_N${i}.aspen
    echo "output "$FILE_NAME" to "${TARGET_DIR}
    
    cp ../models/application/${APP_MODEL}/${APP_MODEL}.aspen ${FILE_NAME}

    oldline="nThreads=1"
    newline="nThreads="$NUM_THREADS
    sed -i "s|${oldline}|${newline}|g" ${FILE_NAME}

    oldline="N=1024"
    newline="N="$i
    sed -i "s|${oldline}|${newline}|g" ${FILE_NAME}

    mv ${FILE_NAME} ${TARGET_DIR}

done

exit 0
