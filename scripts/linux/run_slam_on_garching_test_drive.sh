#!/bin/bash

set -e

ScriptDir=$(realpath $(dirname $0))
ProjectDir=$(realpath ${ScriptDir}/../..)
cd ${ProjectDir}

Platform=`uname -m`
Toolset=gcc.`gcc -dumpversion`
Buildtype=Release

BuildDir="products/cmake.make.linux.${Platform}.${Toolset}.${Buildtype}"

if [ "$1" = "" ]
then
    echo "Provide video path"
    exit 1
fi

if [ ! -e Vocabulary/ORBvoc.bin ]
then
    cd ${ProjectDir}/Vocabulary
    echo "Uncompress vocabulary ..."
    tar -xf ORBvoc.bin.tar.gz
    cd ${ProjectDir}
fi

export LD_LIBRARY_PATH=/usr/local/lib/:$LD_LIBRARY_PATH

${ProjectDir}/${BuildDir}/mono_video \
    ${1} \
    ${ProjectDir}/Vocabulary/ORBvoc.bin \
    ${ProjectDir}/Examples/Monocular/Garching-Test-Drive.yaml
