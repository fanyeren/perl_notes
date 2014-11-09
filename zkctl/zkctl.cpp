/***************************************************************************
 *
 * Copyright (c) 2011 Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file zkctl.cpp
 * @author xiewenhui@baidu.com
 * @date 2011/10/27 10:52:19
 * @brief
 *
 **/

#include "zkctl.h"
#include "zk_util.h"

#include <string.h>
#include <unistd.h>

/** conf */
zkctl_conf g_conf;

/** print help msg */
void print_help(void) {
    fprintf(stderr, "Usage: zkctl.bin <OBJECT> <ACTIONS> [options]\n\n");
    fprintf(stderr, "Attention: use zkctl instead of zkctl.bin\n");
}
/**
 * @brief action implementations
 */
typedef int (*cli_action)(cli_request&, zk_search_cluster_manager &manager);

///////////////special/////////////////////////
/**
 * @brief cluster
 */
int cli_cluster_repair(cli_request &request, zk_search_cluster_manager &manager);
/**
 * @brief cluster 2
 */
int cli_cluster_destroy(cli_request &request, zk_search_cluster_manager &mamanger);
/**
 * @brief slavenode
 */
int cli_slavenode_list(cli_request &request,zk_search_cluster_manager &manager);
/**
 * @brief slavenode 2
 */
int cli_slavenode_list_service_instance(cli_request &request, zk_search_cluster_manager &manager);
/**
 * @brief index-type 
 */
int cli_index_type_list_partition(cli_request &request, zk_search_cluster_manager &manager);
/**
 * @brief index-partition
 */
int cli_index_partition_list_index(cli_request &request, zk_search_cluster_manager &manager);
/**
 * @brief service
 */
int cli_service_list_service_instance(cli_request &request, zk_search_cluster_manager &manager);
/**
 * @brief service-instance
 */
int cli_add_service_instance(cli_request &request, zk_search_cluster_manager &manager);
/**
 * @brief service-instance is valid
 */
int cli_service_instance_is_valid(cli_request &request, zk_search_cluster_manager &manager);
/**
 * @brief dyndata-meta
 */
int cli_list_dyndata(cli_request &request, zk_search_cluster_manager &manager);
/**
 * @brief dyndata add
 */
int cli_add_dyndata(cli_request &request, zk_search_cluster_manager &manager);
/**
 * @brief placement-plan dump
 */
int cli_placement_plan_dump(cli_request &request, zk_search_cluster_manager &manager);
/**
 * @brief service-instance list
 */
int cli_service_instance_list(cli_request &request, zk_search_cluster_manager &manager);
/**
 * @brief service-instance dump
 */
int cli_service_instance_dump(cli_request &request, zk_search_cluster_manager &manager);

///////////////service-> default-planned-state ///////////////////
typedef int (zk_service::*service_state_get_all)(bsl::var::Dict **meta, bsl::ResourcePool &rp);
int cli_service_get_planned_state_all(cli_request &request, zk_search_cluster_manager &manager) {
    return cli_state_get_all<zk_service, service_state_get_all>(request, manager, 
            &zk_service::get_default_planned_state_all);
}
typedef int (zk_service::*service_state_set_all)(const bsl::var::Dict &meta);
int cli_service_set_planned_state_all(cli_request &request, zk_search_cluster_manager &manager) {
    return cli_state_set_all<zk_service, service_state_set_all>(request, manager, 
            &zk_service::set_default_planned_state_all);
}
typedef int (zk_service::*service_state_get_attr)(const vector<string> &, vector<string> &);
int cli_service_get_planned_state_attr(cli_request &request, zk_search_cluster_manager &manager) {
    return cli_state_get_attr<zk_service, service_state_get_attr>(request, manager,
            &zk_service::get_default_planned_state_attr);
}
typedef int (zk_service::*service_state_set_attr)(const map<string,string> &key_values);
int cli_service_set_planned_state_attr(cli_request &request, zk_search_cluster_manager &manager) {
    return cli_state_set_attr<zk_service, service_state_set_attr>(request, manager, 
            &zk_service::set_default_planned_state_attr);
}
typedef int (zk_service::*zk_service_state_add_attr)(const map<string,string> &key_values);
int cli_service_add_planned_state_attr(cli_request &request, zk_search_cluster_manager &manager) {
    return cli_state_add_attr<zk_service, zk_service_state_add_attr>(request, manager, 
            &zk_service::add_default_planned_state_attr);
}
typedef int (zk_service::*zk_service_state_del_default_attr)(const vector<string> &keys);
int cli_service_del_planned_state_attr(cli_request &request, zk_search_cluster_manager &manager) {
    return cli_state_del_attr<zk_service, zk_service_state_del_default_attr>(request, manager, 
            &zk_service::del_default_planned_state_attr);
}
///////////////instance planned/current-state ///////////////////////
typedef int (zk_service_instance::*zk_sinstance_get_all)(bsl::var::Dict **, bsl::ResourcePool &);
int cli_sinstance_get_planned_state_all(cli_request &request, zk_search_cluster_manager &manager) {
    return cli_state_get_all<zk_service_instance, zk_sinstance_get_all>(request, manager, 
            &zk_service_instance::get_planned_state_all);
}
int cli_sinstance_get_current_state_all(cli_request &request, zk_search_cluster_manager &manager) {
    return cli_state_get_all<zk_service_instance, zk_sinstance_get_all>(request, manager, 
            &zk_service_instance::get_current_state_all);
}
typedef int (zk_service_instance::*zk_sinstance_set_all)(const bsl::var::Dict &meta);
int cli_sinstance_set_planned_state_all(cli_request &request, zk_search_cluster_manager &manager) {
    return cli_state_set_all<zk_service_instance, zk_sinstance_set_all>(request, manager, 
            &zk_service_instance::set_planned_state_all);
}
int cli_sinstance_set_current_state_all(cli_request &request, zk_search_cluster_manager &manager) {
    return cli_state_set_all<zk_service_instance, zk_sinstance_set_all>(request, manager, 
            &zk_service_instance::set_current_state_all);
}
typedef int (zk_service_instance::*zk_sinstance_get_attr)(const vector<string> &, vector<string> &);
int cli_sinstance_get_planned_state_attr(cli_request &request, zk_search_cluster_manager &manager) {
    return cli_state_get_attr<zk_service_instance, zk_sinstance_get_attr>(request, manager, 
            &zk_service_instance::get_planned_state_attr);
}
int cli_sinstance_get_current_state_attr(cli_request &request, zk_search_cluster_manager &manager) {
    return cli_state_get_attr<zk_service_instance, zk_sinstance_get_attr>(request, manager, 
            &zk_service_instance::get_current_state_attr);
}
typedef int (zk_service_instance::*zk_sinstance_set_attr)(const map<string,string> &key_values);
int cli_sinstance_set_planned_state_attr(cli_request &request, zk_search_cluster_manager &manager) {
    return cli_state_set_attr<zk_service_instance, zk_sinstance_set_attr>(request, manager, 
            &zk_service_instance::set_planned_state_attr);
}
int cli_sinstance_set_current_state_attr(cli_request &request, zk_search_cluster_manager &manager) {
    return cli_state_set_attr<zk_service_instance, zk_sinstance_set_attr>(request, manager,
            &zk_service_instance::set_current_state_attr);
}
typedef int (zk_service_instance::*zk_sinstance_add_attr)(const map<string, string> &key_values);
int cli_sinstance_add_planned_state_attr(cli_request &request, zk_search_cluster_manager &manager) {
    return cli_state_add_attr<zk_service_instance, zk_sinstance_add_attr>(request, manager,
            &zk_service_instance::add_planned_state_attr);
}
int cli_sinstance_add_current_state_attr(cli_request &request, zk_search_cluster_manager &manager) {
    return cli_state_add_attr<zk_service_instance, zk_sinstance_add_attr>(request, manager,
            &zk_service_instance::add_current_state_attr);
}
typedef int (zk_service_instance::*zk_sinstance_del_attr)(const vector<string> &keys);
int cli_sinstance_del_planned_state_attr(cli_request &request, zk_search_cluster_manager &manager) {
    return cli_state_del_attr<zk_service_instance, zk_sinstance_del_attr>(request, manager,
            &zk_service_instance::del_planned_state_attr);
}
int cli_sinstance_del_current_state_attr(cli_request &request, zk_search_cluster_manager &manager) {
    return cli_state_del_attr<zk_service_instance, zk_sinstance_del_attr>(request, manager, 
            &zk_service_instance::del_current_state_attr);
}
////////////////command
int command_generateid(cli_request &request, zk_search_cluster_manager &manager);
int command_add(cli_request &request, zk_search_cluster_manager &manager);
int command_del(cli_request &request, zk_search_cluster_manager &manager);
int command_list(cli_request &request, zk_search_cluster_manager &manager);
int command_add_meta_attr(cli_request &request, zk_search_cluster_manager &manager);
int command_del_meta_attr(cli_request &request, zk_search_cluster_manager &manager);
int command_set_meta_attr(cli_request &request, zk_search_cluster_manager &manager);
int command_get_meta_attr(cli_request &request, zk_search_cluster_manager &manager);
int command_get_meta_all(cli_request &request, zk_search_cluster_manager &manager);
int command_set_meta_all(cli_request &request, zk_search_cluster_manager &manager);

/**
 * @brief command OPTION
 * eg: --key --resource-name
 */
struct cli_zk_option{
    /** name */
    string name;
    /** description, structure fields */
    string description;
    /** required or not */
    bool required;
    /** has arg */
    bool has_arg;
    /** specified from command line */
    bool specified;
};
/**
 * @brief command ACTION
 * eg: add-meta-attr add del
 */
struct cli_zk_action{
    /** action name */
    string action;
    /** action implementation */
    cli_action action_impl;
    /** action options */
    cli_zk_option options[256];
};
/**
 * @brief command type
 * eg: idc slavenode
 */
struct cli_zk_type{
    /** zk_object type */
    string type;
    /** action list */
    cli_zk_action actions[256];
};

/**
 * @brief super important part
 */
