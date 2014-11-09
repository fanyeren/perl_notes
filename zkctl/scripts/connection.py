#!/usr/bin/env python
# -*- coding: utf-8 -*-
#generate a bc-bs connection json
#Usage
# 1. ./zkctl placement-plan dump > dump.hjz
# 2. ./zkctl connection refresh < dump.hjz > bcbs.json
# 3. YIIC applyconnection applyall --file=bcbs.json
# YIIC="/home/work/zooman/bin/protected/yiic"

import json
import sys
import os
import imp
from StringIO import StringIO

def oops(msg):
    sys.stderr.write("%s\n" % msg)
    sys.exit(1)
def get_conf_file():
    conf=os.environ.get("ZKCTL_CONF")
    if conf==None:
        oops('ZKCTL_CONF not set')
    return conf
def get_conf_item(item):
    conf=get_conf_file()
    for line in open(conf).readlines():
        if line.startswith(item):
            info=line.split(":")
            if len(info) < 2:
                continue
            return info[1].strip()
    oops('"%s" not found in [%s]' % (item,conf))
def get_conf_node():
    return get_conf_item('connection_node')
def get_conf_yiic():
    return get_conf_item('yiic_path')
def get_conf_arbitrator():
    return get_conf_item('arbitrator_path')

def cmp_func_id(node1,node2):
    return cmp(node1.id, node2.id)
def cmp_func_get_index_name(part1, part2):
    first_type,first_id=part1["name"].split('_')
    second_type,second_id=part2["name"].split('_')
    if first_type == second_type:    
        return cmp(int(first_id), int(second_id))
    else:
        return cmp(first_type, second_type)
def cmp_func_get_host_name(host1, host2):
    return cmp(host1["name"], host2["name"])

def update_yiic(manager, group, idc):
    indexes={}
    sorted_slavenodes=manager.slavenodes.values()
    sorted_slavenodes.sort(cmp_func_id)
    for node in sorted_slavenodes:
        #offline slavenode is not used.
        if node.online == 'offline':
            continue
        sorted_partitions=node.index_partitions()
        sorted_partitions.sort(cmp_func_id)
        for index in sorted_partitions:
            #--group-id=xxx
            if not index.group in group:
                continue
            #NEW is not working
            if node.partitions[index.id]['state']=='NEW':
                continue
            if not index.id in indexes:
                indexes[index.id]={}
                indexes[index.id]["index"]=index
                indexes[index.id]["host"]=[]
                indexes[index.id]["name"]=index.id
            port=node.partitions[index.id]["service_port"]
            indexes[index.id]["host"].append({"name":node.id,"port":port,"idc":node.idc})
    json_bsinfo=[]
    sorted_indexes=indexes.values()
    sorted_indexes.sort(cmp_func_get_index_name)
    #for each index.
    for index in sorted_indexes:
        json_hostinfo=[]
        sorted_hosts=index["host"]
        sorted_hosts.sort(cmp_func_get_host_name)
        for host in sorted_hosts:
            json_hostinfo.append( {"hostname":host["name"],
                "port":host["port"],
                "is_local":host["idc"]==idc and 1 or 0})
        json_index={}
        json_index["index_name"]=index["name"]
        if None==index.get("min_usable"):
            json_index["min_usable"]="60"
            sys.stderr.write('index "%s" has no min_usable specified, TODO before online\n' % index["name"])
        else:
            json_index["min_usable"]=index.get("min_usable")
        json_index["hostinfo"]=json_hostinfo
        json_bsinfo.append(json_index)
    json_group={}
    json_group["group"]=group.lstrip('group_')
    json_group["bsinfo"]=json_bsinfo
    return json_group

