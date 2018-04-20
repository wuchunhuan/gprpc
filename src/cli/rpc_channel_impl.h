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
#ifndef MM_CACHE_CLUSTER_RPC_CHANNEL_IMPL_H
#define MM_CACHE_CLUSTER_RPC_CHANNEL_IMPL_H

#include <timeout_op.h>
#include <cstdint>
#include <string>
#include <gprpc/common.h>
#include <boost/thread/shared_mutex.hpp>

namespace gprpc {
    class rpc_data;
    class rpc_controller;
    namespace client {
        class rpc_client;
        class cli_conn;
        typedef boost::shared_ptr<cli_conn> cli_conn_ptr;
    }
}


namespace gprpc {
    namespace client {

        class rpc_channel_impl {
        public:

            typedef boost::shared_mutex wr_mutex;
            typedef boost::unique_lock<wr_mutex> w_lock;
            typedef boost::shared_lock<wr_mutex> r_lock;

            rpc_channel_impl(rpc_client& client, const std::string& address, const std::string& port);

            const string &get_address() const;

            const string &get_port() const;

            rpc_client &get_client() const;

            tcp::resolver &get_resolver();

            tcp::resolver::iterator &get_it_ep();

//            void set_it_ep(tcp::resolver::iterator & it_ep);
            void set_it_ep_if_empty(tcp::resolver::iterator & it_ep);

            void handle_rpc_response(cli_conn_ptr conn);

            void call_method(const MethodDescriptor* method,
                            RpcController* controller,
                            const Message* request,
                            Message* response,
                            Closure* done);

        private:
            bool set_rpc_data(gprpc::rpc_data& data,
                              const MethodDescriptor* method,
                              gprpc::rpc_controller* controller,
                              const Message* request);

            void do_rpc(rpc_data& data,
                        gprpc::rpc_controller* controller,
                        Message* response,
                        Closure* done);

            void abort(gprpc::rpc_controller* controller, Closure *done, int32_t err_code, std::string extra_err_info = "");

            void handle_rpc_response(gprpc::rpc_controller* controller,
                                     const void* buffer,
                                     int size,
                                     Message *response);

        private:
            std::string address_;
            std::string port_;
            rpc_client& client_;
//            cli_conn_ptr conn_;
            tcp::resolver resolver_;
            wr_mutex mutex_;
            tcp::resolver::iterator it_ep_;
        };
    }
}

#endif //MM_CACHE_CLUSTER_RPC_CHANNEL_IMPL_H
