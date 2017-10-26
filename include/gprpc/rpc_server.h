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
#ifndef MM_CACHE_CLUSTER_RPC_SERVER_H
#define MM_CACHE_CLUSTER_RPC_SERVER_H
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <gprpc/common.h>
//#include "rpc_svc_mgr.h"
//#include "srv_conn.h"

using namespace std;
using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;

#include <boost/noncopyable.hpp>

namespace gprpc {
    namespace server {
        class rpc_svc_mgr;
        class srv_conn;
        typedef boost::shared_ptr<srv_conn> srv_conn_ptr;
        class rpc_server : boost::noncopyable{
        public:
            rpc_server();
//            ~rpc_server();
            bool init(std::string port = std::string("8998"), size_t nthread_num = 0);
            bool init(size_t nthread_num, size_t wthread_num, std::string& port);

            //use other running services, make sure the specified io_service is running, or it will failed
            bool init(ios_ptr ios, std::string& port);
            bool init(ios_ptr nios, ios_ptr wios, std::string& port);

            bool register_service(service_ptr service);

            bool start();
            void join();
            void stop();
            io_service &get_nios();

            io_service &get_wios();

        private:
            void listen();
            void start_accept();
            void handle_accept(srv_conn_ptr connection, const boost::system::error_code& e);


        private:
            bool inited_;
            bool native_io_;
            ios_ptr nios_;
            ios_ptr wios_;
            acceptor_ptr acceptor_;
            std::string port_;
//            srv_conn_ptr connection_;
            size_t nthread_num_;
            size_t wthread_num_;
            work_ptr dummy_work_;
            vector<boost::shared_ptr<boost::thread> > nthreads_;
            vector<boost::shared_ptr<boost::thread> > wthreads_;
            boost::shared_ptr<rpc_svc_mgr> svc_mgr_;
        };
    }
}

#endif //MM_CACHE_CLUSTER_RPC_SERVER_H
