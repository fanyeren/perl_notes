/***************************************************************************
 *
 * Copyright (c) 2011 Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file zkctl.h
 * @author xiewenhui@baidu.com
 * @date 2011/10/27 10:52:19
 * @brief
 *
 **/


#ifndef __ZKCTL_
#define __ZKCTL_

#include <zkwrapper.h>
#include <index.h>

#include "Configure.h"
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <map>
#include <string>
#include <bsl/map.h>
#include <bsl/var/IVar.h>
#include <bsl/var/Dict.h>
#include <bsl/var/Array.h>
#include <bsl/var/String.h>
#include <bsl/var/JsonDeserializer.h>
#include <bsl/var/JsonSerializer.h>

using std::map;
using std::string;
using namespace zkwrapper;

/**
 * @brief print error msg, including [id] [type] [action], and return EXIT_FAILURE
 */
#define RETURN_ERROR(fmt) { \
    fprintf(stderr, fmt,  \
            request.type.c_str(), request.object_id.c_str(), request.action.c_str()); \
    return EXIT_FAILURE; \
}
/**
 * @brief request object. put all input into this
 */
struct cli_request {
    /** object */
    string type;
    /** type index in g_meta */
    int type_index;
    /** action */
    string action;
    /** action index in g_meta */
    int action_index;

    ////////options
    /** idc-id */
    string idc_id;
    /** group-id */
    string group_id;
    /** slavenode-id */
    string slavenode_id;
    /** index-type-id */
    string index_type_id;
    /** index-partition-id */
    string index_partition_id;
    /** index-id */
    string index_id;
    /** service-id */
    string service_id;
    /** service-instance-id */
    string service_instance_id;
    /** data */
    string data;
    /** resource-num */
    string resource_num;
    /** command-id */
    string command_id;
    /** job id */
    string job_id;
    /** dyndata id */
    string dyndata_id;
    /** dyndata-meta id */
    string dyndata_meta_id;
    /** pretty */
    string pretty;

    /** key array */
    vector<string> keys;
    /** value */
    map<string, string> key_values;
    /** !!! used for array key */
    string key;
    /** !!! object id, used by add */
    string object_id;

    private:
    map<string, string*> option2var;
    //NOTE: to add a variable. add it to cli_process_options function
    public:
    cli_request(){
        option2var["idc-id"] = &idc_id;
        option2var["group-id"] = &group_id;
        option2var["slavenode-id"] = &slavenode_id;
        option2var["index-type-id"] = &index_type_id;
        option2var["index-partition-id"] = &index_partition_id;
        option2var["index-id"] = &index_id;
        option2var["service-id"] = &service_id;
        option2var["service-instance-id"] = &service_instance_id;
        option2var["data"] = &data;
        option2var["command-id"] = &command_id;
        option2var["job-id"] = &job_id;
        option2var["dyndata-id"] = &dyndata_id;
        option2var["dyndata-meta-id"] = &dyndata_meta_id;
        option2var["resource-num"] = &resource_num;
        option2var["pretty"] = &pretty;
    }
    /** got attribute from option */
    string *option_to_var(string option){
        map<string, string*>::iterator it = option2var.find(option);
        if(option2var.end() == it){
            return NULL;
        }
        return it->second;
    }
};
/**
 * @brief 解析命令行输入,将数据放入request中
 *
 * @param [in/out] argc   : int
 * @param [in/out] argv   : char*[]
 * @param [in/out] request   : cli_request&
 * @return  int 
 * @author huangjindong
 * @date 2012/02/29 19:55:29
**/
int cli_parse_options(int argc, char *argv[], cli_request &request);

/**
 * @brief 执行命令 
 *
 * @param [in/out] request   : cli_request&
 * @return  int 
 * @author huangjindong
 * @date 2012/02/29 19:55:55
**/
int cli_invoke(cli_request &request);

/**
 * @brief 辅助函数,打印字符串, 换行分隔 
 *
 * @param [in/out] values   : vector<string>&
 * @return  void 
 * @author huangjindong
 * @date 2012/02/29 19:56:05
**/
void cli_print_list(vector<string> &values);

/**
 * @brief 辅助函数,打印字符串, 空格分隔 
 *
 * @param [in/out] values   : vector<string>&
 * @return  void 
 * @author huangjindong
 * @date 2012/02/29 19:56:33
**/
void cli_print_token(vector<string> &values);

