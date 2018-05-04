#!/bin/bash

set -e

ScriptDir=$(realpath $(dirname $0))
ProjectDir=$(realpath ${ScriptDir}/..)
cd ${ProjectDir}

docker run --rm -it --name orb-slam2-build --volume="$(pwd):/root/orb-slam2" \
    shanmukhananda/orb-slam2:latest \
    /bin/bash -c \
    "/root/orb-slam2/scripts/linux/build.sh"
