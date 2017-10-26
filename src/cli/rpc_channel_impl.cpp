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
#include "rpc_channel_impl.h"
#include <gprpc/rpc_channel.h>
#include <gprpc/rpc_client.h>
#include <gprpc.pb.h>
#include <gprpc/rpc_controller.h>
#include <rpc_controller_imp.h>
#include <cli_conn.h>
#include <rpc_debug.h>
#include <gprpc/closure.h>

gprpc::client::rpc_channel_impl::rpc_channel_impl(gprpc::client::rpc_client &client, const std::string &address,
                                                const std::string &port)
    : address_(address),
      port_(port),
      client_(client),
      resolver_(*client.get_nios()) {

}

const string &gprpc::client::rpc_channel_impl::get_address() const {
    return address_;
}

const string &gprpc::client::rpc_channel_impl::get_port() const {
    return port_;
}

gprpc::client::rpc_client &gprpc::client::rpc_channel_impl::get_client() const {
    return client_;
}

tcp::resolver &gprpc::client::rpc_channel_impl::get_resolver() {
    return resolver_;
}

tcp::resolver::iterator &gprpc::client::rpc_channel_impl::get_it_ep() {
    return it_ep_;
}

void gprpc::client::rpc_channel_impl::set_it_ep(tcp::resolver::iterator &it_ep) {
    it_ep_ = it_ep;
}

bool gprpc::client::rpc_channel_impl::set_rpc_data(gprpc::rpc_data &data, const MethodDescriptor *method,
                                                 gprpc::rpc_controller *controller, const Message *request) {
    data.set_data_type(gprpc::rpc_data::REQUEST);
    data.set_svc_name(method->service()->full_name());
//    data.set_method_name(method->full_name());
    data.set_method_name(method->name());
    data.set_call_id(client_.generate_seqid());
    controller->impl_->set_call_id(data.call_id());
    std::stringstream call_tag;
    call_tag << data.svc_name() << "." << data.method_name() << "." << data.call_id();
    controller->impl_->set_call_tag(call_tag.str());
    data.set_compress(controller->isCompress());
    data.set_encrypt(controller->isEncrypt());
    std::string pre_content, post_content;
    request->SerializeToString(&pre_content);
    controller->impl_->set_msg_type(gprpc::rpc_controller_impl::REQUEST);
    controller->impl_->set_process_end(gprpc::rpc_controller_impl::RPC_CLIENT);
    controller->impl_->process_data(pre_content, post_content);
    data.set_content(post_content);
    return true;
}

void gprpc::client::rpc_channel_impl::do_rpc(gprpc::rpc_data &data, gprpc::rpc_controller *controller,
                                           Message *response, Closure *done) {
    std::string rpc_data;
    data.SerializeToString(&rpc_data);
    size_t header_size = sizeof(gprpc::rpc_header);
    size_t data_size = rpc_data.size() + header_size;

    if (data_size > conn_buf_size_) {
        abort(controller, done, -1);
        return;
    }

    if (done) {
//        conn_.reset(new cli_conn(*this, *done, *controller, response));
        cli_conn_ptr conn(new cli_conn(*this, *done, *controller, response));
        gprpc::rpc_header& header = conn->get_header();
        memcpy(header.magic, rpc_magic, RPC_MAGIC_LEN);
        header.data_len = (uint32_t)rpc_data.size();
        conn->set_buffer(rpc_data);
        conn->start();
        controller->impl_->set_remote_address(get_address());
    } else {
//        i_log << "rpc call: " << svc_name << "." << method_name << ", conn: " << conn->get_tag();
        single_sync_wait syncer;
        Closure *signaller = new_callback(&syncer, &single_sync_wait::signal_ex);
//        conn_.reset(new cli_conn(*this, *signaller, *controller, NULL));
        cli_conn_ptr conn(new cli_conn(*this, *signaller, *controller, NULL));
        gprpc::rpc_header& header = conn->get_header();
        memcpy(header.magic, rpc_magic, RPC_MAGIC_LEN);
        header.data_len = (uint32_t)rpc_data.size();
        conn->set_buffer(rpc_data);
        conn->start();
        syncer.wait();

        controller->impl_->set_remote_address(get_address());
        handle_rpc_response(controller, conn->get_buffer(), conn->get_header().data_len, response);

    }
}

