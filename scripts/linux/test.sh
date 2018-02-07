#!/bin/bash

ScriptDir=$(realpath $(dirname $0))
ProjectDir=$(realpath ${ScriptDir}/../..)
cd ${ProjectDir}

Platform=`uname -m`
Toolset=gcc.`gcc -dumpversion`
Buildtype=Release

BuildDir="products/cmake.make.linux.${Platform}.${Toolset}.${Buildtype}"

${ProjectDir}/${BuildDir}/tests/test-orb-slam2
