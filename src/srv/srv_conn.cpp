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
//#include <log.h>
#include <rpc_debug.h>
#include <srv_conn.h>
#include <rpc_svc_mgr.h>




gprpc::server::srv_conn::srv_conn(io_service& nios, io_service& wios, rpc_svc_mgr& mgr) : wios_(wios), socket_(nios), mgr_(mgr){

}

tcp::socket &gprpc::server::srv_conn::socket() {
    return socket_;
}

void gprpc::server::srv_conn::start() {
    read_req_head();
}

gprpc::server::srv_conn::~srv_conn() {
    if (end_time_ == 0) {
        end_time_ = get_time_stamp();
    }
    i_log << "rpc connection ended, " << "conn: " << tag_ << " ets: " << end_time_;
}

void gprpc::server::srv_conn::read_req_head() {
    async_read(socket_, buffer(buffer_), transfer_exactly(sizeof(rpc_header)),
               bind(&srv_conn::handle_req_head, shared_from_this(),
                    boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void gprpc::server::srv_conn::handle_req_head(const boost::system::error_code &e, size_t bytes_trans) {
    if (!e) {
        set_start_time(get_time_stamp());
        seq_++;
        set_tag(boost::lexical_cast<std::string>(start_time_) + boost::lexical_cast<std::string>(seq_));
        i_log << "rpc connection started, " << "conn: " << get_tag() << " sts: " << get_start_time();

        if (bytes_trans != sizeof(rpc_header)) {
            e_log << "rpc connection get rpc_header error, " << "conn: " << get_tag();
            return;
        }
        memcpy(&header_, buffer_.data(), bytes_trans);
        if (memcmp(header_.magic, rpc_magic, 4) != 0) {
            e_log << "rpc connection request rpc_magic error, " << "conn: " << get_tag() << "magic error!";
            return;
        }
        uint32_t data_len = ntohl(header_.data_len);
        header_.data_len = data_len;
        if (header_.data_len > conn_buf_size_) {
            e_log << "rpc connection request parsing failed, " << "conn: " << get_tag() << "rpc data too large!";
            return;
        }
        async_read(socket_, buffer(buffer_), transfer_exactly(header_.data_len),
                   bind(&srv_conn::handle_req_data, shared_from_this(),
                        boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    } else {
        if (e == boost::asio::error::eof) {
            i_log << "rpc connection closed by client, " << "conn: " << get_tag();
        } else {
            e_log << "rpc connection read request header error, " << "conn: " << get_tag();
        }
        //todo:no need to send error back
    }
}

void gprpc::server::srv_conn::handle_req_data(const boost::system::error_code &e, size_t bytes_trans) {
    if (!e) {
        if (bytes_trans != header_.data_len) {
            e_log << "rpc connection request parsing failed, " << "conn: " << get_tag();
            return;
        }

//        mgr_.handle_rpc_call(shared_from_this());
        wios_.dispatch(boost::bind(&gprpc::server::rpc_svc_mgr::handle_rpc_call, &mgr_, shared_from_this()));
    } else {
        e_log << "rpc connection read request data error, " << "conn: " << get_tag();
        //todo:no need to send error back
    }
}

const char * gprpc::server::srv_conn::get_buffer() const {
    return buffer_.data();
}

const rpc_header &gprpc::server::srv_conn::get_header() const {
    return header_;
}

void server::srv_conn::set_buffer(const char *data, uint32_t data_len) {
    if (data == NULL || data_len > conn_buf_size_) {
        return;
    }
    header_.data_len = htonl(data_len);
    size_t header_size = sizeof(header_);
    memcpy(buffer_.data(), &header_, header_size);
    memcpy(buffer_.data() + header_size, data, data_len);
    header_.data_len = data_len;
}

void server::srv_conn::write_rsp() {
    async_write(socket_, buffer(buffer_), transfer_exactly(sizeof(header_) + header_.data_len),
                bind(&server::srv_conn::handle_write, shared_from_this(), boost::asio::placeholders::error));
}

void server::srv_conn::handle_write(const boost::system::error_code& e) {
    //Server should hold connection until client close it
    /*if (!e) {
        boost::system::error_code ignore_ec;
        socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignore_ec);
        set_end_time(get_time_stamp());
        i_log << "rpc call done"  << ", conn: " << get_tag() << "elp(ms): " << get_elapsed_time();
    }*/
    if (!e) {
        set_end_time(get_time_stamp());
        i_log << "rpc call done"  << ", conn: " << get_tag() << "elp(ms): " << get_elapsed_time();
        read_req_head();
    } else {
        e_log << "rpc call failed, send rsp data error" << ", conn: " << get_tag() << "elp(ms): " << get_elapsed_time();
    }
}

io_service& server::srv_conn::get_wios() const {
    return wios_;
}

