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
#include "connection.h"
boost::atomic<unsigned long> common::connection::seq_{0};

const std::string &common::connection::get_tag() const {
    return tag_;
}

const int64_t &common::connection::get_start_time() {
    return start_time_;
}

const int64_t &common::connection::get_end_time() {
    return end_time_;
}

void common::connection::set_start_time(int64_t start) {
    start_time_ = start;
}

void common::connection::set_end_time(int64_t end) {
    end_time_ = end;
}

int64_t common::connection::get_time_stamp() {
    boost::posix_time::ptime epoch(boost::gregorian::date(1970, boost::gregorian::Jan, 1));
    boost::posix_time::time_duration time_from_epoch =
        boost::posix_time::microsec_clock::universal_time() - epoch;
    return time_from_epoch.total_milliseconds();
}

void common::connection::set_tag(const std::string &tag) {
    tag_ = tag;
}

const int64_t common::connection::get_elapsed_time() {
    return get_end_time() - get_start_time();
}
