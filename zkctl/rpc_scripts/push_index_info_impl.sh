#! /bin/bash

# read index info from stdin
INDEX_INFO=$(cat)

# TODO:: DANGER!! DO CHECK content of ${INDEX_INFO} 
eval "${INDEX_INFO}"

function log_index_info()
{
    if [ "x$1" == "xstdout" ]; then
        echo ${INDEX_INFO} | xargs -n 1
    else
        echo ${INDEX_INFO} | xargs -n 1 > $1
    fi
}

TEST_FLAG=$1
if [ "x${TEST_FLAG}" == "x--test-only" ]; then
    # echo index info back to remote peer
    log_index_info stdout
    exit 0
elif [ "x${TEST_FLAG}" == "x" ]; then
    # log index info to local file
    LOG_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
    LOG_FILE="${LOG_DIR}/${BUILD_TIME}-${BUILD_NAME}-${INDEX_TYPE}-${PARTITION_NO}.info"
    log_index_info ${LOG_FILE}
    # zkctl index push ...
    exit $?
else
    echo "usage: push_index_info_impl.sh [--test-only] < index.info"
    exit 1
fi

