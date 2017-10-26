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
#include <gprpc/common.h>
#include <cli_conn.h>
#include <gprpc/rpc_client.h>
#include <gprpc/rpc_channel.h>
#include <rpc_debug.h>
#include <gprpc/rpc_controller.h>
#include <rpc_controller_imp.h>
#include <rpc_channel_impl.h>


gprpc::client::cli_conn::cli_conn(gprpc::client::rpc_channel_impl& channel,
                                Closure& done,
                                gprpc::rpc_controller& controller,
                                Message* response)
    : sync_call_(false),
      tr_(NO_TIMEOUT),
      channel_impl_(channel),
      socket_(*channel.get_client().get_nios()),
      total_deadline_(*channel.get_client().get_wios()),
      deadline_(*channel.get_client().get_wios()),
//      connect_retry_num_(2),
      wios_(channel.get_client().get_wios()),
      done_(done),
      controller_(controller),
      response_(response){
    if (response_ == NULL) {
        sync_call_ = true;
    }
}
gprpc::client::cli_conn::~cli_conn() {
}

void gprpc::client::cli_conn::handle_total_timeout(const error_code &e) {
    if (!e) {
        tr_ = TOTAL_TIMEOUT;
        channel_impl_.get_resolver().cancel();
//        deadline_.cancel();
        boost::system::error_code ignored_ec;
        socket_.close(ignored_ec);
    }
}

void gprpc::client::cli_conn::handle_resolve_timeout(const error_code &e) {
    if (!e) {
        tr_ = RESOLVE_TIMEOUT;
        channel_impl_.get_resolver().cancel();
//        total_deadline_.cancel();
    }
}

void gprpc::client::cli_conn::handle_conn_timeout(const error_code &e) {
    if (!e) {
        tr_ = CONNECT_TIMEOUT;
        boost::system::error_code ignored_ec;
        socket_.close(ignored_ec);
    }
}

void gprpc::client::cli_conn::handle_sock_timeout(const error_code &e, timout_reason tr) {
    if (!e) {
        tr_ = tr;
        boost::system::error_code ignored_ec;
        socket_.close(ignored_ec);
//        total_deadline_.cancel();
    }
}

void gprpc::client::cli_conn::start() {
    set_start_time(get_time_stamp());
    seq_++;
    tcp::resolver::iterator end;

    uint32_t total_timeout = controller_.get_total_timeout();
    if (total_timeout > 0) {
        total_deadline_.expires_from_now(boost::posix_time::milliseconds(total_timeout));
        total_deadline_.async_wait(boost::bind(&gprpc::client::cli_conn::handle_total_timeout, shared_from_this(),
                                               boost::asio::placeholders::error));
    }

    if (channel_impl_.get_it_ep() != end) {
        connect();
    } else {
        resolve();
    }
    i_log << "rpc call " << get_tag() << " started," << " sts: " << get_start_time();
}

tcp::socket &gprpc::client::cli_conn::socket() {
    return socket_;
}

void gprpc::client::cli_conn::set_buffer(std::string& data) {

    size_t header_size = sizeof(header_);
    memcpy(buffer_.data(), &header_, header_size);
    memcpy(buffer_.data() + header_size, data.c_str(), data.size());
}

gprpc::rpc_header &gprpc::client::cli_conn::get_header() {
    return header_;
}

