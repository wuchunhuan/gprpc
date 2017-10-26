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
#include <iostream>
#include <gprpc/rpc_client.h>
#include <gprpc/rpc_controller.h>
#include <gprpc/rpc_channel.h>
#include <gprpc/closure.h>
#include "ping.pb.h"

void call_back(gprpc::rpc_controller* controller, ping_req* request, pong_rsp* response) {
    //make sure to check response status by executing controller->Failed(), if failed, response value
    //should be considered as undefined.
    if (controller->Failed()) {
        std::cout << "Rpc call failed, " << controller->ErrorCode() << " " << controller->ErrorText()
                  << " " << controller->error_text_detail() << std::endl;
    } else {
        std::cout << "Rpc call succeed, response: " << response->pong() << std::endl;
    }
    delete controller;
    delete request;
    delete response;
}


int main() {
    gprpc::client::rpc_client client;
    client.init();
    std::string address = "0.0.0.0";
    std::string port = "8998";
    gprpc::client::rpc_channel channel(client, address, port);

    gprpc::rpc_controller* cntl = new gprpc::rpc_controller;
    cntl->set_total_timeout(5000);
//    cntl->set_resolve_timeout(10);
//    cntl->set_connect_timeout(10);
//    cntl->set_write_timeout(20);
//    cntl->set_read_timeout(20);

    ping_req* request = new ping_req;
    request->set_ping("ping");
    pong_rsp* response = new pong_rsp;
    Closure* done = new_cli_rpc_callback(&call_back, cntl, request, response);

    ping_service_Stub stub(&channel);
    stub.ping(cntl, request, response, done);
    client.join();
    return 0;
}


