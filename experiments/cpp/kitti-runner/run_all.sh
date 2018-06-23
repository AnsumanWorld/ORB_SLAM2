#!/bin/bash

Timestamp=$(date '+%Y%m%d_%H%M%S')
ScriptDir=$(realpath $(dirname $0))
ProjectDir=$(realpath ${ScriptDir}/../../..)
cd ${ProjectDir}

Platform=`uname -m`
Toolset=gcc.`gcc -dumpversion`
Buildtype=Release

Products="products/cmake.make.linux.${Platform}.${Toolset}.${Buildtype}"
BuildDir=${ProjectDir}/${Products}

if [ ! -e ${ProjectDir}/Vocabulary/ORBvoc.bin ]
then
    cd ${ProjectDir}/Vocabulary
    echo "Uncompress vocabulary ..."
    tar -xf ORBvoc.bin.tar.gz
    cd ${ProjectDir}
fi

function save_plot_top_view(){
    GroundTruth=${1}
    SlamTraj=${2}
    OutputPng=${3}

    gnuplot -e "set terminal png size 1280,720;\
    set output '${OutputPng}';\
    set xlabel 'x [m]'; \
    set ylabel 'z [m]'; \
    set zlabel 'y [m]'; \
    plot '${GroundTruth}' using 4:12:8 title 'GROUND-TRUTH' with lines,\
    '${SlamTraj}' using 4:12:8 title 'ORB-SLAM2' with lines"
}

function save_plot_side_view(){
    GroundTruth=${1}
    SlamTraj=${2}
    OutputPng=${3}

    gnuplot -e "set terminal png size 1280,720;\
    set output '${OutputPng}';\
    set xlabel 'x [m]'; \
    set ylabel 'z [m]'; \
    set zlabel 'y [m]'; \
    splot '${GroundTruth}' using 4:12:8 title 'GROUND-TRUTH' with lines,\
    '${SlamTraj}' using 4:12:8 title 'ORB-SLAM2' with lines"
}

export LD_LIBRARY_PATH=/usr/local/lib/:${BuildDir}/ext_g2o:$LD_LIBRARY_PATH

App=${BuildDir}/experiments/cpp/kitti-runner/kitti-runner
Vocabulary=${ProjectDir}/Vocabulary/ORBvoc.bin

Settings00_02=${ProjectDir}/Examples/Monocular/using-gps/KITTI00-02.yaml
Settings03=${ProjectDir}/Examples/Monocular/using-gps/KITTI03.yaml
Settings04_12=${ProjectDir}/Examples/Monocular/using-gps/KITTI04-12.yaml
Option="gps"
Sequences=/mnt/hgfs/C/data/data_odometry_gray/dataset/sequences
Poses=/mnt/hgfs/C/data/gt/data_odometry_poses/dataset/poses

function run_sequence() {
    Sequence=${1}
    ResultDir=data/${Timestamp}/${Sequence}
    Settings=""
    if [ "${Sequence}" == "00" ] || [ "${Sequence}" == "01" ] || [ "${Sequence}" == "02" ]
    then
        Settings=${Settings00_02}
    elif [ "${Sequence}" == "03" ]
    then
        Settings=${Settings03}
    else
        Settings=${Settings04_12}
    fi

    cwd=${pwd}
    
    mkdir -p ${ResultDir}
    cd ${ResultDir}

    ImagesDir=${Sequences}/${Sequence}/image_0
    SensorSource=${Poses}/${Sequence}.txt
    Output="${Sequence}.txt"
    ${App} ${Option} ${Vocabulary} ${Settings00_02} ${ImagesDir} ${SensorSource} ${Output}
    save_plot_top_view ${SensorSource} ${Output} ${Sequence}_top.png
    save_plot_side_view ${SensorSource} ${Output} ${Sequence}_side.png

    cd ${cwd}
}

run_sequence 00
# run_sequence 01
run_sequence 02
run_sequence 03
run_sequence 04
run_sequence 05
run_sequence 06
run_sequence 07
run_sequence 08
run_sequence 09
run_sequence 10