struct cli_zk_type g_meta[] = {
    {//cluster
        "cluster",//type
        {//actions
            {
                "create",
                cli_cluster_repair,//impl
                {}
            },
            {
                "repair",//action
                cli_cluster_repair,//impl
                {}//options
            },
            {
                "destroy",//action
                cli_cluster_destroy,//impl
                {}//options
            },
            ////////////////meta
            {
                "add-meta-attr",//action
                cli_add_meta_attr<zk_cluster>,//impl
                {//options
                    { "key", "", true, true, false },
                    { "value", "", true, true, false }
                },
            },
            {
                "del-meta-attr",//action
                cli_del_meta_attr<zk_cluster>,//impl
                {//options
                    { "key", "", true, true, false }
                }
            },
            {
                "set-meta-attr",//action
                cli_set_meta_attr<zk_cluster>,//impl
                {//options
                    { "key", "", true, true, false },
                    { "value", "", true, true, false }
                }
            },
            {
                "get-meta-attr",//action
                cli_get_meta_attr<zk_cluster>,//impl
                {//options
                    { "key", "", true, true, false }
                }
            },
            {
                "set-meta-all",//action
                cli_set_meta_all<zk_cluster>,//impl
                {//options
                    { "data", "{}", true, true, false }
                }
            },
            {
                "get-meta-all",//action
                cli_get_meta_all<zk_cluster>,//impl
                {//options
                    { "pretty", "", false, false, false }
                }
            },
            //semephore
            {
                "create-semaphore",//action
                cli_create_semaphore<zk_cluster>,//impl
                {//options
                    { "key", "", true, true, false },
                    { "resource-num", "", true, true, false }
                }
            },
            {
                "destroy-semaphore",//action
                cli_destroy_semaphore<zk_cluster>,//impl
                {//options
                    { "key", "", true, true, false }
                }
            },
            {
                "get-semaphore-resource-num",//action
                cli_get_semaphore_resource_num<zk_cluster>,//impl
                {//options
                    { "key", "", true, true, false }
                }
            },
            {
                "set-semaphore-resource-num",//action
                cli_set_semaphore_resource_num<zk_cluster>,//impl
                {//options
                    { "key", "", true, true, false },
                    { "resource-num", "", true, true, false }
                }
            },
            {
                "list-semaphore",//action
                cli_list_semaphore<zk_cluster>,//impl
                {}//options
            },
            {
                "list-semaphore-holder",//action
                cli_list_semaphore_holder<zk_cluster>,//impl
                {//options
                    { "key", "", true, true, false }
                }
            }
        }
    },
    {//idc
        "idc",//type
        {//actions
            {
                "list",//action
                cli_list<zk_idc>,//impl
                {}//options
            },
            {
                "add",//action
                cli_add<zk_idc>,//impl
                {//options
                    { "idc-id", "", true, true, false },
                    { "data", "{}", true, true, false }
                }
            },
            {
                "del",//action
                cli_del<zk_idc>,//impl
                {//options
                    { "idc-id", "", true, true, false }
                }
            },
            ////////////////meta
            {
                "add-meta-attr",//action
                cli_add_meta_attr<zk_idc>,//impl
                {//options
                    { "idc-id", "", true, true, false },
                    { "key", "", true, true, false },
                    { "value", "", true, true, false }
                },
            },
            {
                "del-meta-attr",//action
                cli_del_meta_attr<zk_idc>,//impl
                {//options
                    { "idc-id", "", true, true, false },
                    { "key", "", true, true, false }
                }
            },
            {
                "set-meta-attr",//action
                cli_set_meta_attr<zk_idc>,//impl
                {//options
                    { "idc-id", "", true, true, false },
                    { "key", "", true, true, false },
                    { "value", "", true, true, false }
                }
            },
            {
                "get-meta-attr",//action
                cli_get_meta_attr<zk_idc>,//impl
                {//options
                    { "idc-id", "", true, true, false },
                    { "key", "", true, true, false }
                }
            },
            {
                "set-meta-all",//action
                cli_set_meta_all<zk_idc>,//impl
                {//options
                    { "idc-id", "", true, true, false },
                    { "data", "{}", true, true, false }
                }
            },
            {
                "get-meta-all",//action
                cli_get_meta_all<zk_idc>,//impl
                {//options
                    { "idc-id", "", true, true, false },
                    { "pretty", "", false, false, false }
                }
            },
            //semephore
            {
                "create-semaphore",//action
                cli_create_semaphore<zk_idc>,//impl
                {//options
                    { "idc-id", "", true, true, false },
                    { "key", "", true, true, false },
                    { "resource-num", "", true, true, false }
                }
            },
            {
                "destroy-semaphore",//action
                cli_destroy_semaphore<zk_idc>,//impl
                {//options
                    { "idc-id", "", true, true, false },
                    { "key", "", true, true, false }
                }
            },
            {
                "get-semaphore-resource-num",//action
                cli_get_semaphore_resource_num<zk_idc>,//impl
                {//options
                    { "idc-id", "", true, true, false },
                    { "key", "", true, true, false }
                }
            },
            {
                "set-semaphore-resource-num",//action
                cli_set_semaphore_resource_num<zk_idc>,//impl
                {//options
                    { "idc-id", "", true, true, false },
                    { "key", "", true, true, false },
                    { "resource-num", "", true, true, false }
                }
            },
            {
                "list-semaphore",//action
                cli_list_semaphore<zk_idc>,//impl
                {
                    { "idc-id", "", true, true, false },
                }//options
            },
            {
                "list-semaphore-holder",//action
                cli_list_semaphore_holder<zk_idc>,//impl
                {//options
                    { "idc-id", "", true, true, false },
                    { "key", "", true, true, false }
                }
            }
        }
    },
    {//group
        "group",//type
        {//actions
            {
                "list",//action
                cli_list<zk_group>,//impl
                {}//options
            },
            {
                "add",//action
                cli_add<zk_group>,//impl
                {//options
                    { "group-id", "", true, true, false },
                    { "data", "{}", true, true, false }
                }
            },
            {
                "del",//action
                cli_del<zk_group>,//impl
                {//options
                    { "group-id", "", true, true, false }
                }
            },
            ////////////////meta
            {
                "add-meta-attr",//action
                cli_add_meta_attr<zk_group>,//impl
                {//options
                    { "group-id", "", true, true, false },
                    { "key", "", true, true, false },
                    { "value", "", true, true, false }
                },
            },
            {
                "del-meta-attr",//action
                cli_del_meta_attr<zk_group>,//impl
                {//options
                    { "group-id", "", true, true, false },
                    { "key", "", true, true, false }
                }
            },
            {
                "set-meta-attr",//action
                cli_set_meta_attr<zk_group>,//impl
                {//options
                    { "group-id", "", true, true, false },
                    { "key", "", true, true, false },
                    { "value", "", true, true, false }
                }
            },
            {
                "get-meta-attr",//action
                cli_get_meta_attr<zk_group>,//impl
                {//options
                    { "group-id", "", true, true, false },
                    { "key", "", true, true, false }
                }
            },
            {
                "set-meta-all",//action
                cli_set_meta_all<zk_group>,//impl
                {//options
                    { "group-id", "", true, true, false },
                    { "data", "{}", true, true, false }
                }
            },
            {
                "get-meta-all",//action
                cli_get_meta_all<zk_group>,//impl
                {//options
                    { "group-id", "", true, true, false },
                    { "pretty", "", false, false, false }
                }
            },
            //semephore
            {
                "create-semaphore",//action
                cli_create_semaphore<zk_group>,//impl
                {//options
                    { "group-id", "", true, true, false },
                    { "key", "", true, true, false },
                    { "resource-num", "", true, true, false }
                }
            },
            {
                "destroy-semaphore",//action
                cli_destroy_semaphore<zk_group>,//impl
                {//options
                    { "group-id", "", true, true, false },
                    { "key", "", true, true, false }
                }
            },
            {
                "get-semaphore-resource-num",//action
                cli_get_semaphore_resource_num<zk_group>,//impl
                {//options
                    { "group-id", "", true, true, false },
                    { "key", "", true, true, false }
                }
            },
            {
                "set-semaphore-resource-num",//action
                cli_set_semaphore_resource_num<zk_group>,//impl
                {//options
                    { "group-id", "", true, true, false },
                    { "key", "", true, true, false },
                    { "resource-num", "", true, true, false }
                }
            },
            {
                "list-semaphore",//action
                cli_list_semaphore<zk_group>,//impl
                {//options
                    { "group-id", "", true, true, false },
                }
            },
            {
                "list-semaphore-holder",//action
                cli_list_semaphore_holder<zk_group>,//impl
                {//options
                    { "group-id", "", true, true, false },
                    { "key", "", true, true, false }
                }
            }
        }
    },
    {//slavenode
        "slavenode",//type
        {//actions
            {
                "list",//actino
                cli_slavenode_list,//impl
                {//options
                    { "idc-id", "", false, 1, 0 },
                    { "group-id", "", false, 1, 0 }
                }
            },
            {
                "add",//action
                cli_add<zk_slavenode>,//impl
                {//options
                    { "slavenode-id", "", true, true, false },
                    { "data", "{}", true, true, false }
                }
            },
            {
                "del",//action
                cli_del<zk_slavenode>,//impl
                {//options
                    { "slavenode-id", "", true, true, false }
                }
            },
            {
                "list-service-instance",//action
                cli_slavenode_list_service_instance,//impl
                {//options
                    { "slavenode-id", "", true, true, false }
                }
            },
            ////////////////meta
            {
                "add-meta-attr",//action
                cli_add_meta_attr<zk_slavenode>,//impl
                {//options
                    { "slavenode-id", "", true, true, false },
                    { "key", "", true, true, false },
                    { "value", "", true, true, false }
                },
            },
            {
                "del-meta-attr",//action
                cli_del_meta_attr<zk_slavenode>,//impl
                {//options
                    { "slavenode-id", "", true, true, false },
                    { "key", "", true, true, false }
                }
            },
            {
                "set-meta-attr",//action
                cli_set_meta_attr<zk_slavenode>,//impl
                {//options
                    { "slavenode-id", "", true, true, false },
                    { "key", "", true, true, false },
                    { "value", "", true, true, false }
                }
            },
            {
                "get-meta-attr",//action
                cli_get_meta_attr<zk_slavenode>,//impl
                {//options
                    { "slavenode-id", "", true, true, false },
                    { "key", "", true, true, false }
                }
            },
            {
                "set-meta-all",//action
                cli_set_meta_all<zk_slavenode>,//impl
                {//options
                    { "slavenode-id", "", true, true, false },
                    { "data", "{}", true, true, false }
                }
            },
            {
                "get-meta-all",//action
                cli_get_meta_all<zk_slavenode>,//impl
                {//options
                    { "slavenode-id", "", true, true, false },
                    { "pretty", "", false, false, false }
                }
            },
            //semephore
            {
                "create-semaphore",//action
                cli_create_semaphore<zk_slavenode>,//impl
                {//options
                    { "slavenode-id", "", true, true, false },
                    { "key", "", true, true, false },
                    { "resource-num", "", true, true, false }
                }
            },
            {
                "destroy-semaphore",//action
                cli_destroy_semaphore<zk_slavenode>,//impl
                {//options
                    { "slavenode-id", "", true, true, false },
                    { "key", "", true, true, false }
                }
            },
            {
                "get-semaphore-resource-num",//action
                cli_get_semaphore_resource_num<zk_slavenode>,//impl
                {//options
                    { "slavenode-id", "", true, true, false },
                    { "key", "", true, true, false }
                }
            },
            {
                "set-semaphore-resource-num",//action
                cli_set_semaphore_resource_num<zk_slavenode>,//impl
                {//options
                    { "slavenode-id", "", true, true, false },
                    { "key", "", true, true, false },
                    { "resource-num", "", true, true, false }
                }
            },
            {
                "list-semaphore",//action
                cli_list_semaphore<zk_slavenode>,//impl
                {//options
                    { "slavenode-id", "", true, true, false },
                }
            },
            {
                "list-semaphore-holder",//action
                cli_list_semaphore_holder<zk_slavenode>,//impl
                {//options
                    { "slavenode-id", "", true, true, false },
                    { "key", "", true, true, false }
                }
            }
        }
    },
    {//index-type
        "index-type",//type
        {//actions
            {
                "list",//actino
                cli_list<zk_index_type>,//impl
                {}//options
            },
            {
                "add",//action
                cli_add<zk_index_type>,//impl
                {//options
                    { "index-type-id", "", true, true, false },
                    { "data", "{}", true, true, false }
                }
            },
            {
                "del",//action
                cli_del<zk_index_type>,//impl
                {//options
                    { "index-type-id", "", true, true, false }
                }
            },
            {
                "list-index-partition",//action
                cli_index_type_list_partition,//impl
                {//options
                    { "index-type-id", "", true, true, false }
                }
            },
            ////////////////meta
            {
                "add-meta-attr",//action
                cli_add_meta_attr<zk_index_type>,//impl
                {//options
                    { "index-type-id", "", true, true, false },
                    { "key", "", true, true, false },
                    { "value", "", true, true, false }
                },
            },
            {
                "del-meta-attr",//action
                cli_del_meta_attr<zk_index_type>,//impl
                {//options
                    { "index-type-id", "", true, true, false },
                    { "key", "", true, true, false }
                }
            },
            {
                "set-meta-attr",//action
                cli_set_meta_attr<zk_index_type>,//impl
                {//options
                    { "index-type-id", "", true, true, false },
                    { "key", "", true, true, false },
                    { "value", "", true, true, false }
                }
            },
            {
                "get-meta-attr",//action
                cli_get_meta_attr<zk_index_type>,//impl
                {//options
                    { "index-type-id", "", true, true, false },
                    { "key", "", true, true, false }
                }
            },
            {
                "set-meta-all",//action
                cli_set_meta_all<zk_index_type>,//impl
                {//options
                    { "index-type-id", "", true, true, false },
                    { "data", "{}", true, true, false }
                }
            },
            {
                "get-meta-all",//action
                cli_get_meta_all<zk_index_type>,//impl
                {//options
                    { "index-type-id", "", true, true, false },
                    { "pretty", "", false, false, false }
                }
            }
        }
    },
    {//index-partition
        "index-partition",//type
        {//actions
            /** list is implemented via script. */
            {
                "add",//action
                cli_add<zk_index_partition>,//impl
                {//options
                    { "index-partition-id", "", true, true, false },
                    { "data", "{}", true, true, false }
                }
            },
            {
                "del",//action
                cli_del<zk_index_partition>,//impl
                {//options
                    { "index-partition-id", "", true, true, false }
                }
            },
            {
                "list-index",//action
                cli_index_partition_list_index,//impl
                {//options
                    { "index-partition-id", "", true, true, false }
                }
            },
            ////////////////meta
            {
                "add-meta-attr",//action
                cli_add_meta_attr<zk_index_partition>,//impl
                {//options
                    { "index-partition-id", "", true, true, false },
                    { "key", "", true, true, false },
                    { "value", "", true, true, false }
                },
            },
            {
                "del-meta-attr",//action
                cli_del_meta_attr<zk_index_partition>,//impl
                {//options
                    { "index-partition-id", "", true, true, false },
                    { "key", "", true, true, false }
                }
            },
            {
                "set-meta-attr",//action
                cli_set_meta_attr<zk_index_partition>,//impl
                {//options
                    { "index-partition-id", "", true, true, false },
                    { "key", "", true, true, false },
                    { "value", "", true, true, false }
                }
            },
            {
                "get-meta-attr",//action
                cli_get_meta_attr<zk_index_partition>,//impl
                {//options
                    { "index-partition-id", "", true, true, false },
                    { "key", "", true, true, false }
                }
            },
            {
                "set-meta-all",//action
                cli_set_meta_all<zk_index_partition>,//impl
                {//options
                    { "index-partition-id", "", true, true, false },
                    { "data", "{}", true, true, false }
                }
            },
            {
                "get-meta-all",//action
                cli_get_meta_all<zk_index_partition>,//impl
                {//options
                    { "index-partition-id", "", true, true, false },
                    { "pretty", "", false, false, false }
                }
            }
        }
    },
    {//index
        "index",//type
        {//actions
            /** list implemented by script */
            {
                "add",//action
                cli_add<zk_index>,//impl
                {//options
                    { "index-id", "", true, true, false },
                    { "data", "{}", true, true, false }
                }
            },
            {
                "del",//action
                cli_del<zk_index>,//impl
                {//options
                    { "index-id", "", true, true, false },
                }
            },
            ////////////////meta
            {
                "add-meta-attr",//action
                cli_add_meta_attr<zk_index>,//impl
                {//options
                    { "index-id", "", true, true, false },
                    { "key", "", true, true, false },
                    { "value", "", true, true, false }
                },
            },
            {
                "del-meta-attr",//action
                cli_del_meta_attr<zk_index>,//impl
                {//options
                    { "index-id", "", true, true, false },
                    { "key", "", true, true, false }
                }
            },
            {
                "set-meta-attr",//action
                cli_set_meta_attr<zk_index>,//impl
                {//options
                    { "index-id", "", true, true, false },
                    { "key", "", true, true, false },
                    { "value", "", true, true, false }
                }
            },
            {
                "get-meta-attr",//action
                cli_get_meta_attr<zk_index>,//impl
                {//options
                    { "index-id", "", true, true, false },
                    { "key", "", true, true, false }
                }
            },
            {
                "set-meta-all",//action
                cli_set_meta_all<zk_index>,//impl
                {//options
                    { "index-id", "", true, true, false },
                    { "data", "{}", true, true, false }
                }
            },
            {
                "get-meta-all",//action
                cli_get_meta_all<zk_index>,//impl
                {//options
                    { "index-id", "", true, true, false },
                    { "pretty", "", false, false, false }
                }
            }
        }
    },
    {//service
        "service",//type
        {//actions
            {
                "list",//actino
                cli_list<zk_service>,//impl
                {}//options
            },
            {
                "add",//action
                cli_add<zk_service>,//impl
                {//options
                    { "service-id", "", true, true, false },
                    { "data", "{}", true, true, false }
                }
            },
            {
                "del",//action
                cli_del<zk_service>,//impl
                {//options
                    { "service-id", "", true, true, false }
                }
            },
            {
                "list-service-instance",//action
                cli_service_list_service_instance,//impl
                {//options
                    { "service-id", "", true, true, false }
                }
            },
            ////////////////meta
            {
                "add-meta-attr",//action
                cli_add_meta_attr<zk_service>,//impl
                {//options
                    { "service-id", "", true, true, false },
                    { "key", "", true, true, false },
                    { "value", "", true, true, false }
                },
            },
            {
                "del-meta-attr",//action
                cli_del_meta_attr<zk_service>,//impl
                {//options
                    { "service-id", "", true, true, false },
                    { "key", "", true, true, false }
                }
            },
            {
                "set-meta-attr",//action
                cli_set_meta_attr<zk_service>,//impl
                {//options
                    { "service-id", "", true, true, false },
                    { "key", "", true, true, false },
                    { "value", "", true, true, false }
                }
            },
            {
                "get-meta-attr",//action
                cli_get_meta_attr<zk_service>,//impl
                {//options
                    { "service-id", "", true, true, false },
                    { "key", "", true, true, false }
                }
            },
            {
                "set-meta-all",//action
                cli_set_meta_all<zk_service>,//impl
                {//options
                    { "service-id", "", true, true, false },
                    { "data", "{}", true, true, false }
                }
            },
            {
                "get-meta-all",//action
                cli_get_meta_all<zk_service>,//impl
                {//options
                    { "service-id", "", true, true, false },
                    { "pretty", "", false, false, false }
                }
            },
            //semephore
            {
                "create-semaphore",//action
                cli_create_semaphore<zk_service>,//impl
                {//options
                    { "service-id", "", true, true, false },
                    { "key", "", true, true, false },
                    { "resource-num", "", true, true, false }
                }
            },
            {
                "destroy-semaphore",//action
                cli_destroy_semaphore<zk_service>,//impl
                {//options
                    { "service-id", "", true, true, false },
                    { "key", "", true, true, false }
                }
            },
            {
                "get-semaphore-resource-num",//action
                cli_get_semaphore_resource_num<zk_service>,//impl
                {//options
                    { "service-id", "", true, true, false },
                    { "key", "", true, true, false }
                }
            },
            {
                "set-semaphore-resource-num",//action
                cli_set_semaphore_resource_num<zk_service>,//impl
                {//options
                    { "service-id", "", true, true, false },
                    { "key", "", true, true, false },
                    { "resource-num", "", true, true, false }
                }
            },
            {
                "list-semaphore",//action
                cli_list_semaphore<zk_service>,//impl
                {
                    { "service-id", "", true, true, false },
                }//options
            },
            {
                "list-semaphore-holder",//action
                cli_list_semaphore_holder<zk_service>,//impl
                {//options
                    { "service-id", "", true, true, false },
                    { "key", "", true, true, false }
                }
            },
            ///////////////////planned state/////////TODO
            {
                "add-default-planned-state-attr",//action
                cli_service_add_planned_state_attr,//impl
                {//options
                    { "service-id", "", true, true, false },
                    { "key", "", true, true, false },
                    { "value", "", true, true, false },
                }
            },
            {
                "del-default-planned-state-attr",//action
                cli_service_del_planned_state_attr,//imp
                {//options
                    { "service-id", "", true, true, false },
                    { "key", "", true, true, false }
                }
            },
            {
                "set-default-planned-state-attr",//action
                cli_service_set_planned_state_attr,//impl
                {//options
                    { "service-id", "", true, true, false },
                    { "key", "", true, true, false },
                    { "value", "", true, true, false },
                }
            },
            {
                "get-default-planned-state-attr",//action
                cli_service_get_planned_state_attr,//impl
                {//options
                    { "service-id", "", true, true, false },
                    { "key", "", true, true, false }
                }
            },
            {
                "set-default-planned-state-all",//action
                cli_service_set_planned_state_all,//impl
                {//options
                    { "service-id", "", true, true, false },
                    { "data", "{}", true, true, false}
                }
            },
            {
                "get-default-planned-state-all",//action
                cli_service_get_planned_state_all,//impl
                {//options
                    { "service-id", "", true, true, false },
                    { "pretty", "", false, false, false }
                }
            }
        }
    },
    {//service-instance
        "service-instance",//type
        {//actions
            /** list is implemented via script. */
            {
                "add",//action
                cli_add_service_instance,//impl
                {//options
                    { "service-id", "", true, true, false },
                    { "slavenode-id", "", true, true, false },
                    { "data", "{}", true, true, false }
                }
            },
            {
                "del",//action
                cli_del<zk_service_instance>,//impl
                {//options
                    { "service-instance-id", "", true, true, false }
                }
            },
            {
                "list",//action
                cli_service_instance_list,//impl
                {//options
                    { "slavenode-id", "", false, true, false }
                }
            },
            {
                "dump",//action
                cli_service_instance_dump,//impl
                {//options
                    { "pretty", "", false, false, false }
                }
            },
            {
                "isvalid",//action
                cli_service_instance_is_valid,//impl
                {//options
                    { "service-instance-id", "", true, true, false }
                }
            },
            ////////////////meta
            {
                "add-meta-attr",//action
                cli_add_meta_attr<zk_service_instance>,//impl
                {//options
                    { "service-instance-id", "", true, true, false },
                    { "key", "", true, true, false },
                    { "value", "", true, true, false }
                },
            },
            {
                "del-meta-attr",//action
                cli_del_meta_attr<zk_service_instance>,//impl
                {//options
                    { "service-instance-id", "", true, true, false },
                    { "key", "", true, true, false }
                }
            },
            {
                "set-meta-attr",//action
                cli_set_meta_attr<zk_service_instance>,//impl
                {//options
                    { "service-instance-id", "", true, true, false },
                    { "key", "", true, true, false },
                    { "value", "", true, true, false }
                }
            },
            {
                "get-meta-attr",//action
                cli_get_meta_attr<zk_service_instance>,//impl
                {//options
                    { "service-instance-id", "", true, true, false },
                    { "key", "", true, true, false }
                }
            },
            {
                "set-meta-all",//action
                cli_set_meta_all<zk_service_instance>,//impl
                {//options
                    { "service-instance-id", "", true, true, false },
                    { "data", "{}", true, true, false }
                }
            },
            {
                "get-meta-all",//action
                cli_get_meta_all<zk_service_instance>,//impl
                {//options
                    { "service-instance-id", "", true, true, false },
                    { "pretty", "", false, false, false }
                }
            },
            ///////////////////planned state/////////
            {
                "add-planned-state-attr",//action
                cli_sinstance_add_planned_state_attr,//impl
                {//options
                    { "service-instance-id", "", true, true, false },
                    { "key", "", true, true, false },
                    { "value", "", true, true, false },
                }
            },
            {
                "del-planned-state-attr",//action
                cli_sinstance_del_planned_state_attr,//imp
                {//options
                    { "service-instance-id", "", true, true, false },
                    { "key", "", true, true, false }
                }
            },
            {
                "set-planned-state-attr",//action
                cli_sinstance_set_planned_state_attr,//impl
                {//options
                    { "service-instance-id", "", true, true, false },
                    { "key", "", true, true, false },
                    { "value", "", true, true, false },
                }
            },
            {
                "get-planned-state-attr",//action
                cli_sinstance_get_planned_state_attr,//impl
                {//options
                    { "service-instance-id", "", true, true, false },
                    { "key", "", true, true, false }
                }
            },
            {
                "set-planned-state-all",//action
                cli_sinstance_set_planned_state_all,//impl
                {//options
                    { "service-instance-id", "", true, true, false },
                    { "data", "{}", true, true, false },
                }
            },
            {
                "get-planned-state-all",//action
                cli_sinstance_get_planned_state_all,//impl
                {//options
                    { "service-instance-id", "", true, true, false },
                    { "pretty", "", false, false, false }
                }
            },
            ///////////////////current state/////////
            {
                "add-current-state-attr",//action
                cli_sinstance_add_current_state_attr,//impl
                {//options
                    { "service-instance-id", "", true, true, false },
                    { "key", "", true, true, false },
                    { "value", "", true, true, false },
                }
            },
            {
                "del-current-state-attr",//action
                cli_sinstance_del_current_state_attr,//imp
                {//options
                    { "service-instance-id", "", true, true, false },
                    { "key", "", true, true, false }
                }
            },
            {
                "set-current-state-attr",//action
                cli_sinstance_set_current_state_attr,//impl
                {//options
                    { "service-instance-id", "", true, true, false },
                    { "key", "", true, true, false },
                    { "value", "", true, true, false },
                }
            },
            {
                "get-current-state-attr",//action
                cli_sinstance_get_current_state_attr,//impl
                {//options
                    { "service-instance-id", "", true, true, false },
                    { "key", "", true, true, false }
                }
            },
            {
                "set-current-state-all",//action
                cli_sinstance_set_current_state_all,//impl
                {//options
                    { "service-instance-id", "", true, true, false },
                    { "data", "{}", true, true, false }
                }
            },
            {
                "get-current-state-all",//action
                cli_sinstance_get_current_state_all,//impl
                {//options
                    { "service-instance-id", "", true, true, false },
                    { "pretty", "", false, false, false }
                }
            }
        }
    },
    {//command
        "command",//type
        {//actions
            {
                "generateid",//action
                command_generateid,//impl
                {}
            },
            {
                "add",//action
                command_add,//impl
                {//option
                    { "service-instance-id", "", true, true, false },
                    { "command-id", "", true, true, false },
                    { "data", "{}", true, true, false }
                }
            },
            {
                "del",//action
                command_del,//impl
                {//options
                    { "service-instance-id", "", true, true, false },
                    { "command-id", "", true, true, false },
                }
            },
            {
                "list",//action
                command_list,//impl
                {//options
                    { "service-instance-id", "", true, true, false }
                }
            },
            {
                "add-meta-attr",//action
                command_add_meta_attr,//impl
                {//options
                    { "service-instance-id", "", true, true, false },
                    { "command-id", "", true, true, false },
                    { "key", "", true, true, false },
                    { "value", "", true, true, false }
                }
            },
            {
                "del-meta-attr",//action
                command_del_meta_attr,//impl
                {//options
                    { "service-instance-id", "", true, true, false },
                    { "command-id", "", true, true, false },
                    { "key", "", true, true, false }
                }
            },
            {
                "set-meta-attr",//action
                command_set_meta_attr,//impl
                {//options
                    { "service-instance-id", "", true, true, false },
                    { "command-id", "", true, true, false },
                    { "key", "", true, true, false },
                    { "value", "", true, true, false }
                }
            },
            {
                "get-meta-attr",//action
                command_get_meta_attr,//impl
                {//options
                    { "service-instance-id", "", true, true, false },
                    { "command-id", "", true, true, false },
                    { "key", "", true, true, false }
                }
            },
            {
                "set-meta-all",//action
                command_set_meta_all,//impl
                {//options
                    { "service-instance-id", "", true, true, false },
                    { "command-id", "", true, true, false },
                    { "data", "{}", true, true, false }
                }
            },
            {
                "get-meta-all",//action
                command_get_meta_all,//impl
                {//options
                    { "service-instance-id", "", true, true, false },
                    { "command-id", "", true, true, false },
                    { "pretty", "", false, false, false }
                }
            }
        }
    },
    {//job
        "job",//type
        {//actions
            {
                "list",//action
                cli_list<zk_job>,//impl
                {}//options
            },
            {
                "add",//action
                cli_add<zk_job>,//impl
                {//options
                    { "job-id", "", true, true, false },
                    { "data", "{}", true, true, false }
                }
            },
            {
                "del",//action
                cli_del<zk_job>,//impl
                {//options
                    { "job-id", "", true, true, false }
                }
            },
            ////////////////meta
            {
                "add-meta-attr",//action
                cli_add_meta_attr<zk_job>,//impl
                {//options
                    { "job-id", "", true, true, false },
                    { "key", "", true, true, false },
                    { "value", "", true, true, false }
                },
            },
            {
                "del-meta-attr",//action
                cli_del_meta_attr<zk_job>,//impl
                {//options
                    { "job-id", "", true, true, false },
                    { "key", "", true, true, false }
                }
            },
            {
                "set-meta-attr",//action
                cli_set_meta_attr<zk_job>,//impl
                {//options
                    { "job-id", "", true, true, false },
                    { "key", "", true, true, false },
                    { "value", "", true, true, false }
                }
            },
            {
                "get-meta-attr",//action
                cli_get_meta_attr<zk_job>,//impl
                {//options
                    { "job-id", "", true, true, false },
                    { "key", "", true, true, false }
                }
            },
            {
                "set-meta-all",//action
                cli_set_meta_all<zk_job>,//impl
                {//options
                    { "job-id", "", true, true, false },
                    { "data", "{}", true, true, false }
                }
            },
            {
                "get-meta-all",//action
                cli_get_meta_all<zk_job>,//impl
                {//options
                    { "job-id", "", true, true, false },
                    { "pretty", "", false, false, false }
                }
            },
            {
                "get-meta-keys",//action
                cli_get_meta_keys<zk_job>,//impl
                {//options
                    { "job-id", "", true, true, false },
                }
            }
        }
    },
    {//dyndata-meta
        "dyndata-meta",//type
        {//actions
            {
                "list",//action
                cli_list<zk_dyndata_meta>,//impl
                {}
            },
            {
                "add",//action
                cli_add<zk_dyndata_meta>,//impl
                {
                    { "dyndata-meta-id", "", true, true, false },
                    { "data", "{}", true, true, false }
                }
            },
            {
                "del",//action
                cli_del<zk_dyndata_meta>,//impl
                {
                    { "dyndata-meta-id", "", true, true, false }
                }
            },
            {
                "list-dyndata",//action
                cli_list_dyndata,//impl
                {
                    { "dyndata-meta-id", "", true, true, false }
                }
            },
            ////////////////meta
            {
                "add-meta-attr",//action
                cli_add_meta_attr<zk_dyndata_meta>,//impl
                {//options
                    { "dyndata-meta-id", "", true, true, false },
                    { "key", "", true, true, false },
                    { "value", "", true, true, false }
                },
            },
            {
                "del-meta-attr",//action
                cli_del_meta_attr<zk_dyndata_meta>,//impl
                {//options
                    { "dyndata-meta-id", "", true, true, false },
                    { "key", "", true, true, false }
                }
            },
            {
                "set-meta-attr",//action
                cli_set_meta_attr<zk_dyndata_meta>,//impl
                {//options
                    { "dyndata-meta-id", "", true, true, false },
                    { "key", "", true, true, false },
                    { "value", "", true, true, false }
                }
            },
            {
                "get-meta-attr",//action
                cli_get_meta_attr<zk_dyndata_meta>,//impl
                {//options
                    { "dyndata-meta-id", "", true, true, false },
                    { "key", "", true, true, false }
                }
            },
            {
                "set-meta-all",//action
                cli_set_meta_all<zk_dyndata_meta>,//impl
                {//options
                    { "dyndata-meta-id", "", true, true, false },
                    { "data", "{}", true, true, false }
                }
            },
            {
                "get-meta-all",//action
                cli_get_meta_all<zk_dyndata_meta>,//impl
                {//options
                    { "dyndata-meta-id", "", true, true, false },
                    { "pretty", "", false, false, false }
                }
            },
            {
                "get-meta-keys",//action
                cli_get_meta_keys<zk_dyndata_meta>,//impl
                {//options
                    { "dyndata-meta-id", "", true, true, false },
                }
            }
        }
    },
    {//dyndata
        "dyndata",//type
        {//actions
            {
                "add",//action
                cli_add_dyndata,//impl
                {//options
                    { "dyndata-id", "", true, true, false },
                    { "data", "{}", false, true, false },
                    { "key", "", false, true, false },
                    { "value", "", false, true, false },
                }
            },
            {
                "del",//action
                cli_del<zk_dyndata>,//impl
                {//options
                    { "dyndata-id", "", true, true, false },
                }
            },
            ////////////////meta
            {
                "add-meta-attr",//action
                cli_add_meta_attr<zk_dyndata>,//impl
                {//options
                    { "dyndata-id", "", true, true, false },
                    { "key", "", true, true, false },
                    { "value", "", true, true, false }
                },
            },
            {
                "del-meta-attr",//action
                cli_del_meta_attr<zk_dyndata>,//impl
                {//options
                    { "dyndata-id", "", true, true, false },
                    { "key", "", true, true, false }
                }
            },
            {
                "set-meta-attr",//action
                cli_set_meta_attr<zk_dyndata>,//impl
                {//options
                    { "dyndata-id", "", true, true, false },
                    { "key", "", true, true, false },
                    { "value", "", true, true, false }
                }
            },
            {
                "get-meta-attr",//action
                cli_get_meta_attr<zk_dyndata>,//impl
                {//options
                    { "dyndata-id", "", true, true, false },
                    { "key", "", true, true, false }
                }
            },
            {
                "set-meta-all",//action
                cli_set_meta_all<zk_dyndata>,//impl
                {//options
                    { "dyndata-id", "", true, true, false },
                    { "data", "{}", true, true, false }
                }
            },
            {
                "get-meta-all",//action
                cli_get_meta_all<zk_dyndata>,//impl
                {//options
                    { "dyndata-id", "", true, true, false },
                    { "pretty", "", false, false, false }
                }
            },
            {
                "get-meta-keys",//action
                cli_get_meta_keys<zk_dyndata>,//impl
                {//options
                    { "dyndata-id", "", true, true, false },
                }
            }

        }
    },
    {//placement-plan
        "placement-plan",//type
        {//actions
            {
                "dump",//action
                cli_placement_plan_dump,//impl
                {//options
                    { "group-id", "", false, true, false },
                    { "idc-id", "", false, true, false },
                }
            }
        }
    }
};
/**
 * @brief 根据type和action构造帮助信息
 *
 * @param [in/out] request   : cli_request&
 * @author huangjindong
 * @date 2012/02/27 19:01:59
**/
static void print_usage(cli_request &request)
{
    char buffer[1024000];//1M
    int offset=0;
    cli_zk_option *options = g_meta[request.type_index].actions[request.action_index].options;

    offset += snprintf(buffer+offset, sizeof(buffer)-offset, "Usage: zkctl %s %s", 
            g_meta[request.type_index].type.c_str(),
            g_meta[request.type_index].actions[request.action_index].action.c_str());
    for(int i=0; !options[i].name.empty(); i++){
        const char *fmt;
        if(options[i].has_arg){
            if(options[i].required){
                fmt = " --%s='%s'";
            }
            else{
                fmt = " [--%s='%s']";
            }
            offset += snprintf(buffer+offset, sizeof(buffer)-offset, fmt,
                    options[i].name.c_str(), options[i].description.c_str());
        }
        else{
            if(options[i].required){
                fmt = " --%s";
            }
            else{
                fmt = " [--%s]";
            }
            offset += snprintf(buffer+offset, sizeof(buffer)-offset, fmt,
                options[i].name.c_str());
        }
    }
    fprintf(stderr, "%s\n", buffer);
}
/**
 * @brief 将命令行参数的值放入cli_request中 
 *
 * @param [in/out] request   : cli_request&
 * @param [in/out] name   : const char*
 * @param [in/out] value   : const char*
 * @author huangjindong
 * @date 2012/02/27 19:02:37
**/
int cli_process_option(cli_request &request, const char *name, const char *value){
    //TODO: check if key value match 
    string *var = request.option_to_var(name);
    if(NULL != var){
        if(NULL == value){//option that has no arg.
            *var = "OK";
        }
        else{
            *var = value;
        }
    }
    else if(0 == strcmp("key", name)){
        request.key = value;
        request.keys.push_back(value);
    }
    else if(0 == strcmp("value", name)){
        request.key_values[request.key] = value;
    }
    else{
        return -1;
    }
    //object-id
    if(0 == strncmp(request.type.c_str(), name, strlen(request.type.c_str()))){
        request.object_id = value;
    }
    return 0;
}
/**
 * @brief 解析命令行参数
 *
 * @param [in/out] argc   : int
 * @param [in/out] argv   : char*[]
 * @param [in/out] request   : cli_request&
 * @return  int 
 * @retval   
 * @author huangjindong
 * @date 2012/02/27 16:14:41
**/
int cli_parse_options(int argc, char *argv[], cli_request &request)
{
    int OPTION_OFFSET = 3;
    if(argc < OPTION_OFFSET){
        print_help();
        return EXIT_FAILURE;
    }
    request.type = argv[1];
    request.action = argv[2];
    request.type_index = -1;
    request.action_index = -1;
    //1.find type
    for(uint32_t i=0; i<sizeof(g_meta)/sizeof(cli_zk_type); i++){
        if(g_meta[i].type == request.type){
            request.type_index = i;
            break;
        }
    }
    if(request.type_index < 0){
        fprintf(stderr, "\"%s\" is not a valid zk object\n", request.type.c_str());
        print_help();
        return EXIT_FAILURE;
    }
    //2.find action
    for(int i=0; !g_meta[request.type_index].actions[i].action.empty(); i++){
        if(g_meta[request.type_index].actions[i].action == request.action){
            request.action_index = i;
            break;
        }
    }
    if(request.action_index < 0){
        fprintf(stderr,"\"%s\" doesn't support \"%s\" operation\n", 
                request.type.c_str(), request.action.c_str());
        return EXIT_FAILURE;
    }
    //3.parse options
    //3.1 build long options
    option long_options[ZKSH_CMD_MAX_NUM];
    cli_zk_option *options = g_meta[request.type_index].actions[request.action_index].options;
    int option_index = 0;
    for(; !options[option_index].name.empty(); option_index++){
        long_options[option_index].name = options[option_index].name.c_str();
        long_options[option_index].has_arg = options[option_index].has_arg;
        long_options[option_index].flag = 0;
        long_options[option_index].val = option_index;
        options[option_index].specified = 0;
    }
    long_options[option_index].name = NULL;
    //3.2 parse long options
    opterr=0;
    while(1){
        int c = getopt_long(argc, argv, "", long_options, NULL);
        if(-1 == c){
            break;
        }
        if(c == '?'){
            fprintf(stderr, "unkonw option \"--%s\"\n", argv[optind-1]);
            print_usage(request);
            return EXIT_FAILURE;
        }
        options[c].specified = 1;
        int ret = cli_process_option(request, long_options[c].name, optarg);
        if(ret < 0){
            fprintf(stderr, "\"%s\" error in process_option(), pls check code\n", 
                    long_options[c].name);
            return EXIT_FAILURE;
        }
    }
    //3.3 validate
    option_index = 0;
    for(; !options[option_index].name.empty(); option_index++){
        if(options[option_index].required
                &&!options[option_index].specified){
            fprintf(stderr, "missing option \"--%s\"\n", options[option_index].name.c_str());
            print_usage(request);
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

/**
 * @brief 运行命令 
 *
 * @param [in/out] request   : cli_request&
 * @return  int 
 * @retval   
 * @see 
 * @note 
 * @author huangjindong
 * @date 2012/02/27 19:03:48
**/
int cli_invoke(cli_request &request)
{
    zk_search_cluster_manager manager("test_cluster_manager");
    int ret = manager.connect(g_conf.zk_cluster, g_conf.zk_root, 
            g_conf.zk_username, g_conf.zk_password);
    if(ZK_OK != ret){
        zk_log(UL_LOG_FATAL, "connect to zookeeper %s@%s failed error=%s", 
                g_conf.zk_cluster.c_str(), g_conf.zk_root.c_str(), strerror(errno));
        return ZK_ERROR;
    }
    cli_action action = g_meta[request.type_index].actions[request.action_index].action_impl;
    ret = action(request, manager);
    manager.disconnect();
    return ret;
}

/**
 * @brief 分行打印列表 
 *
 * @param [in/out] values   : vector<string>&
 * @return  void 
 * @author huangjindong
 * @date 2012/02/28 17:55:04
**/
void cli_print_list(vector<string> &values)
{
    for(uint32_t i=0;i<values.size();i++){
        fprintf(stdout, "%s\n", values[i].c_str());
    }
}

/**
 * @brief 分段打印列表 
 *
 * @param [in/out] values   : vector<string>&
 * @return  void 
 * @author huangjindong
 * @date 2012/02/28 19:02:14
**/
void cli_print_token(vector<string> &values)
{
    for(uint32_t i=0;i<values.size();i++){
        fprintf(stdout, "%s", values[i].c_str());
        if(i+1 != values.size()){
            fprintf(stdout, " ");
        }
        else{
            fprintf(stdout, "\n");
        }
    }
}

/**
 * @brief CLUSTER only operation. 
 *
 * @param [in/out] request   : cli_request&
 * @param [in/out] manager   : zk_search_cluster_manager&
 * @return  int 
 * @author huangjindong
 * @date 2012/02/28 17:55:13
**/
int cli_cluster_repair(cli_request &/*request*/, zk_search_cluster_manager &manager)
{
    static int need_repair = 1;
    int ret = manager.check_nodes(need_repair);
    if(ret != ZK_OK)
    {
        fprintf(stderr, "cluster repair failed, refer log for detail\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

/**
 * @brief CLUSTER only operation, NOT implemented
 *
 * @param [in/out] request   : cli_request&
 * @param [in/out] manager   : zk_search_cluster_manager&
 * @return  int 
 * @author huangjindong
 * @date 2012/02/28 17:55:26
**/
int cli_cluster_destroy(cli_request &/*request*/, zk_search_cluster_manager &manager)
{
    int ret = manager.clear();
    if(ret != ZK_OK)
    {
        fprintf(stderr, "cluster destroy failed, refer log for detail\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
/**
 * @brief SLAVENODE only operation. filter enabled list.
 *
 * @param [in/out] request   : cli_request&
 * @param [in/out] manager   : zk_search_cluster_manager&
 * @return  int 
 * @retval   
 * @author huangjindong
 * @date 2012/02/28 17:55:43
**/
int cli_slavenode_list(cli_request &request,zk_search_cluster_manager &manager)
{
    int ret;
    vector<zk_slavenode*> all_nodes;
    bsl::ResourcePool rp;
    ret = manager.get_slavenodes_by_idc_group(&all_nodes, rp, request.idc_id, request.group_id);
    if(ret != ZK_OK)
    {
        fprintf(stderr, "list %s failed, check log for detail\n", request.type.c_str());
        return EXIT_FAILURE;
    }
    cli_print_list< zk_slavenode >(all_nodes);
    return EXIT_SUCCESS;
}
/**
 * @brief  SLAVENODE only operation, NOT implemented
 *
 * @param [in/out] request   : cli_request&
 * @param [in/out] manager   : zk_search_cluster_manager&
 * @return  int 
 * @author huangjindong
 * @date 2012/02/28 17:56:08
**/
int cli_slavenode_list_service_instance(cli_request &request, zk_search_cluster_manager &manager)
{
    int ret;
    vector<zk_service_instance*> instance_vector;
    zk_slavenode *slavenode;
    bsl::ResourcePool local_rp;
    ret = manager.get_object_by_id<zk_slavenode>(request.slavenode_id, &slavenode, local_rp);
    if(ret != ZK_OK){
        fprintf(stderr, "slavenode \"%s\" not found\n", request.slavenode_id.c_str());
        return EXIT_FAILURE;
    }
    ret = slavenode->get_all_service_instances(&instance_vector, local_rp);
    if(ret != ZK_OK){
        fprintf(stderr, "slavenode \"%s\" find instance failed, refer log for detail\n", 
                request.slavenode_id.c_str());
        return EXIT_FAILURE;
    }
    cli_print_list<zk_service_instance>(instance_vector);
    return EXIT_SUCCESS;
}
/**
 * @brief INDEX-TYPE only operation
 *
 * @param [in/out] request   : cli_request&
 * @param [in/out] manager   : zk_search_cluster_manager&
 * @return  int 
 * @author huangjindong
 * @date 2012/02/28 19:54:46
**/
int cli_index_type_list_partition(cli_request &request, zk_search_cluster_manager &manager)
{
    int ret;
    vector<zk_index_partition*> partition_vector;
    bsl::ResourcePool local_rp;

    ret = manager.get_index_partitions_by_index_type_id(request.index_type_id, 
            &partition_vector, local_rp);
    if (ret == ZK_NODE_NOT_EXISTS)
    {
        fprintf(stderr, "index_type list index_partition failed, index_type \"%s\" not exists\n", 
                request.index_id.c_str());
        return EXIT_FAILURE;
    }
    else if (ret != ZK_OK)
    {
        fprintf(stderr, "index_type \"%s\" list index_partition failed, refer log for detail\n",
                request.index_id.c_str());
        return EXIT_FAILURE;
    }
    cli_print_list<zk_index_partition>(partition_vector);
    return EXIT_SUCCESS;
}
/**
 * @brief INDEX-PARTITION only operation
 *
 * @param [in/out] request   : cli_request&
 * @param [in/out] manager   : zk_search_cluster_manager&
 * @return  int 
 * @author huangjindong
 * @date 2012/02/29 19:45:48
**/
int cli_index_partition_list_index(cli_request &request, zk_search_cluster_manager &manager)
{
    int ret;
    vector<zk_index*> index_vector;
    bsl::ResourcePool local_rp;
    ret = manager.get_indices_by_index_partition_id(request.index_partition_id, 
            &index_vector, local_rp);
    if(ret != ZK_OK){
        fprintf(stderr, "index partition \"%s\" list index failed, refer log for detail\n", 
                request.index_partition_id.c_str());
        return EXIT_FAILURE;
    }
    cli_print_list<zk_index>(index_vector);
    return EXIT_SUCCESS;
}
/**
 * @brief SERVICE only opeation
 *
 * @param [in/out] request   : cli_request&
 * @param [in/out] manager   : zk_search_cluster_manager&
 * @return  int 
 * @author huangjindong
 * @date 2012/02/29 19:46:20
**/
int cli_service_list_service_instance(cli_request &request, zk_search_cluster_manager &manager)
{
    int ret;
    zk_service *service;
    bsl::ResourcePool local_rp;
    ret = manager.get_object_by_id<zk_service>(request.service_id, &service, local_rp);
    if(ret != ZK_OK){
        fprintf(stderr, "service list service-instance failed, service \"%s\" not found\n", 
                request.service_id.c_str());
        return EXIT_FAILURE;
    }
    vector<zk_service_instance*> instance_vector;
    ret = service->get_all_service_instances(&instance_vector, local_rp);
    if(ret != ZK_OK){
        fprintf(stderr, "service \"%s\" list instance error, refer log for detail\n", 
                request.service_id.c_str());
        return EXIT_FAILURE;
    }
    cli_print_list<zk_service_instance>(instance_vector);
    return EXIT_SUCCESS;
}
/**
 * @brief dyndata add
 */
int cli_add_dyndata(cli_request &request, zk_search_cluster_manager &manager)
{
    int ret;
    zk_dyndata_meta *dyndata_meta;
    zk_dyndata *dyndata;
    bsl::ResourcePool local_rp;
    bsl::var::Dict *result_dict;
    ret = manager.new_object<zk_dyndata>(request.dyndata_id, &dyndata, local_rp);
    if(ret != ZK_OK){
        RETURN_ERROR("%s \"%s\" %s failed, no valid dyndata-meta found.\n");
    }
    if(!request.data.empty()){
        bsl::var::JsonDeserializer jd(local_rp);
        try{
            bsl::var::IVar &tmp_meta = jd.deserialize(request.data.c_str());
            result_dict = &((bsl::var::Dict&)tmp_meta);
        } catch (bsl::Exception &e) {
            RETURN_ERROR("%s \"%s\" %s failed, invalid json\n");
        }
    }
    else{
        string meta_id = dyndata->get_parent_id();
        ret = manager.get_object_by_id<zk_dyndata_meta>(meta_id, &dyndata_meta, local_rp);
        if(ret != ZK_OK){
            RETURN_ERROR("%s \"%s\" %s failed, can't find meta\n");
        }
        ret = dyndata_meta->get_meta_all(&result_dict, local_rp);
        if(ret != ZK_OK){
            RETURN_ERROR("%s \%s\" %s failed, get meta of meta failed\n");
        }
    }
    ret = manager.add_object(dyndata);
    if(ret != ZK_OK){
        RETURN_ERROR("%s \"%s\" %s failed, refer log for detail\n");
    }
    ret = dyndata->set_meta_all(*result_dict, &request.key_values);
    if(ret != ZK_OK){
        RETURN_ERROR("%s \"%s\" %s set meta failed, refer log for detail\n");
    }
    return EXIT_SUCCESS;
}

/**
 * @brief DYNDATA only operation
 */
int cli_list_dyndata(cli_request &request, zk_search_cluster_manager &manager)
{
    int ret;
    zk_dyndata_meta *dyndata_meta;
    bsl::ResourcePool local_rp;
    ret = manager.get_object_by_id<zk_dyndata_meta>(request.dyndata_meta_id, 
            &dyndata_meta, local_rp);
    if(ret != ZK_OK){
        RETURN_ERROR("%s \"%s\" %s failed, can't find this\n");
    }
    vector<zk_dyndata*> dyndata_vector;
    ret = dyndata_meta->get_dyndata(&dyndata_vector, local_rp);
    if(ret != ZK_OK){
        RETURN_ERROR("%s \"%s\" %s failed, refer log for detail\n");
    }
    cli_print_list<zk_dyndata>(dyndata_vector);
    return EXIT_SUCCESS;
}
int cli_service_instance_is_valid(cli_request &request, zk_search_cluster_manager &manager)
{
    int ret;
    bsl::ResourcePool local_rp;
    zk_service_instance *tmp_instance;
    //check if service valid
    ret = manager.get_object_by_id<zk_service_instance>(request.service_instance_id, &tmp_instance, local_rp);
    if(ret != ZK_OK){
        fprintf(stdout,"invalid\n");
    }
    else{
        fprintf(stdout, "valid\n");
    }
    return EXIT_SUCCESS;
}
/**
 * @brief service-instance add special
 *
 * @param [in/out] request   : cli_request&
 * @param [in/out] manager   : zk_search_cluster_manager&
 * @return  int 
 * @author huangjindong
 * @date 2012/02/29 19:47:58
**/
int cli_add_service_instance(cli_request &request, zk_search_cluster_manager &manager)
{
    int ret;
    zk_service_instance *new_object;
    bsl::ResourcePool local_rp;
    bsl::var::JsonDeserializer jd(local_rp);
    bsl::var::Dict *result_dict;
    zk_service *tmp_service;
    zk_slavenode *tmp_slavenode;
    //check if service valid
    ret = manager.get_object_by_id<zk_service>(request.service_id, &tmp_service, local_rp);
    if(ret != ZK_OK){
        fprintf(stderr, "service-instance add failed, can't find service \"%s\"\n",
                request.service_id.c_str());
        return EXIT_FAILURE;
    }
    //check if slavenode valid
    ret = manager.get_object_by_id<zk_slavenode>(request.slavenode_id, &tmp_slavenode, local_rp);
    if(ret != ZK_OK){
        fprintf(stderr, "service-instance add failed, can't find slavenode \"%s\"\n", 
                request.slavenode_id.c_str());
        return EXIT_FAILURE;
    }
    //parse json data
    try{
        bsl::var::IVar &tmp_meta = jd.deserialize(request.data.c_str());
        result_dict = &((bsl::var::Dict&)tmp_meta);
    } catch (bsl::Exception &e) {
        RETURN_ERROR("%s \"%s\" %s failed, invalid json\n");
    }
    //new object
    ret = tmp_service->new_service_instance(&new_object, request.slavenode_id, local_rp);
    if(ret != ZK_OK) {
        RETURN_ERROR("%s \"%s\" %s failed, refer log for detail\n");
    }

    //add to cluster
    ret = manager.add_object(new_object);
    if (ret != ZK_OK) {
        RETURN_ERROR("%s \"%s\" %s failed, it may be exist already\n");
    }
    //TODO: move to add.
    ret = new_object->set_meta_all(*result_dict);
    if(ret != ZK_OK) {
        fprintf(stderr,"%s set meta attr.\n", request.type.c_str());
        return EXIT_FAILURE;
    }
    //clone planned-state from service's default-planned-state
    ret = tmp_service->get_default_planned_state_all(&result_dict, local_rp);
    if(ret != ZK_OK){
        fprintf(stderr,"%s set planned-state failed, For %s get default planned state fail\n", 
                new_object->get_id().c_str(), tmp_service->get_id().c_str());
    }
    else{
        ret = new_object->set_planned_state_all(*result_dict);
        if(ret != ZK_OK){
            RETURN_ERROR("%s \"%s\" %s failed while set planned-state, refer log for detail\n");
        }
    }
    fprintf(stdout, "%s\n", new_object->get_id().c_str());
    return EXIT_SUCCESS;
}
static int get_instance_attrs(zk_service_instance* instance, string &partition,
        bsl::var::Dict **pdict, bsl::ResourcePool &rp){
    int ret;
    bsl::var::Dict &dict = rp.create<bsl::var::Dict>();
    vector<string> meta_keys;
    meta_keys.push_back("state");//metas
    vector<string> meta_values;
    string bs_id = instance->get_service_id();
    if(bs_id.substr(0,3) != "bs_"){
        return ZK_ERROR;
    }
    else{
        partition = bs_id.substr(3);
    }
    ret = instance->get_meta_attr(meta_keys, meta_values);
    if(ret != ZK_OK){
        fprintf(stderr, "%s get meta attr \"state\" failed\n", instance->get_id().c_str());
        return ret;
    }
    ///meta-attr
    dict["partition"] = rp.create<bsl::var::String>(partition.c_str());
    for(uint32_t i=0; i<meta_keys.size(); i++){
        dict[meta_keys[i].c_str()] = rp.create<bsl::var::String>(meta_values[i].c_str());
    }

    ///current-state-attr
    string state_value;
    //service_port
    string state_key="service_port";
    ret = instance->get_current_state_attr(state_key, &state_value);
    if(ret != ZK_OK){
        fprintf(stderr, "%s get current_state \"%s\" failed\n", 
                instance->get_id().c_str(), state_key.c_str());
        state_value = "-1";//default -1
    }
    dict[state_key.c_str()] = rp.create<bsl::var::String>(state_value.c_str());

    *pdict = &dict;
    return ZK_OK;
}
static int get_index_partitions(string group_id, zk_cluster_manager &manager,
        bsl::var::Dict &dict, bsl::ResourcePool &rp){
    int ret;
    bsl::var::Dict *dict_meta;
    vector<zk_index_type*> all_type;
    vector<zk_index_partition*> all_partition;
    ret = manager.get_all_objects<zk_index_type>(&all_type, rp);
    if(ret != ZK_OK){
        fprintf(stderr, "get index types failed\n");
        return ZK_ERROR;
    }
    for(vector<zk_index_type*>::iterator it=all_type.begin(); it!=all_type.end(); it++){
        all_partition.clear();
        ret = (*it)->get_all_index_partitions(&all_partition, rp);
        if(ret != ZK_OK){
            fprintf(stderr, "get index partitions of %s failed\n", (*it)->get_id().c_str());
            return ZK_ERROR;
        }
        for(vector<zk_index_partition*>::iterator part_it=all_partition.begin(); 
                part_it!=all_partition.end(); part_it++){
            ret = (*part_it)->get_meta_all(&dict_meta, rp);
            if(ret != ZK_OK){
                fprintf(stderr, "get meta of %s failed\n", (*part_it)->get_id().c_str());
                return ZK_ERROR;
            }
            string cur_group_id = (*dict_meta)["group"].c_str();
            if(!group_id.empty() && cur_group_id != group_id){
                continue;
            }
            bsl::string key = rp.create<bsl::string>( (*part_it)->get_id().c_str() );
            dict[key] = *dict_meta;
        }
    }
    return ZK_OK;
}
static int get_all_service_instance(cli_request &request, zk_search_cluster_manager &manager,
        vector<zk_service_instance*> &all_instance, bsl::ResourcePool &rp)
{
    vector<zk_service*> all_service;
    int ret;
    ret = manager.get_all_objects<zk_service>(&all_service, rp);
    if(ret != ZK_OK){
        fprintf(stderr, "%s %s failed, list service failed, refer log for detail\n",
                request.type.c_str(), request.action.c_str());
        return ZK_ERROR;
    }
    for(vector<zk_service*>::iterator it=all_service.begin(); it!=all_service.end(); it++){
        ret = (*it)->get_all_service_instances(&all_instance, rp);
        if(ret != ZK_OK){
            fprintf(stderr, "%s %s failed, get %s's instance failed\n",
                    request.type.c_str(), request.action.c_str(), (*it)->get_id().c_str());
            return ZK_ERROR;
        }
    }
    return ZK_OK;
}
int cli_service_instance_list(cli_request &request, zk_search_cluster_manager &manager)
{
    int ret;
    bsl::ResourcePool rp;
    zk_slavenode *slavenode;
    vector<zk_service_instance*> all_instance;
    if(request.slavenode_id.empty()){
        ret = get_all_service_instance(request, manager, all_instance, rp);
    }
    else{
        ret = manager.get_object_by_id<zk_slavenode>(request.slavenode_id, &slavenode, rp); 
        if(ret != ZK_OK){
            fprintf(stderr, "service-instance list failed, can't get slavenode %s\n",
                    request.slavenode_id.c_str());
            return EXIT_FAILURE;
        }
        ret = slavenode->get_all_service_instances(&all_instance, rp);
        if(ret != ZK_OK){
            fprintf(stderr, "service-instance list of %s failed\n", request.slavenode_id.c_str());
            return EXIT_FAILURE;
        }
    }

    if(ret != ZK_OK){
        return EXIT_FAILURE;
    }
    cli_print_list(all_instance);
    return EXIT_SUCCESS;

}
int cli_service_instance_dump(cli_request &request, zk_search_cluster_manager &manager)
{
    int ret;
    bsl::ResourcePool rp;
    vector<zk_service_instance*> all_instance;
    bsl::var::Dict &dict_result = rp.create<bsl::var::Dict>();
    bsl::var::Dict *meta_all;
    bsl::var::Dict *planned_state;
    bsl::var::Dict *current_state;
    ret = get_all_service_instance(request, manager, all_instance, rp);
    if(ret != ZK_OK){
        return EXIT_FAILURE;
    }
    for(vector<zk_service_instance*>::iterator inst=all_instance.begin();
            inst!=all_instance.end(); inst++){
        ret = (*inst)->get_meta_all(&meta_all, rp);
        if(ret != ZK_OK){
            fprintf(stderr, "service-instance dump failed, instance %s get-meta-all failed\n",
                    (*inst)->get_id().c_str());
            return EXIT_FAILURE;
        }
        ret = (*inst)->get_planned_state_all(&planned_state, rp);
        if(ret != ZK_OK){
            fprintf(stderr, "service-instance dump failed, instance %s get-plan-state faild\n",
                    (*inst)->get_id().c_str());
            return EXIT_FAILURE;
        }
        ret = (*inst)->get_current_state_all(&current_state, rp);
        if(ret != ZK_OK){
            fprintf(stderr, "service-instance dump failed, instance %s get-current failed\n",
                    (*inst)->get_id().c_str());
            return EXIT_FAILURE;
        }
        bsl::var::Dict &dict_inst = rp.create<bsl::var::Dict>();
        dict_inst["meta-all"] = *meta_all;
        dict_inst["planned-state-all"] = *planned_state;
        dict_inst["current-state-all"] = *current_state;
        dict_result[(*inst)->get_id().c_str()] = dict_inst;
    }
    print_json(dict_result);
    return EXIT_SUCCESS;
}
//
int cli_placement_plan_dump(cli_request &request, zk_search_cluster_manager &manager)
{
    int ret;
    bsl::ResourcePool rp;
    vector<zk_slavenode*> all_nodes;
    vector<zk_index_type*> all_indextype;
    vector<zk_index_partition*> index_partition;
    vector<zk_service_instance*> all_instance;
    bsl::var::Dict &dict_slavenode = rp.create<bsl::var::Dict>();
    bsl::var::Dict &dict_placement = rp.create<bsl::var::Dict>();
    bsl::var::Dict &dict_partition = rp.create<bsl::var::Dict>();
    bsl::var::Dict &result = rp.create<bsl::var::Dict>();
    bsl::var::Array *dict_instance;
    bsl::var::Dict *meta;
    bsl::string key;
    string partition;
    ret = get_index_partitions(request.group_id, manager, dict_partition, rp);
    if(ret != ZK_OK) {
        RETURN_ERROR("%s %s%s failed, get index partitions error\n");
    }
    //check if idc and group is valid.
    ret = manager.get_slavenodes_by_idc_group(&all_nodes, rp, request.idc_id, request.group_id);
    if(ret != ZK_OK) {
        RETURN_ERROR("%s \"%s\" %s failed, get slavenode error, may be idc or group invalid\n");
    }
    for(vector<zk_slavenode*>::iterator it=all_nodes.begin(); it!=all_nodes.end(); it++){
        ret = (*it)->get_meta_all(&meta, rp);
        if(ret != ZK_OK){
            fprintf(stderr, "placement-plan dump failed, get %s's meta failed\n", 
                    (*it)->get_id().c_str());
            return EXIT_FAILURE;
        }
        //slavenode: { slavenode_x: {attrs} }
        key = rp.create<bsl::string>((*it)->get_id().c_str());
        dict_slavenode[key] = *meta;
        all_instance.clear();
        ret = (*it)->get_all_service_instances(&all_instance, rp);
        if(ret != ZK_OK){
            fprintf(stderr, "placement-plan dump failed, get %s's service instance failed\n",
                    (*it)->get_id().c_str());
            return EXIT_FAILURE;
        }
        //placement: { slavenode_x: {instances} }
        dict_instance = &(rp.create<bsl::var::Array>());
        for(vector<zk_service_instance*>::iterator it=all_instance.begin(); 
                it != all_instance.end(); it++){
            if(ZK_OK != get_instance_attrs(*it, partition, &meta, rp)){//if not BS
                continue;
            }
            dict_instance->set(dict_instance->size(), *meta);
        }
        dict_placement[key] = *dict_instance;
    }
 
    result["index_partition"] = dict_partition;
    result["slavenode"] = dict_slavenode;
    result["placement"] = dict_placement;
    print_json(result);
    return EXIT_SUCCESS;
}
//////////////command//////
/** generate id */
int command_generateid(cli_request &request, zk_search_cluster_manager &/*manager*/)
{
    struct timeval now;
    char buffer[64];
    int ret;
    if(0 != gettimeofday(&now, NULL)){
        RETURN_ERROR("%s \"%s\" %s failed, gettimeofday fail\n");
    }
    ret = snprintf(buffer, sizeof(buffer), "cmd%010ld%05ld%05d", now.tv_sec, now.tv_usec, getpid());
    if(ret < 0){
        RETURN_ERROR("%s %s%s failed, snprintf error");
    }
    fprintf(stdout, "%s\n", buffer);
    return EXIT_SUCCESS;
}
/** add */
int command_add(cli_request &request, zk_search_cluster_manager &manager)
{
    int ret;
    zk_service_instance *service_instance;
    zk_command *command;
    bsl::ResourcePool local_rp;
    bsl::var::Dict *result_dict;
    bsl::var::JsonDeserializer jd(local_rp);
    ret = manager.get_object_by_id<zk_service_instance>(request.service_instance_id, 
            &service_instance, local_rp);
    if(ret != ZK_OK){
        RETURN_ERROR("%s \"%s\" %s failed, service instance not found\n");
    }
    try {
        bsl::var::IVar &tmp_meta = jd.deserialize(request.data.c_str());
        result_dict = &((bsl::var::Dict&)tmp_meta);
    } catch (bsl::Exception &e) {
        RETURN_ERROR("%s \"%s\" %s failed, invalid json\n");
    }
    ret = service_instance->new_command(request.command_id, &command, local_rp);
    if (ret != ZK_OK) {
        RETURN_ERROR("%s \"%s\" %s failed, this command may be exist already\n");
    }
    ret = command->set_meta_all(*result_dict);
    if(ret != ZK_OK) {
        RETURN_ERROR("%s \"%s\" %s failed, refer log for detail\n");
    }
    return EXIT_SUCCESS;
}
/** 辅助函数,』袢ommand */
static int find_command(cli_request &request, zk_search_cluster_manager &manager, 
        bsl::ResourcePool &rp, zk_service_instance **instance, zk_command **command)
{
    int ret = manager.get_service_instance_by_id(request.service_instance_id, instance, rp);
    if(ret != ZK_OK){
        fprintf(stderr, "command \"%s\" %s failed, service-instance %s not found\n",
                request.object_id.c_str(), request.action.c_str(), 
                request.service_instance_id.c_str());
        return ZK_ERROR;
    }
    ret = (*instance)->get_command_by_id(request.command_id, command, rp);
    if(ret != ZK_OK){
        fprintf(stderr, "command \"%s\" %s failed, command %s not found\n",
                request.object_id.c_str(), request.action.c_str(), request.command_id.c_str());
        return ZK_ERROR;
    }
    return ZK_OK;
}
/** del */
int command_del(cli_request &request, zk_search_cluster_manager &manager)
{
    bsl::ResourcePool local_rp;
    zk_service_instance *service_instance;
    zk_command *command;
    int ret = find_command(request, manager, local_rp, &service_instance, &command);
    if(ret != ZK_OK){
        return EXIT_FAILURE;
    }
    ret = service_instance->delete_command(command);
    if(ret != ZK_OK){
        RETURN_ERROR("%s \"%s\" %s failed, refer log for detail\n");
    }
    return EXIT_SUCCESS;
}
/** list */
int command_list(cli_request &request, zk_search_cluster_manager &manager)
{
    bsl::ResourcePool local_rp;
    zk_service_instance *service_instance;
    vector<zk_command*> all_commands;
    int ret = manager.get_object_by_id<zk_service_instance>(request.service_instance_id,
            &service_instance, local_rp);
    if(ret != ZK_OK){
        RETURN_ERROR("%s \"%s\" %s failed, can't find service-instance\n");
    }
    ret = service_instance->get_all_commands(&all_commands, local_rp);
    if(ret != ZK_OK){
        RETURN_ERROR("%s \"%s\" %s failed, refer log for detail\n");
    }
    cli_print_list<zk_command>(all_commands);
    return EXIT_SUCCESS;
}
/** set-meta-attr */
int command_set_meta_attr(cli_request &request, zk_search_cluster_manager &manager)
{
    bsl::ResourcePool local_rp;
    zk_service_instance *service_instance;
    zk_command *command;
    int ret = find_command(request, manager, local_rp, &service_instance, &command);
    if(ret != ZK_OK){
        return EXIT_FAILURE;
    }
    ret = command->set_meta_attr(request.key_values); 
    if (ret!= ZK_OK){
        RETURN_ERROR("%s \"%s\" %s failed, this key may not be exist\n");
    }
    return EXIT_SUCCESS;
}
/** get-meta-attr */
int command_get_meta_attr(cli_request &request, zk_search_cluster_manager &manager)
{
    bsl::ResourcePool local_rp;
    zk_service_instance *service_instance;
    zk_command *command;
    string attr_value;
    int ret = find_command(request, manager, local_rp, &service_instance, &command);
    if(ret != ZK_OK){
        return EXIT_FAILURE;
    }
    ret = command->get_meta_attr(request.key,  &attr_value); 
    if(ret != ZK_OK){
        RETURN_ERROR("%s \"%s\" %s failed, this key field may not be exist\n");
    }
    fprintf(stdout, "%s\n", attr_value.c_str());
    return EXIT_FAILURE;
}
/** add-meta-attr */
int command_add_meta_attr(cli_request &request, zk_search_cluster_manager &manager)
{
    bsl::ResourcePool local_rp;
    zk_service_instance *service_instance;
    zk_command *command;
    string attr_value;
    int ret = find_command(request, manager, local_rp, &service_instance, &command);
    if(ret != ZK_OK){
        return EXIT_FAILURE;
    }
    ret = command->add_meta_attr(request.key_values);
    if(ret != ZK_OK){
        RETURN_ERROR("%s \"%s\" %s failed, this key field may be exist already\n");
    }
    return EXIT_SUCCESS;
}
/** del-meta-attr */
int command_del_meta_attr(cli_request &request, zk_search_cluster_manager &manager)
{
    bsl::ResourcePool local_rp;
    zk_service_instance *service_instance;
    zk_command *command;
    string attr_value;
    int ret = find_command(request, manager, local_rp, &service_instance, &command);
    if(ret != ZK_OK){
        return EXIT_FAILURE;
    }
    ret = command->del_meta_attr(request.keys); 
    if(ret != ZK_OK){
        RETURN_ERROR("%s \"%s\" %s failed, this key field may not exist\n");
    }
    return EXIT_SUCCESS;
}
/** set-meta-all */
int command_set_meta_all(cli_request &request, zk_search_cluster_manager &manager)
{
    bsl::ResourcePool local_rp;
    bsl::var::JsonDeserializer jd(local_rp);
    bsl::var::Dict *result_dict;
    zk_service_instance *service_instance;
    zk_command *command;
    string attr_value;
    int ret = find_command(request, manager, local_rp, &service_instance, &command);
    if(ret != ZK_OK){
        return EXIT_FAILURE;
    }
    try {
        bsl::var::IVar &tmp_meta = jd.deserialize(request.data.c_str());
        result_dict = &((bsl::var::Dict&)tmp_meta);
    }
    catch (bsl::Exception &e) {
        RETURN_ERROR("%s \"%s\" %s failed, invalid json format\n");
    }
    ret = command->set_meta_all(*result_dict);
    if(ret != ZK_OK){
        RETURN_ERROR("%s \"%s\" %s failed, refer log for detail\n");
    }
    return EXIT_SUCCESS;
}
/** get-meta-all */	
int command_get_meta_all(cli_request &request, zk_search_cluster_manager &manager)
{
    bsl::ResourcePool local_rp;
    bsl::var::JsonSerializer js;
    bsl::var::Dict *result_dict;
    bsl::AutoBuffer tmp_buf;
    zk_service_instance *service_instance;
    zk_command *command;
    string attr_value;
    int ret = find_command(request, manager, local_rp, &service_instance, &command);
    if(ret != ZK_OK){
        return EXIT_FAILURE;
    }
    ret = command->get_meta_all(&result_dict, local_rp); 
    if(ret != ZK_OK) {
        RETURN_ERROR("%s \"%s\" %s failed, refer log for detail\n");
    }
    try
    {
        js.serialize(*result_dict, tmp_buf);
    }
    catch (bsl::Exception &e)
    {
        RETURN_ERROR("%s \"%s\" %s failed, invlaid json format\n");
    }
    fprintf(stdout,"%s\n", tmp_buf.c_str());
    return EXIT_SUCCESS;
}
/**
* @brief Main entry of the tool
*
* @param argv   : int argc char**
* @return  int 
* @retval   
* @author 
* 
**/
int main(int argc, char** argv) {
    int ret;
    cli_request request;

    //load conf
    ret = g_conf.parse_conf(ZK_DEFAULT_CONF_FILE);
    if(ret != 0){
        return EXIT_FAILURE;
    }
    //init log
    zk_log_init(g_conf, g_conf.m_log_zkctl);

    //log command
    char buffer[ZKSH_CMD_MAX_LEN];
    int offset = 0;
    for(int i=0; i<argc; i++){
        offset += snprintf(buffer+offset, sizeof(buffer)-offset, "%s ", argv[i]);
    }
    zk_log(UL_LOG_NOTICE, "zkctl cmd: %s", buffer);

    //parse options
    ret = cli_parse_options(argc, argv, request);
    if(ret != EXIT_SUCCESS){
        return ret;
    }

    //invoke action.
    return cli_invoke(request);
}
/** skip blanks and print str */
static void skip_and_print(int layer, FILE *stream, char *fmt, ...)
{
    char tmplog[ZK_LOG_MAX_LEN];
    va_list args;
    va_start(args, fmt);
    vsnprintf(tmplog, sizeof(tmplog), fmt, args);
    va_end(args);
    for(int i=0; i<layer; i++){
        fprintf(stream, "    ");
    }
    fprintf(stream, "%s", tmplog);
}
/** print json recursively */
static void print_json(bsl::var::IVar &var, int layer, FILE *stream){
    const char *seperator = "";
    if(var.is_dict()){
        fprintf(stream, "{");
        for(bsl::var::IVar::dict_iterator it = var.dict_begin(); it!=var.dict_end(); ++it){
            fprintf(stream, "%s\n", seperator);
            skip_and_print(layer+1, stdout, "\"%s\":", it->key().c_str());
            if(it->value().is_dict() || it->value().is_array()){
                print_json(it->value(), layer+1, stream);
            }
            else{
                fprintf(stream, "\"%s\"", it->value().to_string().c_str());
            }
            seperator = ",";
        }
        fprintf(stream, "\n");
        skip_and_print(layer, stream, "}");
    }
    else if(var.is_array()){
        fprintf(stream, "[");
        for(bsl::var::IVar::array_iterator it=var.array_begin(); it!=var.array_end(); ++it){
            fprintf(stream, "%s\n", seperator);
            if(it->value().is_dict() || it->value().is_array()){
                skip_and_print(layer+1, stdout, "");
            }
            print_json(it->value(), layer+1, stream);
            seperator = ",";
        }
        fprintf(stream, "\n");
        skip_and_print(layer, stream, "]");
    }
    else{
        skip_and_print(layer, stdout, "\"%s\"", var.to_string().c_str());
    }
}
/** print json object */
void print_json(bsl::var::IVar &var)
{
    print_json(var, 0, stdout);
    fprintf(stdout, "\n");
}
