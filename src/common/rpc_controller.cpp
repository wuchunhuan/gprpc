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
#include <gprpc/rpc_controller.h>
#include "rpc_controller_imp.h"


void gprpc::rpc_controller::Reset() {
    impl_->reset();
}

bool gprpc::rpc_controller::Failed() const {
    return impl_->failed();
}

string gprpc::rpc_controller::ErrorText() const {
    return impl_->error_text();
}

void gprpc::rpc_controller::StartCancel() {
    return;
}

void gprpc::rpc_controller::SetFailed(const string &reason) {
    impl_->set_failed(reason);
    return;
}

bool gprpc::rpc_controller::IsCanceled() const {
    return false;
}

void gprpc::rpc_controller::NotifyOnCancel(Closure *callback) {
    return;
}

bool gprpc::rpc_controller::isCompress() const {
    return impl_->is_compress();
}

void gprpc::rpc_controller::setCompress(bool compress) {
    impl_->set_compress(compress);
}

bool gprpc::rpc_controller::isEncrypt() const {
    return impl_->is_encrypt();
}

void gprpc::rpc_controller::setEncrypt(bool encrypt) {
    impl_->set_encrypt(encrypt);
}

//void gprpc::rpc_controller::SetFailed(int32_t err_code) {
//    impl_->set_failed(err_code);
//}

int32_t gprpc::rpc_controller::ErrorCode() const {
    return impl_->error_code();
}

void gprpc::rpc_controller::SetErrorCode(int32_t err_code) {
    impl_->set_error_code(err_code);
}

gprpc::rpc_controller::rpc_controller() : impl_(new gprpc::rpc_controller_impl()){
    Reset();
}

std::string gprpc::rpc_controller::getRemoteAddress() const {
    return impl_->get_remote_address();
}

void gprpc::rpc_controller::set_resolve_timeout(uint32_t ms) {
    impl_->set_resolve_timeout(ms);
}

void gprpc::rpc_controller::set_connect_timeout(uint32_t ms) {
    impl_->set_connect_timeout(ms);
}

void gprpc::rpc_controller::set_read_timeout(uint32_t ms) {
    impl_->set_read_timeout(ms);
}

void gprpc::rpc_controller::set_write_timeout(uint32_t ms) {
    impl_->set_write_timeout(ms);
}

void gprpc::rpc_controller::set_total_timeout(uint32_t ms) {
    impl_->set_total_timeout(ms);
}

uint32_t gprpc::rpc_controller::get_resolve_timeout() {
    return impl_->get_resolve_timeout();
}

uint32_t gprpc::rpc_controller::get_connect_timeout() {
    return impl_->get_connect_timeout();
}

uint32_t gprpc::rpc_controller::get_read_timeout() {
    return impl_->get_read_timeout();
}

uint32_t gprpc::rpc_controller::get_write_timeout() {
    return impl_->get_write_timeout();
}

uint32_t gprpc::rpc_controller::get_total_timeout() {
    return impl_->get_total_timeout();
}

string gprpc::rpc_controller::error_text_detail() const{
    return impl_->error_text_detail();
}

void gprpc::rpc_controller::set_error_detail(const string& detail) {
    impl_->set_error_detail(detail);
}