#! /bin/bash

# read dyndata info from stdin
DYNDATA_INFO=$(cat)

# TODO:: DANGER!! DO CHECK content of ${DYNDATA_INFO} 
eval "${DYNDATA_INFO}"

function log_dyndata_info()
{
    if [ "x$1" == "xstdout" ]; then
        echo ${DYNDATA_INFO} | xargs -n 1
    else
        echo ${DYNDATA_INFO} | xargs -n 1 > $1
    fi
}

TEST_FLAG=$1
if [ "x${TEST_FLAG}" == "x--test-only" ]; then
    # echo dyndata info back to remote peer
    log_dyndata_info stdout
    exit 0
elif [ "x${TEST_FLAG}" == "x" ]; then
    # log dyndata info to local file
    LOG_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
    if [ "x${INDEX_PARTITION_ID}" == "x" ]; then
        LOG_FILE="${LOG_DIR}/${DATA_TIMESTAMP}-${DATA_NAME}.info"
    else
        LOG_FILE="${LOG_DIR}/${DATA_TIMESTAMP}-${DATA_NAME}-${INDEX_PARTITION_ID}.info"
    fi
    log_dyndata_info ${LOG_FILE}
    # ./zkctl dyndata push ...
    exit $?   
else
    echo "usage: push_dyndata_info_impl.sh [--test-only] < dyndata.info"
    exit 1
fi
