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
#ifndef MM_CACHE_CLUSTER_RPC_CLIENT_H
#define MM_CACHE_CLUSTER_RPC_CLIENT_H
#include <cstdint>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <gprpc/common.h>
using namespace boost::asio;

namespace gprpc {
    namespace client {
        class rpc_client {
        public:
            rpc_client();
            bool init(uint32_t async_io_thread_num = 2, uint32_t async_work_thread_num = 2);
            bool init(ios_ptr nios, ios_ptr wios);
            static uint32_t generate_seqid();

            uint32_t get_async_io_thread_num() const;

            uint32_t get_async_work_thread_num() const;

            const ios_ptr &get_nios() const;

            const ios_ptr &get_wios() const;

            void join();

        private:
            bool inited_;
            bool native_io_;
            uint32_t async_io_thread_num_;
            uint32_t async_work_thread_num_;

            ios_ptr nios_;
            ios_ptr wios_;
            work_ptr dummy_io;
            work_ptr dummy_work;
            std::vector<boost::shared_ptr<boost::thread> > nthreads_;
            std::vector<boost::shared_ptr<boost::thread> > wthreads_;
            static boost::atomic<uint32_t> seq_;
        };
    }
}


#endif //MM_CACHE_CLUSTER_RPC_CLIENT_H
