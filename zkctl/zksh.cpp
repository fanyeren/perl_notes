/***************************************************************************
 * 
 * Copyright (c) 2012 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
/**
 * @file zksh.cpp
 * @author huangjindong(com@baidu.com)
 * @date 2012/02/02 12:24:38
 * @brief  zookeeper shell
 * Options:
 *  s: safe     no lock
 *  r: rlock    read lock
 *  w: wlock    write lock
 *  v: version  print version
 *  l: log      log to stderr
 *  d: debug    print more detail msg
 *
 **/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <Configure.h>
#include "zksh.h"
#include "zk_util.h"
#include "version.h"

/** zksh config */
zksh_conf_t g_shconf;
/** no lock */
#define TYPE_NO_LOCK    0
/** read lock */
#define TYPE_READ_LOCK  1
/** write lock */
#define TYPE_WRITE_LOCK 2

/**
 * @brief constructor, init pointers
 *
 * @see 
 * @note 
 * @author huangjindong
 * @date 2012/02/06 22:09:00
**/
zksh_conf_t::zksh_conf_t(): 
    m_manager("zksh_cm"), m_cluster(NULL), m_argv(NULL)
{
    this->script_pos = -1;
    this->m_lock = -1;
}
/**
 * @brief free resource if needed.
 *
 * @see 
 * @note 
 * @author huangjindong
 * @date 2012/02/07 16:30:46
**/
zksh_conf_t::~zksh_conf_t()
{
    if(NULL != m_argv){
        delete []m_argv;
    }
    if(NULL != m_cluster){
        delete m_cluster;
    }
}
/**
 * @brief read zk conf, got zookeeper ip, node and readonly commands
 *
 * @param [in] conf_file   : const char*
 * @return  int 
 * @retval   
 * @see 
 * @note log not init, fprintf error.
 * @author huangjindong
 * @date 2012/02/05 23:04:19
**/
int zksh_conf_t::load_conf(const char *conf_file)
{
    return this->m_conf.parse_conf(conf_file);
}

/**
 * @brief wether the command need to get lock or not.
 *
 * @param [in] argc   : char*
 * @param [in] argv   : char*[]
 * @return  int 
 * @retval   
 * @see 
 * @note 
 * @author huangjindong
 * @date 2012/02/05 23:04:38
**/
int zksh_conf_t::parse_opt(int argc, char *argv[])
{
    /** backup opt, because getopt() will disturb the seq */
    m_argv = new char *[argc];
    for(int i=0;i<argc;i++) {
        m_argv[i] = argv[i];
    }

    script_pos = -1;
    opt_pos = argc;
    //skip the zksh options.
    for(int i=1; i<argc; i++)
    {
        if('-' != g_shconf.m_argv[i][0])
        {
            if(-1 == script_pos) {
                script_pos = i;
            }
            else {
                opt_pos = i;
                break;
            }
        }
    }

    static const char* short_options="srwdv";
    /** parse options */
    static struct option long_options[] = {
        /** safe mode, no lock needed */
        {"safe", 0, 0, 0},
        /** rlock */
        {"rlock", 0, 0, 1},
        /** wlock */
        {"wlock", 0, 0, 2},
        /** debug, print more log */
        {"debug", 0, 0, 3},
        /** version */
        {"version", 0, 0, 4},
        /** NULL */
        {0, 0, 0, 0}
    };
    int option_index = 0;
    opterr = 0;/** does not report error when encounter script's option */
    while(1)
    {
        int c = getopt_long(opt_pos, argv, short_options, long_options, &option_index);
        if(-1 == c)
        {
            break;
        }
        switch(c){
            case 0:
            case 's':
                this->m_lock = TYPE_NO_LOCK;
                break;
            case 1:
            case 'r':
                this->m_lock = TYPE_READ_LOCK;
                break;
            case 2:
            case 'w':
                this->m_lock = TYPE_WRITE_LOCK;
                break;
            case 3:
            case 'd':
                this->m_conf.m_log_level = UL_LOG_DEBUG;
                break;
            case 4:
            case 'v':
                get_version();
                exit(EXIT_SUCCESS);
            default:
                //fprintf(stderr, "unkonw option:%s", optarg);
                break;
        }
    }
    if(this->m_lock < 0){
        fprintf(stderr, "lock type must be specified[-s|safe,-r|rlock,-w|wlock]\n");
        return -1;
    }
    return 1;
}