void gprpc::client::rpc_channel_impl::abort(gprpc::rpc_controller *controller, Closure *done,
                                          int32_t err_code, string extra_err_info) {
    controller->impl_->set_failed(err_code);
    if (!extra_err_info.empty()) {
        controller->impl_->set_failed(controller->impl_->error_text() + extra_err_info);
    }
    if (err_code == 0) {
        i_log << "Rpc call: "
              << controller->impl_->get_call_tag()
              << " done successfully: "
              << controller->ErrorCode()
              << ", "
              << controller->ErrorText()
              << extra_err_info;
    } else {
        e_log << "Rpc call: "
              << controller->impl_->get_call_tag()
              << " failed, error code: "
              << controller->ErrorCode()
              << ", "
              << controller->ErrorText()
              << extra_err_info;
    }
    if (done) {
        client_.get_wios()->post(boost::bind(&Closure::Run, done));
    }
}

void gprpc::client::rpc_channel_impl::handle_rpc_response(gprpc::client::cli_conn_ptr conn) {
    gprpc::rpc_controller& controller = conn->get_controller();

    if (!controller.Failed()) {
        gprpc::rpc_data data;
        if (!data.ParseFromArray(conn->get_buffer(), conn->get_header().data_len)) {
            controller.impl_->set_failed(-9);
//            if (!conn->is_sync_call()) {
                conn->get_done().Run();
//            }
            return;
        }
        controller.impl_->set_msg_type(gprpc::rpc_controller_impl::RESPONSE);
        controller.impl_->set_process_end(gprpc::rpc_controller_impl::RPC_CLIENT);
        if (data.failed()) {
            controller.SetFailed(data.reason());
            controller.SetErrorCode(data.error_code());
        }
        controller.setEncrypt(data.encrypt());
        controller.setCompress(data.compress());
        //socket might have been closed by now due to timeout
        /*tcp::endpoint remote_ep = conn->socket().remote_endpoint();
        controller.impl_->set_remote_address(remote_ep.address().to_string());*/
        std::string out;
        controller.impl_->process_data(data.content(), out);
        if (!conn->get_response()->ParseFromString(out)) {
            //override the err info in rpc data
            controller.impl_->set_failed(-10);
        }
    }

//    if (!conn->is_sync_call()) {
        conn->get_done().Run();
//    }
}

void gprpc::client::rpc_channel_impl::handle_rpc_response(gprpc::rpc_controller* controller,
                                                        const void* buffer,
                                                        int size,
                                                        Message *response) {
    if (!controller->Failed()) {
        gprpc::rpc_data data;
        if (!data.ParseFromArray(buffer, size)) {
            controller->impl_->set_failed(-9);
            return;
        }
        controller->impl_->set_msg_type(gprpc::rpc_controller_impl::RESPONSE);
        controller->impl_->set_process_end(gprpc::rpc_controller_impl::RPC_CLIENT);
        if (data.failed()) {
            controller->SetFailed(data.reason());
            controller->SetErrorCode(data.error_code());
        }
        controller->setEncrypt(data.encrypt());
        controller->setCompress(data.compress());
        //socket might have been closed by now due to timeout
        /*tcp::endpoint remote_ep = conn->socket().remote_endpoint();
        controller.impl_->set_remote_address(remote_ep.address().to_string());*/
        std::string out;
        controller->impl_->process_data(data.content(), out);
        if (!response->ParseFromString(out)) {
            //override the err info in rpc data
            controller->impl_->set_failed(-10);
        }
    }
}

void gprpc::client::rpc_channel_impl::call_method(const MethodDescriptor *method, RpcController *controller,
                                                const Message *request, Message *response, Closure *done) {
    gprpc::rpc_data data;
    gprpc::rpc_controller* ctrl = dynamic_cast<gprpc::rpc_controller*>(controller);
    set_rpc_data(data, method, ctrl, request);
    do_rpc(data, ctrl, response, done);
}
