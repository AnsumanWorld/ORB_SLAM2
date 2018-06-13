#/bin/bash

script_dir=$(realpath $(dirname $0))
project_dir=$(realpath ${script_dir}/../../../../..)

src_dir=${project_dir}/cpp/gps-fusion
find ${src_dir} \
    -name '*.cpp' \
    -o -name '*.hpp' \
    -o -name '*.cc' \
    -o -name '*.hh' \
    -o -name '*.c' \
    -o -name '*.h' \
    | xargs clang-format -i -style=file