/**
 * @brief 辅助函数,打印对象ID, 换行分隔 
 *
 * @param [in/out] nodes   : vector<T* >&
 * @return  void 
 * @author huangjindong
 * @date 2012/02/29 19:57:08
**/
template <class T>
void cli_print_list(vector<T*> &nodes)
{
    for(uint32_t i=0; i<nodes.size(); i++)
    {
        fprintf(stdout, "%s\n", nodes[i]->get_id().c_str());
    }
}

///////////////object operation///////////////////////
template <class T>
/** list */
int cli_list(cli_request &request, zk_search_cluster_manager &manager)
{
    int ret;
    vector<T*> all_objects;
    bsl::ResourcePool local_rp;
    ret = manager.get_all_objects<T>(&all_objects, local_rp);
    if(ret != ZK_OK){
        fprintf(stderr, "%s list failed\n", request.type.c_str());
        return EXIT_FAILURE;
    }
    cli_print_list<T>(all_objects);
    return EXIT_SUCCESS;
}
template <class T>
/** add */
int cli_add(cli_request &request, zk_search_cluster_manager &manager)
{
    int ret;
    T *new_object;
    bsl::ResourcePool local_rp;
    bsl::var::JsonDeserializer jd(local_rp);
    bsl::var::Dict *result_dict;
    try{
        bsl::var::IVar &tmp_meta = jd.deserialize(request.data.c_str());
        result_dict = &((bsl::var::Dict&)tmp_meta);
    } catch (bsl::Exception &e) {
        fprintf(stderr,"invalid meta data. deserialze from json failed.\n");
        return EXIT_FAILURE;
    }

    ret = manager.new_object<T>(request.object_id, &new_object, local_rp);
    if(ret != ZK_OK) {
        RETURN_ERROR("%s %s %s failed, mallo fail, refer log for detail\n");
    }

    ret = manager.add_object(new_object);
    if (ret != ZK_OK) {
        RETURN_ERROR("%s %s %s failed, it may be exist already\n");
    }

    ret = new_object->set_meta_all(*result_dict);
    if(ret != ZK_OK) {
        fprintf(stderr,"%s add failed.\n", request.type.c_str());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
template <class T>
/** del */
int cli_del(cli_request &request ,zk_search_cluster_manager &manager)
{
    int ret;
    T *object;
    bsl::ResourcePool local_rp;
    ret = manager.get_object_by_id<T>(request.object_id, &object, local_rp);
    if(ret != ZK_OK){
        RETURN_ERROR("%s %s %s failed, can not find this\n");
    }
    ret = manager.delete_object(object);
    if(ret != ZK_OK){
        fprintf(stderr, "%s del failed, probally has child\n", request.type.c_str());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
///////////////meta-operation////////////////////
template <class T>
/** add-meta-attr */
int cli_add_meta_attr(cli_request &request, zk_search_cluster_manager &manager)
{
    int ret;
    T *object;
    bsl::ResourcePool local_rp;
    ret = manager.get_object_by_id<T>(request.object_id, &object, local_rp);
    if(ret != ZK_OK){
        RETURN_ERROR("%s %s %s failed, can not find this\n");
    }
    ret = object->add_meta_attr(request.key_values);
    if(ret != ZK_OK){
        fprintf(stderr, "%s %s failed, this key field may be exist already\n",
                request.type.c_str(), request.action.c_str());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
template <class T>
/** del-meta-attr */
int cli_del_meta_attr(cli_request &request, zk_search_cluster_manager &manager)
{
    int ret;
    T *object;
    bsl::ResourcePool local_rp;
    ret = manager.get_object_by_id<T>(request.object_id, &object, local_rp);
    if(ret != ZK_OK){
        RETURN_ERROR("%s %s %s failed, can not find this\n");
    }
    ret = object->del_meta_attr(request.keys);
    if(ret != ZK_OK){
        fprintf(stderr, "%s %s failed, this key field may be not exist\n",
                request.type.c_str(), request.action.c_str());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
template <class T>
/** set-meta-attr */
int cli_set_meta_attr(cli_request &request, zk_search_cluster_manager &manager)
{
    int ret;
    T *object;
    bsl::ResourcePool local_rp;
    ret = manager.get_object_by_id<T>(request.object_id, &object, local_rp);
    if(ret != ZK_OK){
        RETURN_ERROR("%s %s %s failed, can not find this\n");
    }
    ret = object->set_meta_attr(request.key_values);
    if(ret != ZK_OK){
        RETURN_ERROR("%s %s %s failed, this key field may be not exist\n");
    }
    return EXIT_SUCCESS;

}
template <class T>
/** get-meta-attr */
int cli_get_meta_attr(cli_request &request, zk_search_cluster_manager &manager)
{
    int ret;
    T *object;
    bsl::ResourcePool local_rp;
    //1. get object
    ret = manager.get_object_by_id<T>(request.object_id, &object, local_rp);
    if(ret != ZK_OK){
        RETURN_ERROR("%s %s %s failed, can not find this\n");
    }
    vector<string> values;
    //2. get attr
    ret = object->get_meta_attr(request.keys, values);
    if(ret != ZK_OK){
        fprintf(stderr, "%s %s failed, this key field may not be exist\n",
                request.type.c_str(), request.action.c_str());
        return EXIT_FAILURE;
    }
    //3. print attr
    cli_print_token(values);
    return EXIT_SUCCESS;
}
template <class T>
/** set-meta-all */
int cli_set_meta_all(cli_request &request, zk_search_cluster_manager &manager)
{
    int ret;
    T *object;
    bsl::ResourcePool local_rp;
    bsl::var::JsonDeserializer jd(local_rp);
    bsl::var::Dict *result_dict;
    ret = manager.get_object_by_id<T>(request.object_id, &object, local_rp);
    if(ret != ZK_OK){
        RETURN_ERROR("%s \"%s\" %s failed, can not find this\n");
    }
    try{
        bsl::var::IVar &tmp_meta = jd.deserialize(request.data.c_str());
        result_dict = &((bsl::var::Dict&)tmp_meta);
    } catch (bsl::Exception &e) {
        fprintf(stderr,"invalid meta data. deserialze from json failed.\n");
        return EXIT_FAILURE;
    }

    ret = object->set_meta_all(*result_dict, &request.key_values);
    if(ret != ZK_OK){
        RETURN_ERROR("%s \"%s\" %s failed, refer log for detail\n");
    }
    return EXIT_SUCCESS;

}
/** print json pretty */
void print_json(bsl::var::IVar &var);

template <class T>
/** get-meta-all */
int cli_get_meta_all(cli_request &request, zk_search_cluster_manager &manager)
{
    int ret;
    T *object;
    bsl::ResourcePool local_rp;
    bsl::var::JsonSerializer js;
    bsl::AutoBuffer tmp_buf;
    bsl::var::Dict *result_dict;

    ret = manager.get_object_by_id<T>(request.object_id, &object, local_rp);
    if(ret != ZK_OK){
        RETURN_ERROR("%s %s %s failed, can not find this\n");
    }
    ret = object->get_meta_all(&result_dict, local_rp);
    if(ret != ZK_OK){
        RETURN_ERROR("%s %s %s failed, refer log for detail\n");
    }
    if(request.pretty.empty()){
        try
        {
            js.serialize(*result_dict, tmp_buf);
        }
        catch (bsl::Exception &e)
        {
            fprintf(stderr,"get-meta-all failed, invalid json data\n");
            return EXIT_FAILURE;
        }
        fprintf(stdout,"%s\n", tmp_buf.c_str());
    }
    else{
        print_json(*result_dict);
    }
    return EXIT_SUCCESS;
}
template <class T>
int cli_get_meta_keys(cli_request &request, zk_search_cluster_manager &manager)
{
    int ret;
    T *object;
    vector<string> keys;
    bsl::ResourcePool local_rp;
    ret = manager.get_object_by_id<T>(request.object_id, &object, local_rp);
    if(ret != ZK_OK){
        RETURN_ERROR("%s %s %s failed, can not find this\n");
    }
    ret = object->get_meta_keys(keys, local_rp);
    if(ret != ZK_OK){
        RETURN_ERROR("%s %s %s failed, refer log for detail\n");
    }
    cli_print_list(keys);
    return EXIT_SUCCESS;
}
/////////////////////////////////semaphore-operation////////////////////////////////////////
template <class T>
/** creaet-semaphore */
int cli_create_semaphore(cli_request &request ,zk_search_cluster_manager &manager)
{
    int ret;
    T *object;
    bsl::ResourcePool local_rp;
    ret = manager.get_object_by_id<T>(request.object_id, &object, local_rp);
    if(ret != ZK_OK){
        RETURN_ERROR("%s %s %s failed, can not find this\n");
    }
    ret = object->create_semaphore(request.key, atoi(request.resource_num.c_str()));
    if(ret == ZK_SEMA_EXISTS){
        fprintf(stderr, "create semaphore failed, semaphore %s already exist\n",
                request.key.c_str());
        return EXIT_FAILURE;
    }
    else if(ret != ZK_OK){
        fprintf(stderr, "create semaphore %s failed\n", request.object_id.c_str());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
template <class T>
/** destroy-semaphore */
int cli_destroy_semaphore(cli_request &request ,zk_search_cluster_manager &manager)
{
    int ret;
    T *object;
    bsl::ResourcePool local_rp;
    ret = manager.get_object_by_id<T>(request.object_id, &object, local_rp);
    if(ret != ZK_OK){
        RETURN_ERROR("%s %s %s failed, can not find this\n");
    }
    ret = object->destroy_semaphore(request.key);
    if(ret != ZK_OK){
        RETURN_ERROR("%s %s %s failed\n");
    }
    return EXIT_SUCCESS;
}
template <class T>
/** list-semaphore */
int cli_list_semaphore(cli_request &request ,zk_search_cluster_manager &manager)
{
    int ret;
    T *object;
    bsl::ResourcePool local_rp;
    ret = manager.get_object_by_id<T>(request.object_id, &object, local_rp);
    if(ret != ZK_OK){
        RETURN_ERROR("%s %s %s failed, can not find this\n");
    }
    vector<string> semaphore_vector;
    ret = object->get_all_semaphores(&semaphore_vector, local_rp);
    if(ret != ZK_OK){
        RETURN_ERROR("%s %s %s failed, refer log for detail\n");
    }
    cli_print_list(semaphore_vector);
    return EXIT_SUCCESS;
}
template <class T>
/** list-semaphore-holder */
int cli_list_semaphore_holder(cli_request &request ,zk_search_cluster_manager &manager)
{
    int ret;
    T *object;
    bsl::ResourcePool local_rp;
    ret = manager.get_object_by_id<T>(request.object_id, &object, local_rp);
    if(ret != ZK_OK){
        RETURN_ERROR("%s %s %s failed, can not find this\n");
    }
    vector<string> semaphore_holder_vector;
    ret = object->get_semaphore_holders(request.key, &semaphore_holder_vector);
    if(ret != ZK_OK){
        RETURN_ERROR("%s %s %s failed, refer log for detail\n");
    }
    cli_print_list(semaphore_holder_vector);
    return EXIT_SUCCESS;
}
template <class T>
/** set-semaphore-resource-num */
int cli_set_semaphore_resource_num(cli_request &request ,zk_search_cluster_manager &manager)
{
    int ret;
    T *object;
    bsl::ResourcePool local_rp;
    ret = manager.get_object_by_id<T>(request.object_id, &object, local_rp);
    if(ret != ZK_OK){
        fprintf(stderr, "%s %s failed, can not find this %s\n",
                request.type.c_str(), request.action.c_str(), request.type.c_str());
        return EXIT_FAILURE;
    }
    ret = object->set_semaphore_resource_num(request.key, atoi(request.resource_num.c_str()));
    if(ret != ZK_OK){
        RETURN_ERROR("%s %s %s failed\n"); 
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
template <class T>
/** get-semaphore-resource-num */
int cli_get_semaphore_resource_num(cli_request &request, zk_search_cluster_manager &manager)
{
    int ret;
    T *object;
    bsl::ResourcePool local_rp;
    ret = manager.get_object_by_id<T>(request.object_id, &object, local_rp);
    if(ret != ZK_OK){
        fprintf(stderr, "%s %s failed, can not find this %s\n",
                request.type.c_str(), request.action.c_str(), request.type.c_str());
        return EXIT_FAILURE;
    }
    int resource_num;
    ret = object->get_semaphore_resource_num(request.key, &resource_num);
    if(ret != ZK_OK){
        RETURN_ERROR("%s %s %s failed\n"); 
        return EXIT_FAILURE;
    }
    fprintf(stdout, "%d\n", resource_num);
    return EXIT_SUCCESS;
}
////////////////////////////////state operation ///////////////////////////
template <class T, class Func>
/** add-xxx-state-attr */
int cli_state_add_attr(cli_request &request, zk_search_cluster_manager &manager, Func func)
{
    int ret;
    bsl::ResourcePool local_rp;
    T *object;
    ret = manager.get_object_by_id<T>(request.object_id, &object, local_rp);
    if(ret != ZK_OK){
        RETURN_ERROR("%s %s %s failed, can not find this\n");
    }
    ret = (object->*func)(request.key_values);
    if(ret != ZK_OK){
        RETURN_ERROR("%s %s %s failed, this key field may be exist already\n");
    }
    return EXIT_SUCCESS;
}
template <class T, class Func>
/** del-xxx-state-attr */
int cli_state_del_attr(cli_request &request, zk_search_cluster_manager &manager, Func func)
{
    int ret;
    bsl::ResourcePool local_rp;
    T *object;
    ret = manager.get_object_by_id<T>(request.object_id, &object, local_rp);
    if(ret != ZK_OK){
        RETURN_ERROR("%s %s %s failed, can not find this\n");
    }
    ret = (object->*func)(request.keys);
    if(ret != ZK_OK){
        RETURN_ERROR("%s %s %s failed, this key may not be exist\n");
    }
    return EXIT_SUCCESS;
}
template <class T, class Func>
/** get-xxx-state-attr */
int cli_state_get_attr(cli_request &request, zk_search_cluster_manager &manager, Func func)
{
    int ret;
    vector<string> values;
    bsl::ResourcePool local_rp;
    T *object;
    ret = manager.get_object_by_id<T>(request.object_id, &object, local_rp);
    if(ret != ZK_OK){
        RETURN_ERROR("%s %s %s failed, can not find this\n");
    }
    ret = (object->*func)(request.keys, values);
    if(ret != ZK_OK){
        RETURN_ERROR("%s %s %s failed, this key field may be not exist\n");
    }
    cli_print_token(values);
    return EXIT_SUCCESS;
}
template <class T, class Func>
/** set-xxx-state-attr */
int cli_state_set_attr(cli_request &request, zk_search_cluster_manager &manager, Func func)
{
    int ret;
    bsl::ResourcePool local_rp;
    T *object;
    ret = manager.get_object_by_id<T>(request.object_id, &object, local_rp);
    if(ret != ZK_OK){
        RETURN_ERROR("%s %s %s failed, can not find this\n");
    }

    ret = (object->*func)(request.key_values);
    if(ret != ZK_OK){
        RETURN_ERROR("%s %s %s failed, refer log for detail\n");
    }
    return EXIT_SUCCESS;
}
template <class T, class Func>
/** set-xxx-state-all */
int cli_state_set_all(cli_request &request, zk_search_cluster_manager &manager, Func func)
{
    int ret;
    T *object;
    bsl::ResourcePool local_rp;
    bsl::var::JsonDeserializer jd(local_rp);
    bsl::var::Dict *result_dict;
    ret = manager.get_object_by_id<T>(request.object_id, &object, local_rp);
    if(ret != ZK_OK){
        return EXIT_FAILURE;
    }
    try{
        bsl::var::IVar &tmp_meta = jd.deserialize(request.data.c_str());
        result_dict = &((bsl::var::Dict&)tmp_meta);
    } catch (bsl::Exception &e) {
        fprintf(stderr,"invalid meta data. deserialze from json failed.\n");
        return EXIT_FAILURE;
    }

    ret = (object->*func)(*result_dict);
    if(ret != ZK_OK){
        fprintf(stderr, "%s %s failed\n",
                request.type.c_str(), request.action.c_str());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
template <class T, class Func>
/** get-xxx-state-all */
int cli_state_get_all(cli_request &request, zk_search_cluster_manager &manager, Func func)
{
    int ret;
    T *object;
    bsl::ResourcePool local_rp;
    bsl::var::JsonSerializer js;
    bsl::AutoBuffer tmp_buf;
    bsl::var::Dict *result_dict;

    ret = manager.get_object_by_id<T>(request.object_id, &object, local_rp);
    if(ret != ZK_OK){
        RETURN_ERROR("%s %s %s failed, can not find this\n");
    }
    ret = (object->*func)(&result_dict, local_rp);
    if(ret != ZK_OK){
        fprintf(stderr, "%s %s failed\n",
                request.type.c_str(), request.action.c_str());
        return EXIT_FAILURE;
    }
    if(request.pretty.empty()){
        try
        {
            js.serialize(*result_dict, tmp_buf);
        }
        catch (bsl::Exception &e)
        {
            fprintf(stderr,"get-meta-all failed, invalid json data\n");
            return EXIT_FAILURE;
        }
        fprintf(stdout,"%s\n", tmp_buf.c_str());
    }
    else{
        print_json(*result_dict);
    }
    return EXIT_SUCCESS;
}

#endif

