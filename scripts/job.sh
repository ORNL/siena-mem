#submit batch jobs in parallel

#SBATCH -J sweep
#SBATCH -p skx-normal
#SBATCH -N 1
#SBATCH -n 1
#SBATCH -t 01:00:00

#!/bin/bash
DIR:=${CURDIR}                                                                                                          
EXE_DIR=$DIR/bin
MACH_DIR=$DIR/models/mach
APP_DIR=$DIR/models/app

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

for (( i=0; i<${appnum}; i++ ))
do
    APP=${applist[$i]}
    for (( j=0; j<${machnum}; j++ ))
    do
	MACH=${machlist[$j]}
	for (( k=0; k<${mapnum}; k++ ))
	do
	    MAP=${maplist[$k]}
	    echo "Exec "${APP}" on "${MACH}" by mapping "$MAP
	    ${EXE_DIR}/siena ${APP}.aspen ${MACH}.aspen ${MAP}.aspen > output_${APP}_${MACH}_${MAP}.out 2>&1 &
	    done
	done
    wait
done

exit 0
