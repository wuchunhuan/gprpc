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
#ifndef MM_CACHE_CLUSTER_TIMEOUT_OP_H
#define MM_CACHE_CLUSTER_TIMEOUT_OP_H
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/system/error_code.hpp>

using namespace boost::system;
using namespace boost::asio::ip;

class single_sync_wait :private boost::noncopyable{
public:
    typedef enum wait_rst_e {
        WAIT_DONE = 0,
        ALREADY_WAITING,
        TIMED_OUT
    } wait_rst;
    typedef enum signal_rst_e {
        SIG_DONE = 0,
        NO_WAITER
    } signal_rst;
    single_sync_wait();
    ~single_sync_wait();
    wait_rst wait();

    template<typename duration_type>
    wait_rst timed_wait(duration_type const& wait_duration) {
        boost::unique_lock<boost::mutex> lock(the_mutex);
        if (waiting_) {
            return ALREADY_WAITING;
        }
        while(!go_) {
            waiting_ = true;
            if(!the_condition_variable.timed_wait(lock, wait_duration)) {
                go_ = false;
                waiting_ = false;
                return TIMED_OUT;
            }
        }
        go_ = false;
        waiting_ = false;
        return WAIT_DONE;
    }

    signal_rst signal();

    void signal_ex();

private:
    boost::mutex the_mutex;
    boost::condition_variable the_condition_variable;
    bool go_;
    bool waiting_;
};

bool connect_with_timeout(tcp::socket& s,
                          tcp::resolver::iterator& ep_it,
                          long time_out_ms) throw( system_error);



tcp::resolver::iterator resolve_with_timeout(tcp::resolver& resolver,
                                             const std::string& addr,
                                             const std::string& port,
                                             long time_out_ms) throw( system_error);


void handle_trans_with_timeout_ex(const error_code &e,
                                  size_t bytes_trans,
                                  single_sync_wait* syncer,
                                  size_t* out_bytes,
                                  bool* out_timeout,
                                  error_code* out_ec);


template <typename SyncReadStream, typename MutableBufferSequence>
size_t read_with_timeout_ex(SyncReadStream& s,
                            const MutableBufferSequence& buffer,
                            size_t size, long time_out_ms) throw( system_error){
    bool sync_time_out = true;
    size_t sync_trans_size = 0;
    error_code ec = boost::asio::error::would_block;
    single_sync_wait syncer;
    boost::asio::async_read(
        s,
        buffer,
        boost::asio::transfer_exactly( size ),
        boost::bind( &handle_trans_with_timeout_ex,
                     boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred,
                     &syncer, &sync_trans_size, &sync_time_out, &ec));

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

    return sync_trans_size;
}

template <typename SyncReadStream, typename MutableBufferSequence>
size_t write_with_timeout_ex(SyncReadStream& s,
                            const MutableBufferSequence& buffer,
                            size_t size, long time_out_ms) throw( system_error){
    bool sync_time_out = true;
    size_t sync_trans_size = 0;
    error_code ec = boost::asio::error::would_block;
    single_sync_wait syncer;
    boost::asio::async_write(
        s,
        buffer,
        boost::asio::transfer_exactly( size ),
        boost::bind( &handle_trans_with_timeout_ex,
                     boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred,
                     &syncer, &sync_trans_size, &sync_time_out, &ec));

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

    return sync_trans_size;
}

#endif //MM_CACHE_CLUSTER_TIMEOUT_OP_H
