#/bin/bash

script_dir=$(realpath $(dirname $0))
root_dir=$(realpath ${script_dir}/../../..)

rm -rf ${root_dir}/products

scan-build ${root_dir}/scripts/linux/build.sh
