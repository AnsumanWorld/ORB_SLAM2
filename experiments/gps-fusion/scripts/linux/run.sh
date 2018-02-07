#!/bin/bash

script_dir=$(realpath $(dirname $0))
project_dir=$(realpath ${script_dir}/../..)
build_dir="products/linux"
gps_fusion_dir="${project_dir}/${build_dir}"

current_dir=`pwd`
cd ${project_dir}

app=${gps_fusion_dir}/apps/app-gps-ba
cfg=${project_dir}/cfg/config.json

${app} ${cfg}

cd ${current_dir}
