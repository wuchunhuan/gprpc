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
#include <gprpc/rpc_channel.h>
#include <gprpc/rpc_client.h>
#include <gprpc.pb.h>
#include <gprpc/rpc_controller.h>
#include <rpc_channel_impl.h>
#include <cli_conn.h>

gprpc::client::rpc_channel::rpc_channel(gprpc::client::rpc_client &client, const std::string &address, const std::string& port)
    : address_(address),
      port_(port),
      client_(client),
      impl_(new gprpc::client::rpc_channel_impl(client, address, port)){

}

void gprpc::client::rpc_channel::CallMethod(const MethodDescriptor *method,
                                     RpcController *controller,
                                     const Message *request,
                                     Message *response,
                                     Closure *done) {
    impl_->call_method(method, controller, request, response, done);
}


const string &gprpc::client::rpc_channel::get_address() const {
    return address_;
}

const string &gprpc::client::rpc_channel::get_port() const {
    return port_;
}

gprpc::client::rpc_client &gprpc::client::rpc_channel::get_client() const {
    return client_;
}
