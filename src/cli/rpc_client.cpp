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
#include <boost/thread.hpp>
#include <gprpc/rpc_client.h>
boost::atomic<uint32_t> gprpc::client::rpc_client::seq_{0};


uint32_t gprpc::client::rpc_client::generate_seqid() {
    return ++seq_;
}

uint32_t gprpc::client::rpc_client::get_async_io_thread_num() const {
    return async_io_thread_num_;
}

uint32_t gprpc::client::rpc_client::get_async_work_thread_num() const {
    return async_work_thread_num_;
}

const ios_ptr &gprpc::client::rpc_client::get_nios() const {
    return nios_;
}

const ios_ptr &gprpc::client::rpc_client::get_wios() const {
    return wios_;
}

void gprpc::client::rpc_client::join() {
    if (!native_io_) {
        return;
    }
    if (async_io_thread_num_ > 0 && async_work_thread_num_ > 0) {

        for (size_t i = 0; i < async_io_thread_num_; ++i) {
            nthreads_[i]->join();
        }

        for (size_t i = 0; i < async_work_thread_num_; ++i) {
            wthreads_[i]->join();
        }
    }
}

gprpc::client::rpc_client::rpc_client()
    : inited_(false),
      native_io_(false),
      async_io_thread_num_(0),
      async_work_thread_num_(0),
      nios_(NULL),
      wios_(NULL),
      dummy_io(NULL),
      dummy_work(NULL) {

}

bool gprpc::client::rpc_client::init(uint32_t async_io_thread_num, uint32_t async_work_thread_num) {
    if (inited_) {
        return true;
    }
    async_io_thread_num_ = async_io_thread_num;
    async_work_thread_num_ = async_work_thread_num;
    nios_.reset(new io_service());
    wios_.reset(new io_service());
    dummy_io.reset(new io_service::work(*nios_));
    dummy_work.reset(new io_service::work(*wios_));

    for (size_t i = 0; i < async_io_thread_num_; ++i) {
        boost::shared_ptr<boost::thread> t(new boost::thread(bind(&boost::asio::io_service::run, nios_)));
        nthreads_.push_back(t);
    }

    for (size_t i = 0; i < async_work_thread_num_; ++i) {
        boost::shared_ptr<boost::thread> t(new boost::thread(bind(&boost::asio::io_service::run, wios_)));
        wthreads_.push_back(t);
    }
    inited_ = true;
    native_io_ = true;
    return true;
}

bool gprpc::client::rpc_client::init(ios_ptr nios, ios_ptr wios) {
    if (inited_) {
        return true;
    }
    if (nios->stopped() || wios->stopped()) {
        return false;
    }
    nios_ = nios;
    wios_ = wios;
    inited_ = true;
    native_io_ = false;
    return true;
}
