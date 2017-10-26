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
#ifndef MM_CACHE_CLUSTER_CONNECTION_H
#define MM_CACHE_CLUSTER_CONNECTION_H


#include <cstdint>
#include <boost/date_time/microsec_time_clock.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/atomic.hpp>
#define conn_buf_size_ (50 * 1024)

namespace common {
    class connection {
    public:
        connection()
            : start_time_(0),
              end_time_(0),
              tag_(""){
            seq_.store(0);
        }
        virtual void start() = 0;


        const std::string &get_tag() const;

        const int64_t& get_start_time();

        const int64_t& get_end_time();

        void set_start_time(int64_t start);

        void set_end_time(int64_t end);

        int64_t get_time_stamp();

        void set_tag(const std::string &tag_);

        const int64_t get_elapsed_time();

    protected:
        int64_t start_time_;
        int64_t end_time_;
        static boost::atomic<unsigned long> seq_;
        std::string tag_;
    };
}


#endif //MM_CACHE_CLUSTER_CONNECTION_H
