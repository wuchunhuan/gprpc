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
#ifndef MM_CACHE_CLUSTER_RPC_CHANNEL_H
#define MM_CACHE_CLUSTER_RPC_CHANNEL_H

#include <cstdint>
#include <string>
#include <gprpc/common.h>

namespace gprpc {
    namespace client {
        class rpc_client;
        class rpc_channel_impl;
        typedef boost::shared_ptr<rpc_channel_impl> rpc_channel_impl_ptr;
    }
}

namespace gprpc {
    namespace client {

        class rpc_channel : public RpcChannel {
        public:
            rpc_channel(rpc_client& client, const std::string& address, const std::string& port);

            /**
             * Do not call this method directly, call the STUB method generated in corresponding pb header
             * @param method
             * @param controller
             * @param request
             * @param response
             * @param done
             */
            void CallMethod(const MethodDescriptor* method,
                            RpcController* controller,
                            const Message* request,
                            Message* response,
                            Closure* done);

            const string &get_address() const;

            const string &get_port() const;

            rpc_client &get_client() const;

        private:
            std::string address_;
            std::string port_;
            rpc_client& client_;
            rpc_channel_impl_ptr impl_;
        };
    }
}


#endif //MM_CACHE_CLUSTER_RPC_CHANNEL_H
