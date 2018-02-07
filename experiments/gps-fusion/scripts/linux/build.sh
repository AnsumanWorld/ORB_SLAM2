#!/bin/bash

script_dir=$(realpath $(dirname $0))
project_dir=$(realpath ${script_dir}/../..)
build_dir="products/linux"

cd "${project_dir}"
mkdir -p "${build_dir}"

cmake ${project_dir} \
    -B${build_dir} \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DUSE_COTIRE=OFF

cores=$(nproc)
time cmake --build ${build_dir} -- -j${cores}
