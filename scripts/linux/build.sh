#!/bin/bash

set -e

ScriptDir=$(realpath $(dirname $0))
ProjectDir=$(realpath ${ScriptDir}/../..)
cd ${ProjectDir}

Platform=`uname -m`
Toolset=gcc.`gcc -dumpversion`
Buildtype=Release
cores=$(nproc)

if [ ! -z "$1" ]
then
    Platform="$1"
fi

if [ ! -z "$2" ]
then
    Toolset="$2"
fi

if [ ! -z "$3" ]
then
    Buildtype="$3"
fi

BuildDir="${ProjectDir}/products/cmake.make.linux.${Platform}.${Toolset}.${Buildtype}"

mkdir -p "${BuildDir}"

cmake \
    ${ProjectDir} \
    -B${BuildDir} \
    -DCMAKE_BUILD_TYPE=${Buildtype} \
    -DBUILD_ALL_EXAMPLES=ON \
    -DBUILD_EXAMPLES=ON \
    -DBUILD_EXPERIMENTS=ON \
    -DBUILD_TOOLS=ON \
    -DBUILD_TESTING=ON \
    -DWARNINGS=OFF \
    -DPRINT_CMAKE_VARIABLES=OFF \
    -DCMAKE_INSTALL_PREFIX=/usr/local

cmake \
    --build ${BuildDir} \
    --config ${Buildtype} \
    --target install \
    -- -j${cores}
