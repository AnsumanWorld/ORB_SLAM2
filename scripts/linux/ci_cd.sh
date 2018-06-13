#/bin/bash

set -e

ScriptDir=$(realpath $(dirname $0))
ProjectDir=$(realpath ${ScriptDir}/../..)
cd ${ProjectDir}

${ProjectDir}/docker/build-from-docker.sh

# :'
# # Below code detects changes in Dockerfile and bootstrap.sh
# # If there are changes in these files, script will build and upload a new docker image
# # In travis ci due to time limit build and upload of docker is causing error
# # Use this code as reference if automatic updation of docker images are required from CI setup

# function upload_docker_image () {
#   $(git diff --name-only ${TRAVIS_COMMIT_RANGE} | grep docker/Dockerfile > /dev/null)
#   dockerfile_status=$?

#   $(git diff --name-only ${TRAVIS_COMMIT_RANGE} | grep scripts/linux/bootstrap.sh > /dev/null)
#   bootstrap_status=$?

#   new_image_status=1
#   t1=${DOCKER_USER}/orb-slam2:${TRAVIS_BUILD_NUMBER}
#   t2=${DOCKER_USER}/orb-slam2:latest

#   echo bootstrap_status=${bootstrap_status}
#   echo dockerfile_status=${dockerfile_status}

#   if [[ ${dockerfile_status} == "0" || ${bootstrap_status} == "0" ]]
#   then
#       echo "New docker image is required, building new image"
#       docker build --compress --tag=${t1} --tag=${t2} --file=docker/Dockerfile .
#       new_image_status=$?
#   else
#       echo "New docker image is not required"
#   fi

#   if [[ ${new_image_status} == "0" ]]
#   then
#       if [[ ${TRAVIS_BRANCH} == "master" && ${TRAVIS_PULL_REQUEST} == "false" ]]
#       then
#           echo "Uploading docker image to Docker hub"
#           docker login --username=$DOCKER_USER --password=$DOCKER_PASS
#           docker push ${t1}
#           docker push ${t2}
#         else
#           echo "Not a commit on "master", Docker image is not pushed to Docker hub"
#       fi
#   fi
# }
# '