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
#include <rpc_debug.h>
#include <gprpc/rpc_server.h>
#include <srv_conn.h>
#include <rpc_svc_mgr.h>

bool gprpc::server::rpc_server::register_service(service_ptr service) {
    if (!inited_) {
        return false;
    }
    return svc_mgr_->register_service(service);
}

bool gprpc::server::rpc_server::start() {
    if (!inited_) {
        return false;
    }

    if (!native_io_) {
        return true;
    }

    if ((nthread_num_ > 0 && wthread_num_ <= 0) || (wthread_num_ > 0 && nthread_num_ <= 0)) {

        size_t thread_num = std::max(wthread_num_, nthread_num_);

        for (size_t i = 0; i < thread_num; ++i) {
            boost::shared_ptr<boost::thread> t(new thread(bind(&boost::asio::io_service::run, nios_)));
            nthreads_.push_back(t);
        }
    }

    if (nthread_num_ > 0 && wthread_num_ > 0) {

        for (size_t i = 0; i < nthread_num_; ++i) {
            boost::shared_ptr<boost::thread> t(new thread(bind(&boost::asio::io_service::run, nios_)));
            nthreads_.push_back(t);
        }

        for (size_t i = 0; i < wthread_num_; ++i) {
            boost::shared_ptr<boost::thread> t(new thread(bind(&boost::asio::io_service::run, wios_)));
            wthreads_.push_back(t);
        }
    }

    return true;
}

io_service &gprpc::server::rpc_server::get_nios() {
    return *nios_;
}

io_service &gprpc::server::rpc_server::get_wios() {
    return *wios_;
}

//bool gprpc::server::rpc_server::process_data(srv_conn_ptr conn) {
//    return svc_mgr_.handle_rpc_call(conn);
//}

gprpc::server::rpc_server::rpc_server()
    : inited_(false),
      native_io_(false),
      nios_(NULL),
      wios_(NULL),
      acceptor_(NULL),
      port_("8998"),
//      connection_(NULL),
      nthread_num_(0),
      wthread_num_(0),
      dummy_work_(NULL),
      svc_mgr_(NULL) {
    svc_mgr_.reset(new gprpc::server::rpc_svc_mgr());
}

bool gprpc::server::rpc_server::init(ios_ptr ios, std::string& port) {

    return init(ios, ios, port);
}

bool gprpc::server::rpc_server::init(ios_ptr nios, ios_ptr wios, std::string& port) {

    if (inited_) {
        return true;
    }
    if (nios->stopped() || wios->stopped()) {
        return false;
    }

    nios_ = nios;
    wios_ = wios;
    port_ = port;

    listen();
    inited_ = true;
    start_accept();
    i_log << "Rpc server listened on " << ":" << port_;
    return true;
}

//remove connection_ from rpc_server class
void gprpc::server::rpc_server::start_accept() {
    srv_conn_ptr connection(new gprpc::server::srv_conn(*nios_, *wios_, *svc_mgr_));
    acceptor_->async_accept(connection->socket(), bind(&gprpc::server::rpc_server::handle_accept,
                                                        this, connection, boost::asio::placeholders::error));
}

void gprpc::server::rpc_server::handle_accept(srv_conn_ptr connection, const boost::system::error_code& e) {
    if (!e) {
        connection->start();
    }
    start_accept();
}

bool server::rpc_server::init(std::string port, size_t nthread_num) {
    return init(nthread_num, 0, port);
}

void server::rpc_server::listen() {
    acceptor_.reset(new tcp::acceptor(*nios_));
    tcp::endpoint ep( ip::tcp::v4(), boost::lexical_cast<unsigned short>(port_));
    acceptor_->open(ep.protocol());
    acceptor_->set_option(tcp::acceptor::reuse_address(true));
    acceptor_->bind(ep);
    acceptor_->listen();
}

bool server::rpc_server::init(size_t nthread_num, size_t wthread_num, std::string &port) {
    if (inited_) {
        return true;
    }
    nthread_num_ = nthread_num;
    wthread_num_ = wthread_num;
    nios_.reset(new io_service());
    wios_ = nios_;

    if (nthread_num_ > 0 && wthread_num_ > 0) {
        wios_.reset(new io_service());
        dummy_work_.reset(new io_service::work(*wios_));
    }
    port_ = port;
    listen();
    native_io_ = true;
    inited_ = true;
    start_accept();
    i_log << "Rpc server listened on " << ":" << port;
    return true;
}

void server::rpc_server::join() {
    if (!inited_ || !native_io_) {
        return;
    }

    if (nthread_num_ <= 0 && wthread_num_ <= 0) {
        nios_->run();
    }

    if ((nthread_num_ > 0 && wthread_num_ <= 0) || (wthread_num_ > 0 && nthread_num_ <= 0)) {
        size_t thread_num = std::max(wthread_num_, nthread_num_);

        for (size_t i = 0; i < thread_num; ++i) {
            nthreads_[i]->join();
        }
    }

    if (nthread_num_ > 0 && wthread_num_ > 0) {

        for (size_t i = 0; i < nthread_num_; ++i) {
            nthreads_[i]->join();
        }

        for (size_t i = 0; i < wthread_num_; ++i) {
            wthreads_[i]->join();
        }
    }
}

void server::rpc_server::stop() {
    if (!inited_ || !native_io_) {
        return;
    }

    nios_->stop();

    if (nthread_num_ > 0 && wthread_num_ > 0) {
        wios_->stop();
    }
}