/**
 * @brief run the scrip in fork process.
 *
 * @param [in/out] argc   : int
 * @param [in/out] argv   : char*[]
 * @return  int 
 * @retval   
 * @see 
 * @note 
 * @author huangjindong
 * @date 2012/02/06 16:11:49
**/
int run_script(int argc)
{
    char *new_argv[ZKSH_CMD_MAX_NUM];
    char buffer[ZKSH_CMD_MAX_LEN];//100k
    int offset = 0;
    if(argc >= ZKSH_CMD_MAX_NUM)
    {
        zk_log(UL_LOG_FATAL, "too many parameters %d>%d", argc, ZKSH_CMD_MAX_NUM);
        return -1;
    }
    if(-1 == g_shconf.script_pos)
    {
        zk_log(UL_LOG_FATAL, "no script specified");
        return -1;
    }
    for(int i=0; i<argc; i++)
    {
        offset += snprintf(buffer+offset, sizeof(buffer)-offset, "%s ", g_shconf.m_argv[i]);
    }
    zk_log(UL_LOG_NOTICE, "raw command:\t%s", buffer);
    //arg[0]: bash
    new_argv[0] = ZKSH_SHELL;
    //arg[1]: script
    new_argv[1] = g_shconf.m_argv[g_shconf.script_pos];
    //arg[2..n-1]: options
    for(int i=g_shconf.opt_pos; i<argc; i++)
    {
        new_argv[2+i-g_shconf.opt_pos] = g_shconf.m_argv[i];
    }
    //arg[n]: NULL
    new_argv[2+argc-g_shconf.opt_pos] = NULL;
    offset = 0;
    for(int i=0; i<2+argc-g_shconf.opt_pos; i++)
    {
        offset += snprintf(buffer+offset, sizeof(buffer)-offset, "%s ", new_argv[i]);
    }
    zk_log(UL_LOG_NOTICE, "real command:\t%s", buffer);
    return execvp(ZKSH_SHELL, new_argv);
}

int zksh_conf_t::read_lock_zk()
{
    int ret = 0;
    //get wlock
    if(ZK_OK != this->m_cluster->acquire_semaphore(ZKSH_WRITE_LOCK))
    {
        return -1;
    }
    //get rlock
    if(ZK_OK != this->m_cluster->acquire_semaphore(ZKSH_READ_LOCK))
    {
        ret = -1;
    }
    //release wlock
    if(ZK_OK != this->m_cluster->release_semaphore(ZKSH_WRITE_LOCK))
    {
        return -1;
    }
    zk_log(UL_LOG_DEBUG, "successfully got read lock");
    return ret;
}
/** read lock: unlock */
int zksh_conf_t::read_unlock_zk()
{
    return this->m_cluster->release_semaphore(ZKSH_READ_LOCK);
}
/** write lock: lock */
int zksh_conf_t::write_lock_zk()
{
    int ret = 0;
    zk_log(UL_LOG_DEBUG, "acquire write lock ing...");
    //get wlock
    if(ZK_OK != this->m_cluster->acquire_semaphore(ZKSH_WRITE_LOCK))
    {
        return -1;
    }
    zk_log(UL_LOG_DEBUG, "got write lock, waiting for no reader");

    //wait no rlock
    vector<string> readers;
    while(1)
    {
        readers.clear();
        if(this->m_cluster->get_semaphore_holders(ZKSH_READ_LOCK, &readers)<0)
        {
            zk_log(UL_LOG_FATAL, "get readers failed");
            ret = -1;
            break;
        }
        if(readers.size() == 0){
            break;
        }
        zk_log(UL_LOG_DEBUG, "current readers: %u", readers.size());
        sleep(1);
    }
    if(0 != ret)
    {
        this->m_cluster->release_semaphore(ZKSH_WRITE_LOCK);
    }
    zk_log(UL_LOG_DEBUG, "successfully got write lock");
    return ret;
}
/** write lock: unlock */
int zksh_conf_t::write_unlock_zk()
{
    return this->m_cluster->release_semaphore(ZKSH_WRITE_LOCK);
}

