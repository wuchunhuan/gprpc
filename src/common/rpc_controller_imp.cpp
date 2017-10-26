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
#include "rpc_controller_imp.h"

//todo: support user defined error info
const std::map<int32_t, std::string> gprpc::rpc_controller_impl::err_info_ = gprpc::rpc_controller_impl::init_err_info();

std::map<int32_t, std::string> gprpc::rpc_controller_impl::init_err_info() {
    std::map<int32_t, std::string> err_info;
    err_info.insert(std::make_pair(0, "Rpc call success!"));
    err_info.insert(std::make_pair(-1, "Request message too large!"));
    err_info.insert(std::make_pair(-2, "Resolving remote address failed!"));
    err_info.insert(std::make_pair(-3, "Connecting remote address failed!"));
    err_info.insert(std::make_pair(-4, "Send request message failed!"));
    err_info.insert(std::make_pair(-5, "Receive response header failed!"));
    err_info.insert(std::make_pair(-6, "Not a rpc response!"));
    err_info.insert(std::make_pair(-7, "Response message too large!"));
    err_info.insert(std::make_pair(-8, "Receive response message failed!"));
    err_info.insert(std::make_pair(-9, "Corrupted rpc data received!"));
    err_info.insert(std::make_pair(-10, "Corrupted rpc response message received!"));
    err_info.insert(std::make_pair(-11, "Error detail: "));
    return err_info;
};

gprpc::rpc_controller_impl::rpc_controller_impl() {
    reset();
}

void gprpc::rpc_controller_impl::reset() {
    process_end_ = NULL_END;
    msg_type_ = NULL_TYPE;
    failed_ = false;
    failed_reason_ = "";
    failed_detail_ = "";
    err_code_ = 0;
    call_id_ = (uint32_t)-1;
    compress_ = false;
    encrypt_ = false;
    resolve_timeout_ = 0;
    connect_timeout_ = 0;
    read_timeout_ = 0;
    write_timeout_ = 0;
    total_timeout_ = 0;
}

bool gprpc::rpc_controller_impl::failed() const {
    return failed_;
}

std::string gprpc::rpc_controller_impl::error_text() const {
    return failed_reason_;
}

std::string gprpc::rpc_controller_impl::error_text_detail() const {
    return failed_detail_;
}

void gprpc::rpc_controller_impl::start_cancel() {
    return;
}

void gprpc::rpc_controller_impl::set_failed(const std::string &reason) {
    failed_ = true;
    failed_reason_ = reason;
}

void gprpc::rpc_controller_impl::set_error_detail(const std::string &detail) {
    failed_detail_ = detail;
}

int32_t gprpc::rpc_controller_impl::error_code() const {
    return err_code_;
}

void gprpc::rpc_controller_impl::set_error_code(int32_t err_code) {
    err_code_ = err_code;
}

void gprpc::rpc_controller_impl::set_failed(int32_t err_code) {
    set_failed(err_info_.find(err_code)->second);
    set_error_code(err_code);
}

bool gprpc::rpc_controller_impl::is_canceled() const {
    return false;
}

uint32_t gprpc::rpc_controller_impl::get_call_id() const {
    return call_id_;
}

void gprpc::rpc_controller_impl::set_call_id(uint32_t call_id) {
    call_id_ = call_id;
}

const std::string &gprpc::rpc_controller_impl::get_call_tag() const {
    return call_tag_;
}

void gprpc::rpc_controller_impl::set_call_tag(const std::string &call_tag) {
    call_tag_ = call_tag;
}

bool gprpc::rpc_controller_impl::is_compress() const {
    return compress_;
}

void gprpc::rpc_controller_impl::set_compress(bool compress) {
    compress_ = compress;
}

bool gprpc::rpc_controller_impl::is_encrypt() const {
    return encrypt_;
}

void gprpc::rpc_controller_impl::set_encrypt(bool encrypt) {
    encrypt_ = encrypt;
}

void gprpc::rpc_controller_impl::process_data(const std::string &in, std::string &out) {
    std::string intermediate;
    if ((process_end_ == RPC_CLIENT && msg_type_ == REQUEST)
        || (process_end_ == RPC_SERVER && msg_type_ == RESPONSE)) {
        if (is_compress()) {
            compress(in, intermediate);

            if (is_encrypt()) {
                encrypt(intermediate, out);
            } else {
                out = intermediate;
            }

        } else {
            if (is_encrypt()) {
                encrypt(in, out);
            } else {
                out = in;
            }
        }
    } else if ((process_end_ == RPC_CLIENT && msg_type_ == RESPONSE)
               || (process_end_ == RPC_SERVER && msg_type_ == REQUEST)){
        if (is_encrypt()) {
            decrypt(in, intermediate);

            if (is_compress()) {
                decompress(intermediate , out);
            } else {
                out = intermediate;
            }
        } else {
            if (is_compress()) {
                decompress(in, out);
            } else {
                out = in;
            }
        }
    }
}

void gprpc::rpc_controller_impl::compress(const std::string &in, std::string &out) {
    out = in;
}

void gprpc::rpc_controller_impl::decompress(const std::string &in, std::string &out) {
    out = in;
}

void gprpc::rpc_controller_impl::encrypt(const std::string &in, std::string &out) {
    out = in;
}

void gprpc::rpc_controller_impl::decrypt(const std::string &in, std::string &out) {
    out = in;
}

void gprpc::rpc_controller_impl::set_process_end(gprpc::rpc_controller_impl::process_end end) {
    process_end_ = end;
}

void gprpc::rpc_controller_impl::set_msg_type(gprpc::rpc_controller_impl::msg_type type) {
    msg_type_ = type;
}

std::string gprpc::rpc_controller_impl::get_remote_address() const {
    return remote_address_;
}

void gprpc::rpc_controller_impl::set_remote_address(const std::string address) {
    remote_address_ = address;
}

void gprpc::rpc_controller_impl::set_resolve_timeout(uint32_t ms) {
    resolve_timeout_ = ms;
    if (ms != 0) {
        total_timeout_ = 0;
    }
}

void gprpc::rpc_controller_impl::set_connect_timeout(uint32_t ms) {
    connect_timeout_ = ms;
    if (ms != 0) {
        total_timeout_ = 0;
    }
}

void gprpc::rpc_controller_impl::set_read_timeout(uint32_t ms) {
    read_timeout_ = ms;
    if (ms != 0) {
        total_timeout_ = 0;
    }
}

void gprpc::rpc_controller_impl::set_write_timeout(uint32_t ms) {
    write_timeout_ = ms;
    if (ms != 0) {
        total_timeout_ = 0;
    }
}

void gprpc::rpc_controller_impl::set_total_timeout(uint32_t ms) {
    total_timeout_ = ms;
    if (ms != 0) {
        resolve_timeout_ = 0;
        connect_timeout_ = 0;
        read_timeout_ = 0;
        write_timeout_ = 0;
    }
}

uint32_t gprpc::rpc_controller_impl::get_resolve_timeout() {
    return resolve_timeout_;
}

uint32_t gprpc::rpc_controller_impl::get_connect_timeout() {
    return connect_timeout_;
}

uint32_t gprpc::rpc_controller_impl::get_read_timeout() {
    return read_timeout_;
}

uint32_t gprpc::rpc_controller_impl::get_write_timeout() {
    return write_timeout_;
}

uint32_t gprpc::rpc_controller_impl::get_total_timeout() {
    return total_timeout_;
}

