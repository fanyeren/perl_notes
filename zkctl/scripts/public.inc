#! /bin/bash

ZKCTL_BIN=$(pwd)/zkctl.bin
NOAH_OP_SERVICE_INSTANCE=/home/work/opbin/noah-su-dealhost/bin/noah-su-deal-serviceinstance.sh

function log_msg()
{
    echo `date "+%Y-%m-%d %H:%M:%S"`" $@" >> $(dirname $0)/hjz.log
}
function log_err()
{
    log_msg "$@"
    echo `date "+%Y-%m-%d %H:%M:%S"`" $@" > /dev/stderr
}