/**
 * @brief lock zk
 *
 * @return  int 
 * @retval   
 * @see 
 * @note 
 * @author huangjindong
 * @date 2012/02/08 21:20:35
**/
int zksh_conf_t::lock_zk()
{
    int ret = 0;
    if(this->m_lock)
    {
        ret = m_manager.connect(m_conf.zk_cluster, m_conf.zk_root, 
                m_conf.zk_username, m_conf.zk_password);
        if(ZK_OK != ret)
        {
            zk_log(UL_LOG_FATAL, "connect to zk failed, error=%d", ret);
            return -1;
        }
        zk_log(UL_LOG_DEBUG, "connect to zk %s succeed", m_conf.zk_cluster.c_str());
        m_cluster = new zk_cluster(&m_manager);
    }
    switch(this->m_lock)
    {
        case TYPE_NO_LOCK:
            zk_log(UL_LOG_NOTICE, "this operation does NOT need lock");
            break;
        case TYPE_READ_LOCK:
            zk_log(UL_LOG_NOTICE, "trying to get read lock");
            ret = this->read_lock_zk();
            break;
        case TYPE_WRITE_LOCK:
            zk_log(UL_LOG_NOTICE, "trying to get write lock");
            ret = this->write_lock_zk();
            break;
        default:
            ret = -1;
    }
    if(ret == 0){
        zk_log(UL_LOG_NOTICE, "successfully got lock");
    }
    return ret;
}
/**
 * @brief unlock zk
 *
 * @return  int 
 * @retval   
 * @see 
 * @note 
 * @author huangjindong
 * @date 2012/02/08 21:20:41
**/
int zksh_conf_t::unlock_zk()
{
    int ret;
    zk_log(UL_LOG_DEBUG, "try to unlock %d", this->m_lock);
    switch(this->m_lock)
    {
        case TYPE_NO_LOCK:
            ret = 0;
            break;
        case TYPE_READ_LOCK:
            ret = this->read_unlock_zk();
            zk_log(UL_LOG_NOTICE, "release read lock result=%d", ret);
            break;
        case TYPE_WRITE_LOCK:
            ret = this->write_unlock_zk();
            zk_log(UL_LOG_NOTICE, "release write lock result=%d", ret);
            break;
        default:
            ret = -1;
            break;
    }
    zk_log(UL_LOG_DEBUG, "unlock finished");
    return ret;
}

int main(int argc, char *argv[])
{
    /** prepare to run job*/
    //parse command opt
    if(g_shconf.parse_opt(argc, argv) < 0)
    {
        return EXIT_FAILURE;
    }
    // read conf file, get zookeeper ip and node 
    if(g_shconf.load_conf(ZK_DEFAULT_CONF_FILE) < 0)
    {
        fprintf(stderr, "loading conf file [%s/%s] error", 
                g_shconf.get_conf_path(), ZK_DEFAULT_CONF_FILE);
        return EXIT_FAILURE;
    }
     //init log
    zk_log_init(g_shconf.get_conf(), g_shconf.get_log_file());

    /** lock zk if needed */
    if(g_shconf.lock_zk() < 0)
    {
        zk_log(UL_LOG_FATAL, "lock zk error, check conf or repair cluster");
        return EXIT_FAILURE;
    }

    /** invoke scripts */
    zk_log(UL_LOG_DEBUG, "=======================[script started]");
    pid_t pid = fork();
    if(pid == 0)
    {
        if(run_script(argc) < 0)//HERE it is
        {
            zk_log(UL_LOG_FATAL, "exec script failed, error%s", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    int status = 0;
    pid_t gotid;
    do{
        gotid = waitpid(pid, &status, 0);
        zk_log(UL_LOG_DEBUG, "waitpid return pid=%d status=%d", gotid, status);
    }while(gotid != pid);
    zk_log(UL_LOG_NOTICE, "exec script result %d", status);
    zk_log(UL_LOG_DEBUG, "=======================[script done]");

    /** release lock if needed*/
    if(g_shconf.unlock_zk() < 0)
    {
        zk_log(UL_LOG_FATAL, "unlock zk failed");
    }
    zk_log(UL_LOG_DEBUG, "zk job finished");

    if(0 == status) {
        return EXIT_SUCCESS;
    }
    else{
        return EXIT_FAILURE;
    }
}

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
