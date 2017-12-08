#!/bin/bash

ScriptDir=$(realpath $(dirname $0))
ProjectDir=$(realpath ${ScriptDir}/../..)
cd ${ProjectDir}

OrbSlamPlatform=`uname -m`
OrbSlamToolset=gcc.`gcc -dumpversion`
OrbSlamBuildtype=Release

BuildDir="products/cmake.make.linux.${OrbSlamPlatform}.${OrbSlamToolset}.${OrbSlamBuildtype}"

if [ ! -e Vocabulary/ORBvoc.bin ]
then
    cd ${ProjectDir}/Vocabulary
    echo "Uncompress vocabulary ..."
    tar -xf ORBvoc.bin.tar.gz
    cd ${ProjectDir}
fi

${ProjectDir}/${BuildDir}/semantic_monocular \
    ${ProjectDir}/Vocabulary/ORBvoc.bin \
    ${ProjectDir}/Examples/Monocular/Garching-Test-Drive.yaml \
    ${ProjectDir}/data/Garching_LoopClosure-4-images \
    ${ProjectDir}/data/Garching_LoopClosure-4-features.json
