#!/bin/bash

set -e

if [ ! "${EUID}" = "0" ]
then
   echo "ERROR: ${0} must be run as root" 
   exit 1
fi

cores=$(nproc)
packages_dir=/root/packages
install_dir=/usr/local
mkdir -p ${packages_dir}
build_type=Release

function install_dependencies() {
    apt-get update --fix-missing
    apt-get install -y \
        apt-utils \
        build-essential \
        cmake \
        git \
        gnuplot \
        libboost-all-dev \
        libeigen3-dev \
        libglew-dev \
        libopencv-dev \
        libqglviewer-dev-qt5 \
        libsuitesparse-dev \
        qt5-qmake \
        qtdeclarative5-dev \
        texlive-extra-utils \
        wget
}

function download_file() {
    cd ${packages_dir}
    folder=${packages_dir}/${1}
    url=${2}

    mkdir -p ${folder}
    cd ${folder}
    wget -N ${url}
    cd ${packages_dir}
}

function download_packages() {
    pangolin_url=https://github.com/stevenlovegrove/Pangolin/archive/v0.5.tar.gz
    download_file pangolin ${pangolin_url}

    g2o_url=https://github.com/paul-michalik/g2o/archive/master.tar.gz
    download_file g2o ${g2o_url}

    googletest_url=https://github.com/google/googletest/archive/release-1.8.0.tar.gz
    download_file googletest ${googletest_url}
}

function install_googletest() {
    cd ${packages_dir}/googletest
    extracted_folder=googletest-1.8.0
    archive_file=release-1.8.0.tar.gz
    
    if [ ! -e ${extracted_folder} ]
    then
        mkdir -p ${extracted_folder}
        tar xzf ${archive_file} --directory=${extracted_folder} --strip-components=1
    fi
    
    cd ${extracted_folder}
    mkdir -p ${build_type}
    cd ${build_type}
    cmake -D CMAKE_BUILD_TYPE=${build_type} -D CMAKE_INSTALL_PREFIX=${install_dir} ..
    make -j${cores}
    make install
}

function install_pangolin() {
    cd ${packages_dir}/pangolin
    extracted_folder=pangolin-0.5
    archive_file=v0.5.tar.gz
    if [ ! -e ${extracted_folder} ]
    then
        mkdir -p ${extracted_folder}
        tar xzf ${archive_file} --directory=${extracted_folder} --strip-components=1
    fi
    cd ${extracted_folder}
    mkdir -p ${build_type}
    cd ${build_type}
    cmake -D CMAKE_BUILD_TYPE=${build_type} -D CMAKE_INSTALL_PREFIX=${install_dir} ..
    make -j${cores}
    make install
}

function install_g2o() {
    cd ${packages_dir}/g2o
    extracted_folder=g2o-master
    archive_file=master.tar.gz
    if [ ! -e ${extracted_folder} ]
    then
        mkdir -p ${extracted_folder}
        tar xzf ${archive_file} --directory=${extracted_folder} --strip-components=1
    fi
    echo ${extracted_folder}
    cd ${extracted_folder}
    mkdir -p ${build_type}
    cd ${build_type}
    cmake -D CMAKE_BUILD_TYPE=${build_type} -D CMAKE_INSTALL_PREFIX=${install_dir} ..
    make -j${cores}
    make install
}

install_dependencies
download_packages

build_type=Release
install_pangolin
install_g2o
install_googletest

build_type=Debug
install_pangolin
install_g2o
install_googletest