/*
 *
 * Copyright © 2017 Patrick Wu(Wu chunhuan)/wuchunhuan@gmail.com.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#ifndef MM_CACHE_CLUSTER_RPC_SVC_MGR_H
#define MM_CACHE_CLUSTER_RPC_SVC_MGR_H

#include <gprpc/common.h>
#include <map>
#include <gprpc/rpc_controller.h>
//#include <srv_conn.h>

using namespace gprpc;

namespace gprpc{
namespace server{
    class srv_conn;
    typedef boost::shared_ptr<srv_conn> srv_conn_ptr;
    class rpc_svc_mgr {
    public:
        rpc_svc_mgr();
        bool register_service(service_ptr service);
        bool handle_rpc_call(srv_conn_ptr conn);
        void process_response(controller_ptr controller, srv_conn_ptr conn, message_ptr response);
    private:
        std::map<std::string, service_ptr> services_;
    };
    typedef boost::shared_ptr<rpc_svc_mgr> svc_mgr_ptr;
}
}


#endif //MM_CACHE_CLUSTER_RPC_SVC_MGR_H
