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
#ifndef MM_CACHE_CLUSTER_CLI_CONN_H
#define MM_CACHE_CLUSTER_CLI_CONN_H

#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>
#include <connection.h>
#include <gprpc/common.h>
using namespace common;
using namespace boost::asio::ip;
//using namespace boost::system;
namespace gprpc {
    class rpc_controller;
}
namespace google {
    namespace protobuf {
        class Closure;
    }
}

namespace gprpc {
    namespace client {

        typedef enum timout_reason_e {
            TOTAL_TIMEOUT = 0,
            RESOLVE_TIMEOUT,
            CONNECT_TIMEOUT,
            RECEIVE_TIMEOUT,
            SEND_TIMEOUT,
            NO_TIMEOUT
        } timout_reason;

        class rpc_channel_impl;
        class cli_conn : boost::noncopyable, public connection, public boost::enable_shared_from_this<cli_conn>{
        public:
            cli_conn(rpc_channel_impl& channel, Closure& done, gprpc::rpc_controller& controller, Message* response);
            ~cli_conn();
            void start();
            tcp::socket& socket();
            void set_buffer(std::string& data);
            rpc_header &get_header();

            Closure &get_done();

            rpc_controller &get_controller();

            Message* get_response();

            const char * get_buffer();

            const rpc_header &get_header() const;

            bool is_sync_call();

        private:
            void send();
            void handle_send(const boost::system::error_code &e, size_t bytes_trans);
            void resolve();
            void handle_resolve(const boost::system::error_code &e, tcp::resolver::iterator ep_it);
            void connect();
            void handle_connect(const boost::system::error_code &e);
            void rcv_rsp_header();
            void handle_rsp_header(const boost::system::error_code &e, size_t bytes_trans);
            void rcv_rsp_data();
            void handle_rsp_data(const boost::system::error_code &e, size_t bytes_trans);
            void done(int32_t err_code, std::string extra_err_msg = "");
            void handle_total_timeout(const boost::system::error_code &e);
            void handle_resolve_timeout(const boost::system::error_code &e);
            void handle_conn_timeout(const boost::system::error_code &e);
            void handle_sock_timeout(const boost::system::error_code &e, timout_reason tr);
            const std::string& get_timeout_reason();

        private:
            bool sync_call_;
            timout_reason tr_;
            rpc_channel_impl& channel_impl_;
            tcp::socket socket_;
            deadline_timer total_deadline_;
            deadline_timer deadline_;
//            uint32_t    connect_retry_num_;
            ios_ptr wios_;
            boost::array<char, conn_buf_size_> buffer_;
            rpc_header header_;
            Closure& done_;
            gprpc::rpc_controller& controller_;
            Message* response_;
            tcp::resolver::iterator it_ep_;
        };
        typedef boost::shared_ptr<cli_conn> cli_conn_ptr;
    }
}


#endif //MM_CACHE_CLUSTER_CLI_CONN_H
