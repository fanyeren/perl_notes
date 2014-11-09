/***************************************************************************
 * 
 * Copyright (c) 2012 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
/**
 * @file zksh.h
 * @author huangjindong(com@baidu.com)
 * @date 2012/02/05 22:57:56
 * @brief 
 *  
 **/

#ifndef  __ZKSH_H_
#define  __ZKSH_H_
#include <string>
#include <vector>
using namespace std;
#undef LOG_DEBUG
#undef LOG_INFO
#include <zkwrapper.h>
using namespace zkwrapper;
#include "zk_util.h"

/** zk shell's implementation: bash */
#define ZKSH_SHELL          "bash"


/**
 * @brief zksh conf
 */
class zksh_conf_t
{
    /** zk server config */
    zkctl_conf m_conf;
    /** zk handler */
    zk_cluster_manager m_manager;
    /** zk cluster */
    zk_cluster * m_cluster;
    /** need lock or not */
    int m_lock;

    public:
    /** backup of argv, because getopt() disturb the seq */
    char **m_argv;
    /** script file position in argv */
    int script_pos;
    /** script option position in argv */
    int opt_pos;

    public:
    ////////////////////Basic Function/////////////////
    /** constructor */
    zksh_conf_t();
    /** deconstructor */
    ~zksh_conf_t();
    /** load conf file */
    int load_conf(const char *conf_file);
    /** parse opt */
    int parse_opt(int argc, char *argv[]);

    ////////////////////Path and Log///////////////////
    /** conf */
    zkctl_conf& get_conf()           { return m_conf;   }
    /** conf path */
    const char* get_conf_path()      { return m_conf.m_conf_path; }
    /** log file */
    const char* get_log_file()       { return m_conf.m_log_zksh; }

    ////////////////////Lock related///////////////////
    /** lock */
    int lock_zk();
    /** unlock */
    int unlock_zk();

    private:
    /** need lock or not */
    int is_need_lock()              { return  m_lock;  }
    /** read lock */
    int read_lock_zk();
    /** read lock unlock */
    int read_unlock_zk();
    /** write lock */
    int write_lock_zk();
    /** write lock unlock */
    int write_unlock_zk();
};

#endif  //__ZKSH_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
