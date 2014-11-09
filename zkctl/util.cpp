/***************************************************************************
 * 
 * Copyright (c) 2012 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/



/**
 * @file util.cpp
 * @author huangjindong(com@baidu.com)
 * @date 2012/03/05 22:03:28
 * @brief 
 *  
 **/
#include <zookeeper.h>
#include <zookeeper_log.h>
#include <vector>
#include <string>
using namespace std;

struct ACL_vector acl;
struct ACL acl_data;

int update_acl(zhandle_t *handle, const string &path)
{
    int ret;
    String_vector children;
    ret = zoo_get_children(handle,          /* zookeeper handle */
            path.c_str(),    /* zk node path */
            0,
            &children);      /* children vector */
    if(ret != ZOK) {
        LOG_ERROR(("zoo_get_children() error. ret=%s, path=%s", zerror(ret), path.c_str()));
        //return -1;
    }
    if(children.count != 0) {
        for(int i = 0; i < children.count; i++) {
            string child_path = path + "/" + children.data[i];
            ret = update_acl(handle, child_path);
            if(ret != 0) {
                //return ret;
            }
        }
    }

    struct Stat stat;
    char tmp_buf[102400];
    int tmp_buf_len = sizeof(tmp_buf);
    ret = zoo_get(handle,           /* zookeeper handle */
            path.c_str(),     /* zk node path */
            0,                /* watch flag */
            tmp_buf,          /* data value buffer */
            &tmp_buf_len,     /* data value buffer length */
            &stat);           /* state struct */
    if(ret != ZOK) {
        LOG_ERROR(("zoo_get() error. ret=%d, path=%s", zerror(ret), path.c_str()));
        //return ret;
    }
    ret = zoo_set_acl(handle,        /* zookeeper handle */
            path.c_str(),  /* zk node path */
            stat.version,
            &acl); /* zk node version */
    if(ret != ZOK) {
        LOG_ERROR(("zoo_delete() error. ret=%s, path=%s", zerror(ret), path.c_str()));
        return ret;
    }
    return 0;
}
int main(int argc, char *argv[]){
    if(argc < 4){
        fprintf(stderr, "Usage %s zkaddr nodepath acl\n", argv[0]);
        fprintf(stderr, "eg: %s 127.0.0.1:2181 /test_cluster autooperationproject:kIcNgCNpAGV+EBlmL/2kJJkgMKM=\n", argv[0]);
        return EXIT_FAILURE;
    }
    string server = argv[1];
    string node = argv[2];
    char * digest = argv[3];
    zhandle_t *zhandle_ = zookeeper_init(server.c_str(), NULL, 100*1000, NULL, NULL, 0);

    acl_data.perms = ZOO_PERM_ALL;
    acl_data.id.scheme = "digest";
    acl_data.id.id=digest;
    acl.count = 1;
    acl.data = &acl_data;
    update_acl(zhandle_, node);
    return 0;
}

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
