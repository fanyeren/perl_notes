#!/usr/bin/env python
# -*- coding: utf-8 -*-

import json
import sys
import os
import imp
import logging
from operator import itemgetter
from StringIO import StringIO

def run_case(in_name, out_name, handle, diff_name=None):
    '''run a test case.
    1. read input from <in_name>
    2. run placement_plan_generate to generate actions
    3. compare output with <out_name>
    4. if ok, output [Succeed], otherwise write output to <out_name>.diff
    '''
    buff_out=StringIO()
    dest_out=open(out_name).read()
    sysout=sys.stdout
    sysin=sys.stdin

    sys.stdin=open(in_name)
    sys.stdout=buff_out
    handle.main()
    sys.stdout=sysout
    sys.stdin=sysin

    real_out=buff_out.getvalue()
    if dest_out==real_out:
        print '[\x1b[00;32mSucceed\x1b[00m] Case [%s] to [%s]' % (in_name, out_name)
        return True
    else:
        if None==diff_name:
            diff_name='%s.diff' % out_name
        open(diff_name,'w').write(real_out)
        print '[\x1b[00;31mFailed\x1b[00m] Please check diff of [%s] and [%s]' % (out_name, diff_name)
        return False

def json_to_csv(obj):
    '''convert the json placement to csv format.
    '''
    place=obj['placement']
    nodes=obj['slavenode']
    dbs={}
    result=[]
    for node in sorted(place.keys()):
        result.append( '%s' % node )
        if not nodes[node]['online_state'] == 'online':
            result.append( '(%s)' % (nodes[node]['online_state']))
        for block in sorted(place[node]):
            dbs.setdefault(block["partition"],[])
            dbs[block["partition"]].append('%s(%s)' % ((node,block["state"]) ))
            result.append(',%s(%s)' % (block["partition"],block["state"]))
        result.append( '\n' )
    for block in sorted(dbs.keys()):
        result.append( '%s' % block )
        for instance in dbs[block]:
            result.append( ',%s' % instance )
        result.append( '\n' )
    return ''.join(result)


if __name__=='__main__':
    handle=imp.load_source('zk_plan', '.', open('placement-plan_generate'))
    handle.LOG.setLevel(logging.ERROR)
    #case collection, format: [input file, output file, diff file(optional)]
    cases=(
            #service instance
            ("test/in.instance.del", "test/out.instance.del", None),
            #machine
            ("test/in.machine.bad", "test/out.machine.bad", None),
            ("test/in.machine.new", "test/out.machine.new", None),
            #replica
            ("test/in.replica.rm", "test/out.replica.rm", None),
            ("test/in.replica.add", "test/out.replica.add", None),
            #layer
            ("test/in.layer.rm", "test/out.layer.rm", None),
            ("test/in.layer.add", "test/out.layer.add", None),

            #real
            ("test/in.case1", "test/out.case1", None),
            ("test/in.normal", "test/out.normal", None),
            ("test/in.huge", "test/out.huge", None),
            ("test/in.tobedeltonew", "test/out.tobedeltonew", None),
            ("test/in.addthenmove", "test/out.addthenmove", None),
            ("test/in.todo", "test/out.todo", None),
            ("test/in.hjz", "test/out.hjz", None),
            )
    for case in cases:
        if run_case(case[0], case[1], handle, case[2]):
            handle.simulate(case[0], case[1], open('%s.csv' % case[1],'w'))
        else:
            handle.simulate(case[0], '%s.diff' % case[1], open('%s.csv' % case[1],'w'))
    #bad cases, format: [input file, exit code]
    badcases=(
            #normal case.
            {"in": "test/in.case1", "ret": 0},
            #4 replica, while only 3 nodes.
            {"in": "test/in.invalid", "ret": 256},
            )
    for case in badcases:
        ret=os.system('cat %s|python placement-plan_generate &> /dev/null' % case["in"])
        if ret==case["ret"]:  
            fmt='[\x1b[00;32mSucceed\x1b[00m] BadCase [%s] ret [%d] due[%d]'
        else:
            fmt='[\x1b[00;31mFailed\x1b[00m] BadCase [%s] ret [%d] due[%d]'
        print fmt % (case["in"], ret, case["ret"])
