#!/bin/bash

set -e

# apt-get update && apt-get install python-pip python-tk -y
# pip install evo --upgrade

################################################################################
# User Input
Option="gps"
Sequences=/mnt/hgfs/C/data/data_odometry_gray/dataset/sequences
Poses=/mnt/hgfs/C/data/gt/data_odometry_poses/dataset/poses
MinInitGPSFrames=99999
GPSSkipFrequency=99999
################################################################################

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

export LD_LIBRARY_PATH=/usr/local/lib/:${BuildDir}/${Buildtype}/lib:$LD_LIBRARY_PATH

App=${BuildDir}/${Buildtype}/bin/kitti-runner
Vocabulary=${ProjectDir}/Vocabulary/ORBvoc.bin

Settings00_02=${ProjectDir}/Examples/Monocular/using-gps/KITTI00-02.yaml
Settings03=${ProjectDir}/Examples/Monocular/using-gps/KITTI03.yaml
Settings04_12=${ProjectDir}/Examples/Monocular/using-gps/KITTI04-12.yaml

function save_gnuplot_top_view(){
    GroundTruth=${1}
    SlamTraj=${2}
    OutputPng=${3}
    Title=${4}

    gnuplot -e "set terminal png size 1280,720; \
    set title '${Title}'; \
    set output '${OutputPng}'; \
    set xlabel 'x [m]'; \
    set ylabel 'z [m]'; \
    set zlabel 'y [m]'; \
    plot '${GroundTruth}' using 4:12:8 title 'GROUND-TRUTH' with lines, \
    '${SlamTraj}' using 4:12:8 title 'ORB-SLAM2' with lines"
}

function save_gnuplot_side_view(){
    GroundTruth=${1}
    SlamTraj=${2}
    OutputPng=${3}
    Title=${4}

    gnuplot -e "set terminal png size 1280,720; \
    set title '${Title}'; \
    set output '${OutputPng}'; \
    set xlabel 'x [m]'; \
    set ylabel 'z [m]'; \
    set zlabel 'y [m]'; \
    splot '${GroundTruth}' using 4:12:8 title 'GROUND-TRUTH' with lines, \
    '${SlamTraj}' using 4:12:8 title 'ORB-SLAM2' with lines"
}

function save_evo_ape(){
    GroundTruth=${1}
    SlamTraj=${2}
    OutputPdf=${3}

    evo_ape kitti ${GroundTruth} ${SlamTraj} \
        --plot_mode xyz \
        --pose_relation full \
        --save_plot ${Output}_evo_ape.pdf \
        --save_results ${Output}_evo_ape.zip
}

function save_evo_traj(){
    GroundTruth=${1}
    SlamTraj=${2}
    Output=${3}

    evo_traj kitti ${SlamTraj} --ref=${GroundTruth} \
        --plot_mode=xyz \
        --save_plot ${Output}_evo_traj.pdf
}

function run_sequence() {
    Sequence=${1}
    ResultDir=${ProjectDir}/data/${Timestamp}/${Sequence}
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

    echo "Starting sequence ${Sequence}" 

    mkdir -p ${ResultDir}
    cd ${ResultDir}

    ImagesDir=${Sequences}/${Sequence}/image_0
    SensorSource=${Poses}/${Sequence}.txt
    SlamTraj="${Sequence}.txt"

    ${App} ${Option} ${Vocabulary} ${Settings} ${ImagesDir} \
    ${SensorSource} ${SlamTraj} ${MinInitGPSFrames} ${GPSSkipFrequency} \
    > ${Sequence}.log 2>&1
    
    # save_gnuplot_top_view ${SensorSource} ${SlamTraj} ${Sequence}_top.png ${Sequence}
    # save_gnuplot_side_view ${SensorSource} ${SlamTraj} ${Sequence}_side.png ${Sequence}
    save_evo_traj ${SensorSource} ${SlamTraj} ${Sequence}

    GTClipped=${Sequence}_clipped.txt
    python ${ProjectDir}/experiments/py/clip-traj/clip_traj.py ${SensorSource} ${SlamTraj} ${GTClipped}

    save_evo_ape ${GTClipped} ${SlamTraj} ${Sequence}
    rm -rf ${GTClipped}

    echo "Finished sequence ${Sequence}" 
}

# run_sequence 00
# run_sequence 01
# run_sequence 02
# run_sequence 03
run_sequence 04
# run_sequence 05
# run_sequence 06
# run_sequence 07
# run_sequence 08
# run_sequence 09
# run_sequence 10
