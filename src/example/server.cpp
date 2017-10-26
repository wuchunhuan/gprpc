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

#include <gprpc/rpc_server.h>
#include "ping.pb.h"

class ping_service_impl : public ping_service {
public:
    ping_service_impl() {}
    virtual void ping(::google::protobuf::RpcController* controller,
                      const ::ping_req* request,
                      ::pong_rsp* response,
                      ::google::protobuf::Closure* done) {
        std::string ping = request->ping();
        std::string pong = ping + ":" + "pong";
        response->set_pong(pong);

        /**
         * Important: Always call done->Run() on finished to notify rpc server that this call has been
         * processed, no matter in synchronous or asynchronous manner, which means you could
         * either process this rpc call in this method(synchronously) or call other asynchronous
         * method with all the arguments provided in this method to process this rpc call.
         * Either way, done->Run() must be called. After you call done->Run(),
         * request or response should not be accessed!
         */
        done->Run();
        return;
    }
};

int main() {
    boost::shared_ptr<ping_service_impl> service_ptr(new ping_service_impl());
    std::string port = "8998";
    size_t io_thread_num = 2;
    size_t work_thread_num = 2;
    gprpc::server::rpc_server server;
    server.init(io_thread_num, work_thread_num, port);
    server.register_service(service_ptr);
    server.start();
    server.join();
    return 0;
}