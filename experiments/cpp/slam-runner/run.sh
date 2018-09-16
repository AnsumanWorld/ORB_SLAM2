#!/bin/bash

set -e

ScriptDir=$(realpath $(dirname $0))
ProjectDir=$(realpath ${ScriptDir}/../../..)
cd ${ProjectDir}

Platform=`uname -m`
Toolset=gcc.`gcc -dumpversion`
Buildtype=Release

BuildDir="${ProjectDir}/products/cmake.make.linux.${Platform}.${Toolset}.${Buildtype}"

if [ ! -e Vocabulary/ORBvoc.bin ]
then
    cd ${ProjectDir}/Vocabulary
    echo "Uncompress vocabulary..."
    tar -xf ORBvoc.bin.tar.gz
    cd ${ProjectDir}
fi

BinaryDir=${BuildDir}/${Buildtype}/bin

export LD_LIBRARY_PATH=/usr/local/lib/:${BinaryDir}:$LD_LIBRARY_PATH

SettingsPath=${ProjectDir}/Examples/Monocular/Garching-Test-Drive.yaml
# SettingsPath=${ProjectDir}/Examples/Monocular/KITTI04-12.yaml
VocabularyPath=${ProjectDir}/Vocabulary/ORBvoc.bin
# ImageSource=/mnt/c/Shanmukha/Shared/Dataset/Garching/Garching_LoopClosure-5.mp4
ImageSource=/mnt/c/Shanmukha/Shared/Dataset/Garching/Garching-5/images
# ImageSource=/mnt/c/Shanmukha/Shared/Dataset/KITTI/data_odometry_gray/dataset/sequences/07/image_0
# ImageSource=0

 ${BinaryDir}/slam-runner \
    --image_source=${ImageSource} \
    --frame_interval_ms=1000 \
    --settings_path=${SettingsPath} \
    --vocabulary_path=${VocabularyPath} \
    --wait_for_stdin=false \
    --show_viewer=false \
    |& tee slam-runner.log