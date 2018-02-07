#!/bin/bash

script_dir=$(realpath $(dirname $0))
project_dir=$(realpath ${script_dir}/../..)
build_dir="products/linux"
gps_fusion_dir="${project_dir}/${build_dir}"

${gps_fusion_dir}/tests/tests-fuser
