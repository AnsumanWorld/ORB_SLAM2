#!/bin/bash

ScriptDir=$(realpath $(dirname $0))
ProjectDir=$(realpath ${ScriptDir}/../..)
cd ${ProjectDir}

Platform=`uname -m`
Toolset=gcc.`gcc -dumpversion`
Buildtype=Release

BuildDir="products/cmake.make.linux.${Platform}.${Toolset}.${Buildtype}"

if [ ! -e Vocabulary/ORBvoc.bin ]
then
    cd ${ProjectDir}/Vocabulary
    echo "Uncompress vocabulary ..."
    tar -xf ORBvoc.bin.tar.gz
    cd ${ProjectDir}
fi

export LD_LIBRARY_PATH=/usr/local/lib/:${BuildDir}/ext_g2o:$LD_LIBRARY_PATH

App=${BuildDir}/experiments/kitti-runner/kitti-runner
Vocabulary=${ProjectDir}/Vocabulary/ORBvoc.bin

# Settings=${ProjectDir}/Examples/Monocular/using-gps/KITTI00-02.yaml
# Settings=${ProjectDir}/Examples/Monocular/using-gps/KITTI03.yaml
Settings=${ProjectDir}/Examples/Monocular/using-gps/KITTI04-12.yaml

Mode="gt"
# Mode="oxts"

# ImagesDir=/mnt/hgfs/C/data/synced/00/2011_10_03/2011_10_03_drive_0027_sync/image_00/data
# ImagesDir=/mnt/hgfs/C/data/data_odometry_gray/dataset/sequences/00/image_0
# SensorSource=/mnt/hgfs/C/data/synced/00/2011_10_03/2011_10_03_drive_0027_sync/oxts/data
# SensorSource=/mnt/hgfs/C/data/gt/data_odometry_poses/dataset/poses/00.txt

# ImagesDir=/mnt/hgfs/C/data/synced/01/2011_10_03/2011_10_03_drive_0042_sync/image_00/data
# ImagesDir=/mnt/hgfs/C/data/data_odometry_gray/dataset/sequences/01/image_0
# SensorSource=/mnt/hgfs/C/data/synced/01/2011_10_03/2011_10_03_drive_0042_sync/oxts/data
# SensorSource=/mnt/hgfs/C/data/gt/data_odometry_poses/dataset/poses/01.txt

# ImagesDir=/mnt/hgfs/C/data/synced/02/2011_10_03/2011_10_03_drive_0034_sync/image_00/data
# ImagesDir=/mnt/hgfs/C/data/data_odometry_gray/dataset/sequences/02/image_0
# SensorSource=/mnt/hgfs/C/data/synced/02/2011_10_03/2011_10_03_drive_0034_sync/oxts/data
# SensorSource=/mnt/hgfs/C/data/gt/data_odometry_poses/dataset/poses/02.txt

# ImagesDir=/mnt/hgfs/C/data/data_odometry_gray/dataset/sequences/03/image_0
# SensorSource=/mnt/hgfs/C/data/gt/data_odometry_poses/dataset/poses/03.txt

# ImagesDir=/mnt/hgfs/C/data/synced/04/2011_09_30/2011_09_30_drive_0016_sync/image_00/data
# ImagesDir=/mnt/hgfs/C/data/data_odometry_gray/dataset/sequences/04/image_0
# SensorSource=/mnt/hgfs/C/data/synced/04/2011_09_30/2011_09_30_drive_0016_sync/oxts/data
# SensorSource=/mnt/hgfs/C/data/gt/data_odometry_poses/dataset/poses/04.txt

# ImagesDir=/mnt/hgfs/C/data/synced/05/2011_09_30/2011_09_30_drive_0018_sync/image_00/data
# ImagesDir=/mnt/hgfs/C/data/data_odometry_gray/dataset/sequences/05/image_0
# SensorSource=/mnt/hgfs/C/data/synced/05/2011_09_30/2011_09_30_drive_0018_sync/oxts/data
# SensorSource=/mnt/hgfs/C/data/gt/data_odometry_poses/dataset/poses/05.txt

# ImagesDir=/mnt/hgfs/C/data/synced/06/2011_09_30/2011_09_30_drive_0020_sync/image_00/data
# ImagesDir=/mnt/hgfs/C/data/data_odometry_gray/dataset/sequences/06/image_0
# SensorSource=/mnt/hgfs/C/data/synced/06/2011_09_30/2011_09_30_drive_0020_sync/oxts/data
# SensorSource=/mnt/hgfs/C/data/gt/data_odometry_poses/dataset/poses/06.txt

# ImagesDir=/mnt/hgfs/C/data/synced/07/2011_09_30/2011_09_30_drive_0027_sync/image_00/data
# ImagesDir=/mnt/hgfs/C/data/data_odometry_gray/dataset/sequences/07/image_0
# SensorSource=/mnt/hgfs/C/data/synced/07/2011_09_30/2011_09_30_drive_0027_sync/oxts/data
# SensorSource=/mnt/hgfs/C/data/gt/data_odometry_poses/dataset/poses/07.txt

# ImagesDir=/mnt/hgfs/C/data/synced/08/2011_09_30/2011_09_30_drive_0028_sync/image_00/data
# ImagesDir=/mnt/hgfs/C/data/data_odometry_gray/dataset/sequences/08/image_0
# SensorSource=/mnt/hgfs/C/data/synced/08/2011_09_30/2011_09_30_drive_0028_sync/oxts/data
# SensorSource=/mnt/hgfs/C/data/gt/data_odometry_poses/dataset/poses/08.txt

# ImagesDir=/mnt/hgfs/C/data/synced/09/2011_09_30/2011_09_30_drive_0033_sync/image_00/data
# ImagesDir=/mnt/hgfs/C/data/data_odometry_gray/dataset/sequences/09/image_0
# SensorSource=/mnt/hgfs/C/data/synced/09/2011_09_30/2011_09_30_drive_0033_sync/oxts/data
# SensorSource=/mnt/hgfs/C/data/gt/data_odometry_poses/dataset/poses/09.txt

# ImagesDir=/mnt/hgfs/C/data/synced/10/2011_09_30/2011_09_30_drive_0034_sync/image_00/data
# ImagesDir=/mnt/hgfs/C/data/data_odometry_gray/dataset/sequences/10/image_0
# SensorSource=/mnt/hgfs/C/data/synced/10/2011_09_30/2011_09_30_drive_0034_sync/oxts/data
# SensorSource=/mnt/hgfs/C/data/gt/data_odometry_poses/dataset/poses/10.txt

${App} ${Mode} ${Vocabulary} ${Settings} ${ImagesDir} ${SensorSource}
