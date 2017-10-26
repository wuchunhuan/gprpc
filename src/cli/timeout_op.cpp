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
#include <timeout_op.h>

single_sync_wait::single_sync_wait() : go_(false), waiting_(false){

}

single_sync_wait::~single_sync_wait() {
    the_condition_variable.notify_all();
}

single_sync_wait::wait_rst single_sync_wait::wait() {
    boost::unique_lock<boost::mutex> lock(the_mutex);
    if (waiting_) {
        return ALREADY_WAITING;
    }
    while(!go_) {
        waiting_ = true;
        the_condition_variable.wait(lock);
    }
    go_ = false;
    waiting_ = false;
    return WAIT_DONE;
}

single_sync_wait::signal_rst single_sync_wait::signal() {
    boost::unique_lock<boost::mutex> lock(the_mutex);
    if (!waiting_) {
        return NO_WAITER;
    }
    go_ = true;
    lock.unlock();
    the_condition_variable.notify_one();
    return SIG_DONE;
}

void single_sync_wait::signal_ex() {
    boost::unique_lock<boost::mutex> lock(the_mutex);
    while (!waiting_) {
        usleep(1000);
    }
    go_ = true;
    lock.unlock();
    the_condition_variable.notify_one();
}

void handle_connect(const error_code &e, single_sync_wait *syncer, bool *out_timeout, error_code *out_ec) {
    *out_timeout = false;
    while (syncer->signal() == single_sync_wait::NO_WAITER) {
        usleep(1000);
    }

    if (e) {
        *out_ec = e;
    }
}

bool connect_with_timeout(tcp::socket &s,
                          tcp::resolver::iterator &ep_it,
                          long time_out_ms) throw( system_error){
    bool sync_time_out = true;
    error_code ec = boost::asio::error::would_block;
    single_sync_wait syncer;
    boost::asio::async_connect(s, ep_it,
                               bind(&handle_connect, boost::asio::placeholders::error, &syncer, &sync_time_out, &ec));
    single_sync_wait::wait_rst rst = syncer.timed_wait(boost::posix_time::milliseconds(time_out_ms));
    if ( rst == single_sync_wait::TIMED_OUT || sync_time_out ) {
        s.cancel();
        throw system_error(
            boost::asio::error::timed_out
        );
    }

    if (ec != boost::asio::error::would_block) {
        throw system_error( ec );
    }
    return true;
}

void handle_resolve (const error_code &e,
                     tcp::resolver::iterator ep_it,
                     single_sync_wait* syncer,
                     tcp::resolver::iterator* out_it_ep,
                     bool* out_timeout,
                     error_code* out_ec) {
    *out_timeout = false;
    while (syncer->signal() == single_sync_wait::NO_WAITER) {
        usleep(1000);
    }

    if (!e) {
        *out_it_ep = ep_it;
    } else {
        *out_ec = e;
    }
}

tcp::resolver::iterator
resolve_with_timeout(tcp::resolver &resolver,
                     const std::string &addr,
                     const std::string &port,
                     long time_out_ms)  throw( system_error){
    bool sync_time_out = true;
    tcp::resolver::iterator ep_it;
    error_code ec = boost::asio::error::would_block;
    tcp::resolver::query query(addr, port);
    single_sync_wait syncer;
    resolver.async_resolve(query, boost::bind(&handle_resolve,
                                              boost::asio::placeholders::error, boost::asio::placeholders::iterator,
                                              &syncer, &ep_it, &sync_time_out, &ec));
    single_sync_wait::wait_rst rst = syncer.timed_wait(boost::posix_time::milliseconds(time_out_ms));
    if ( rst == single_sync_wait::TIMED_OUT || sync_time_out ) {
        resolver.cancel();
        throw system_error(
            boost::asio::error::timed_out
        );
    }

    if (ec != boost::asio::error::would_block) {
        throw system_error( ec );
    }
    return ep_it;
}

void handle_trans_with_timeout_ex(const error_code &e, size_t bytes_trans, single_sync_wait *syncer, size_t *out_bytes,
                                  bool *out_timeout, error_code *out_ec) {

    *out_timeout = false;
    while (syncer->signal() == single_sync_wait::NO_WAITER) {
        usleep(1000);
    }

    if (!e) {
        *out_bytes = bytes_trans;
    } else {
        *out_ec = e;
    }
}



