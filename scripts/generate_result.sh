#!/bin/bash

DIR=../../models
MACH_DIR=${DIR}/machine/
APP_DIR=${DIR}/application/
EXE_DIR=./

declare -a applist=(
${APP_DIR}"app0"
${APP_DIR}"app1"
${APP_DIR}"app2"
)
appnum=${#applist[@]} 


declare -a machlist=(
${MACH_DIR}"mach0"
${MACH_DIR}"mach1"
${MACH_DIR}"mach2"
)
machnum=${#machlist[@]} 


declare -a maplist=(
"mapping0"
"mapping1"
"mapping2"
)
mapnum=${#maplist[@]} 


export RAMULATOR_ROOT=${MACH_DIR}/config
export DRAMSIM_ROOT=${MACH_DIR}/config
export NVDIMMSIM_ROOT=${MACH_DIR}/config


#for f in *mapping*.out; do
for f in $(ls *mapping*.out) ; do
    echo "$f"
    var=$(grep "Core_S:: \[0 - " $f)
    var1=$(echo $var | awk '{print $4} ')
    var2=${var1::-1}
    echo $var2
done
