#!/bin/bash

ScriptDir=$(realpath $(dirname $0))
WorkingDir=${ScriptDir}
ProjectDir=$(realpath ${ScriptDir}/../..)

pwd_=${pwd}

if [ "$#" -ne 1 ]; then
    echo "usage: run.sh ResultSHA. example: run.sh 07"
	exit 1
fi

ResultSHA="$1"

Platform=`uname -m`
Toolset=gcc.`gcc -dumpversion`
Buildtype=Release
BuildDir=products/cmake.make.linux.${Platform}.${Toolset}.${Buildtype}

cd ${WorkingDir}

${ProjectDir}/${BuildDir}/tools/eval_vo/eval_odometry ${ResultSHA}
cd ${pwd}