void gprpc::client::cli_conn::send() {
    uint32_t write_timeout = controller_.get_write_timeout();
    if (write_timeout > 0) {
        deadline_.expires_from_now(boost::posix_time::milliseconds(write_timeout));
        deadline_.async_wait(boost::bind(&gprpc::client::cli_conn::handle_sock_timeout, shared_from_this(),
                                         boost::asio::placeholders::error, SEND_TIMEOUT));
    } else {
        deadline_.cancel();
    }
    d_log << "Rpc call: " << controller_.impl_->get_call_tag() << ", start sending request...";
    size_t size = sizeof(header_) + header_.data_len;
    async_write(socket_, buffer(buffer_), transfer_exactly(size),
                bind(&gprpc::client::cli_conn::handle_send, shared_from_this(),
                     boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void gprpc::client::cli_conn::handle_send(const error_code &e, size_t bytes_trans) {
    if (!e) {
        if (bytes_trans != sizeof(header_) + header_.data_len) {
            done(-4, "request message not sent completely");
        } else {
            d_log << "Rpc call: " << controller_.impl_->get_call_tag() << ", request sent.";
            rcv_rsp_header();
        }

    } else {
        std::stringstream ss;
        ss << e.message();
        if (e == boost::asio::error::operation_aborted) {
            ss << ", " << get_timeout_reason();
        }
        done(-4, ss.str());
    }
}

void gprpc::client::cli_conn::done(int32_t err_code, std::string extra_err_msg) {
    deadline_.cancel();
    total_deadline_.cancel();

    if (err_code != 0) {
        controller_.impl_->set_failed(err_code);
        controller_.impl_->set_error_detail(extra_err_msg);
    }

    std::stringstream ss;
    if (err_code == 0) {
        ss << "Rpc call: "
               << controller_.impl_->get_call_tag()
               << " rpc data receiving successfully: "
               << controller_.ErrorCode()
               << ", "
               << controller_.ErrorText();
        if (!extra_err_msg.empty()) {
            ss << ", " << extra_err_msg;
        }
        i_log << ss.str();
    } else {
        ss << "Rpc call: "
               << controller_.impl_->get_call_tag()
               << " failed, error code: "
               << controller_.ErrorCode()
               << ", "
               << controller_.ErrorText();
        if (!extra_err_msg.empty()) {
            ss << ", " << extra_err_msg;
        }
        e_log << ss.str();
//        wios_->post(boost::bind(&Closure::Run, &done_));
    }

    if (sync_call_) {
        done_.Run();
    } else {
        wios_->post(boost::bind(&gprpc::client::rpc_channel_impl::handle_rpc_response, &channel_impl_, shared_from_this()));
    }
}

void gprpc::client::cli_conn::handle_connect(const error_code &e) {
    if (!e) {
        d_log << "Rpc call: " << controller_.impl_->get_call_tag() << ", connected!";
        send();
    } else {
        std::stringstream ss;
        ss << "address: " << channel_impl_.get_address() << ", port: " << channel_impl_.get_port() << e.message();
        if (e == boost::asio::error::operation_aborted) {
//            ss << ", time out!";
            ss << ", " << get_timeout_reason();
        }

        channel_impl_.get_it_ep()++;
        if (channel_impl_.get_it_ep() != tcp::resolver::iterator()) {
            e_log << "Rpc call: " << controller_.impl_->get_call_tag() << ", connect failed," << ss.str();
            connect();
        } else {
//            total_deadline_.cancel();
            done(-3, ss.str());
        }
    }
}
void gprpc::client::cli_conn::handle_resolve(const error_code &e, tcp::resolver::iterator ep_it) {
    if (!e) {
        d_log << "Rpc call: " << controller_.impl_->get_call_tag() << ", resolving done!";
        //todo:lock? Nah, won't cause serious problem, just leave it be
//        it_ep_ = ep_it;
        channel_impl_.set_it_ep(ep_it);
        connect();
    } else {
        std::stringstream ss;
        ss << "address: " << channel_impl_.get_address() << ", port: " << channel_impl_.get_port() << e.message();
        if (e == boost::asio::error::operation_aborted) {
//            ss << ", time out!";
            ss << ", " << get_timeout_reason();
        }
        done(-2, ss.str());
    }
}

void gprpc::client::cli_conn::connect() {
    if (channel_impl_.get_it_ep() != tcp::resolver::iterator()) {
        uint32_t connect_timeout = controller_.get_connect_timeout();
        if (connect_timeout > 0) {
            deadline_.expires_from_now(boost::posix_time::milliseconds(connect_timeout));
            deadline_.async_wait(boost::bind(&gprpc::client::cli_conn::handle_conn_timeout, shared_from_this(),
                                             boost::asio::placeholders::error));
        } else {
            deadline_.cancel();
        }

        d_log << "Rpc call: " << controller_.impl_->get_call_tag() << ", start connecting...";
        boost::asio::async_connect(socket_, channel_impl_.get_it_ep(),
                                   bind(&gprpc::client::cli_conn::handle_connect, shared_from_this(), boost::asio::placeholders::error));
    } else {
        e_log << "Rpc call: " << controller_.impl_->get_call_tag() << ", no resolved endpoints available!";
    }
}

void gprpc::client::cli_conn::resolve() {
    uint32_t resolve_timeout = controller_.get_resolve_timeout();
    if (resolve_timeout > 0) {
        deadline_.expires_from_now(boost::posix_time::milliseconds(resolve_timeout));
        deadline_.async_wait(boost::bind(&gprpc::client::cli_conn::handle_resolve_timeout, shared_from_this(),
                                         boost::asio::placeholders::error));
    } else {
        deadline_.cancel();
    }

    d_log << "Rpc call: " << controller_.impl_->get_call_tag() << ", start resolving...";
    tcp::resolver::query query(channel_impl_.get_address(), channel_impl_.get_port());
    channel_impl_.get_resolver().async_resolve(query, bind(&gprpc::client::cli_conn::handle_resolve, shared_from_this(),
                                        boost::asio::placeholders::error, boost::asio::placeholders::iterator));
}

void gprpc::client::cli_conn::rcv_rsp_header() {

    uint32_t read_timeout = controller_.get_read_timeout();
    if (read_timeout > 0) {
        deadline_.expires_from_now(boost::posix_time::milliseconds(read_timeout));
        deadline_.async_wait(boost::bind(&gprpc::client::cli_conn::handle_sock_timeout, shared_from_this(),
                                         boost::asio::placeholders::error, RECEIVE_TIMEOUT));
    } else {
        deadline_.cancel();
    }

    d_log << "Rpc call: " << controller_.impl_->get_call_tag() << ", start receiving response header....";
    async_read(socket_, buffer(buffer_), transfer_exactly(sizeof(rpc_header)),
               bind(&gprpc::client::cli_conn::handle_rsp_header, shared_from_this(),
                    boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void gprpc::client::cli_conn::handle_rsp_header(const error_code &e, size_t bytes_trans) {
    if (!e) {
        if (bytes_trans != sizeof(rpc_header)) {
            done(-5);
            return;
        }
        memcpy(&header_, buffer_.data(), bytes_trans);
        if (memcmp(header_.magic, rpc_magic, 4) != 0) {
            done(-6, "rpc_magic error");
            return;
        }
        if (header_.data_len > conn_buf_size_) {
            done(-7);
            return;
        }
        d_log << "Rpc call: " << controller_.impl_->get_call_tag() << ", response header received!";
        rcv_rsp_data();
    } else {
        std::stringstream ss;
        ss << e.message();
        if (e == boost::asio::error::operation_aborted) {
//            ss << ", time out!";
            ss << ", " << get_timeout_reason();
        }
        done(-5, ss.str());
    }
}

void gprpc::client::cli_conn::rcv_rsp_data() {
    //inherit the previous read deadline timer, if any, from rcv_rsp_header()
    d_log << "Rpc call: " << controller_.impl_->get_call_tag() << ", start receiving response data....";
    async_read(socket_, buffer(buffer_), transfer_exactly(header_.data_len),
               bind(&gprpc::client::cli_conn::handle_rsp_data, shared_from_this(),
                    boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void gprpc::client::cli_conn::handle_rsp_data(const error_code &e, size_t bytes_trans) {
    if (!e) {
        if (bytes_trans != header_.data_len) {
            done(-8, "size of the received data is wrong");
            return;
        }
        d_log << "Rpc call: " << controller_.impl_->get_call_tag() << ", response data received!";
//        wios_->post(boost::bind(&gprpc::client::rpc_channel_impl::handle_rpc_response, channel_.impl_, shared_from_this()));
        done(0);
    } else {
        std::stringstream ss;
        ss << e.message();
        if (e == boost::asio::error::operation_aborted) {
//            ss << ", time out!";
            ss << ", " << get_timeout_reason();
        }
        done(-8, e.message());
    }
}

Closure &gprpc::client::cli_conn::get_done() {
    return done_;
}

gprpc::rpc_controller &gprpc::client::cli_conn::get_controller() {
    return controller_;
}

Message* gprpc::client::cli_conn::get_response() {
    return response_;
}

const char *gprpc::client::cli_conn::get_buffer() {
    return buffer_.data();
}

const gprpc::rpc_header &gprpc::client::cli_conn::get_header() const {
    return header_;
}

bool gprpc::client::cli_conn::is_sync_call() {
    return sync_call_;
}

namespace gprpc {
    namespace client {
        const std::string total_to = "Total time out reached!";
        const std::string resolve_to = "Resolving time out reached!";
        const std::string conn_to = "Connecting time out reached!";
        const std::string recv_to = "Receiving time out reached!";
        const std::string send_to = "Sending time out reached!";
        const std::string no_to = "No time out";
    }
}

const std::string& gprpc::client::cli_conn::get_timeout_reason() {
    using namespace gprpc::client;
    switch (tr_) {
        case TOTAL_TIMEOUT:
            return total_to;
        case RESOLVE_TIMEOUT:
            return resolve_to;
        case CONNECT_TIMEOUT:
            return conn_to;
        case RECEIVE_TIMEOUT:
            return recv_to;
        case SEND_TIMEOUT:
            return send_to;
        case NO_TIMEOUT:
            return no_to;
        default:
            return no_to;
    }
}
