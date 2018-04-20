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
#ifndef MM_CACHE_CLUSTER_SRV_CONN_H
#define MM_CACHE_CLUSTER_SRV_CONN_H

#include <boost/enable_shared_from_this.hpp>
#include <connection.h>
#include <boost/asio.hpp>
#include <gprpc/common.h>
//#include <rpc_svc_mgr.h>
//#include "rpc_server.h"

using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;
//using namespace boost::system;
using namespace common;
using namespace gprpc;

namespace gprpc {
    namespace server {
        class rpc_svc_mgr;
        class srv_conn : boost::noncopyable, public connection, public boost::enable_shared_from_this<srv_conn>{
        public:
            srv_conn(io_service& nios, io_service& wios, rpc_svc_mgr& mgr);
            ~srv_conn();
            tcp::socket& socket();
            void start();
            void read_req_head();
            void handle_req_head(const boost::system::error_code& e, size_t bytes_trans);
            void handle_req_data(const boost::system::error_code& e, size_t bytes_trans);
            void set_buffer(const char * data, uint32_t data_len);
            void write_rsp();
            void handle_write(const boost::system::error_code& e);

            const char * get_buffer() const;

            const rpc_header &get_header() const;

            io_service& get_wios() const;

        private:
            io_service& wios_;
            tcp::socket socket_;
            rpc_svc_mgr& mgr_;
            boost::array<char, conn_buf_size_> buffer_;
            rpc_header header_;
        };
        typedef boost::shared_ptr<srv_conn> srv_conn_ptr;
    }
}


#endif //MM_CACHE_CLUSTER_SRV_CONN_H
