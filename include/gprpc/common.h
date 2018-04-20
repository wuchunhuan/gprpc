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
#ifndef MM_CACHE_CLUSTER_COMMON_H
#define MM_CACHE_CLUSTER_COMMON_H

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/service.h>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

//using namespace std;
using namespace ::google::protobuf;
using namespace boost::asio;
using namespace boost::asio::ip;
typedef boost::shared_ptr<Service> service_ptr;
typedef boost::shared_ptr<Message> message_ptr;
typedef boost::shared_ptr<io_service> ios_ptr;
typedef boost::shared_ptr<tcp::acceptor> acceptor_ptr;
typedef boost::shared_ptr<signal_set> signal_ptr;
typedef boost::shared_ptr<deadline_timer> timer_ptr;
typedef boost::shared_ptr<io_service::work> work_ptr;
namespace gprpc {

    const static int RPC_MAGIC_LEN = 4;
    const static char rpc_magic[4]  = {'_','R','P','C'};
    const static int RPC_DATALEN_LEN = 4;
    struct rpc_header {
        char magic[4];
        uint32_t data_len;
    };

}
#endif //MM_CACHE_CLUSTER_COMMON_H
