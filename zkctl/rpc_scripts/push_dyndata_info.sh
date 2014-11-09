#! /bin/bash

USAGE_MSG="usage: push_dyndata_info.sh <TARGET_HOST> <INDEX.info> [--test-only]"

# disable password authentication and host key checking so not to interrupt caller scripts
SSH_BIN="ssh -o PasswordAuthentication=no -o StrictHostKeyChecking=no"

STUB_CMD="/home/work/console/zkctl_rpc_scripts/mc_mis/push_dyndata_info_impl.sh"

TARGET_HOST=$1
INFO_FILE=$2
TEST_FLAG=$3

# TODO:: check for validate hostname
[ "x${TARGET_HOST}" == "x" ] && echo ${USAGE_MSG} && exit 1

# TODO:: check for validate plain file
[ "x${INFO_FILE}" == "x" ] && echo ${USAGE_MSG} && exit 1

cat ${INFO_FILE} | ${SSH_BIN} work@${TARGET_HOST} ${STUB_CMD} ${TEST_FLAG}
exit $?

