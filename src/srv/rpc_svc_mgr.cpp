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
#include <gprpc.pb.h>
#include "rpc_svc_mgr.h"
#include <gprpc/closure.h>
#include <gprpc/rpc_controller.h>
#include <rpc_controller_imp.h>
#include <srv_conn.h>

using namespace gprpc;

gprpc::server::rpc_svc_mgr::rpc_svc_mgr() {
    services_.clear();
}

bool gprpc::server::rpc_svc_mgr::register_service(service_ptr service) {
    const ServiceDescriptor* sd = service->GetDescriptor();
    std::string name = sd->full_name();
    auto it_svc = services_.find(name);
    if (it_svc != services_.end()) {
        w_log << "Service " << name << " already been registered!";
        return false;
    }

    services_[name] = service;
    return true;
}

bool gprpc::server::rpc_svc_mgr::handle_rpc_call(srv_conn_ptr conn) {

    rpc_data proto_data;
    if (!proto_data.ParseFromArray(conn->get_buffer(), conn->get_header().data_len)) {
        e_log << "rpc connection request parsing failed, " << "conn: " << conn->get_tag();
        return false;
    }

    std::string svc_name = proto_data.svc_name();
    std::string method_name = proto_data.method_name();
    auto it_svc = services_.find(proto_data.svc_name());
    if (it_svc == services_.end()) {
        e_log << "rpc call failed, " << "conn: " << conn->get_tag() << "no service named: " << svc_name << " registered!";
        return false;
    }
    service_ptr service = it_svc->second;
    const ServiceDescriptor* sd = service->GetDescriptor();
    const MethodDescriptor* md = sd->FindMethodByName(method_name);
    if (md == NULL) {
        e_log << "rpc call failed, " << "conn: " << conn->get_tag() << "no method named: " << method_name << " registered!";
        return false;
    }
    i_log << "rpc call: " << svc_name << "." << method_name << ", conn: " << conn->get_tag();

    controller_ptr controller(new rpc_controller);
    controller->impl_->set_call_id(proto_data.call_id());
    std::stringstream call_tag;
    call_tag << svc_name << "." << method_name << "." << proto_data.call_id();
    controller->impl_->set_call_tag(call_tag.str());

    controller->setCompress(proto_data.compress());
    controller->setEncrypt(proto_data.encrypt());
    tcp::endpoint remote_ep = conn->socket().remote_endpoint();
    controller->impl_->set_remote_address(remote_ep.address().to_string());
    controller->impl_->set_msg_type(gprpc::rpc_controller_impl::REQUEST);
    controller->impl_->set_process_end(gprpc::rpc_controller_impl::RPC_SERVER);
    //preprocess rpc data
    std::string out;
    controller->impl_->process_data(proto_data.content(), out);

    message_ptr req_ptr(service->GetRequestPrototype(md).New());
    message_ptr rsp_ptr(service->GetResponsePrototype(md).New());
    req_ptr->ParseFromString(out);
    Closure* done = new_callback(this,
                                 &server::rpc_svc_mgr::process_response,
                                 controller,
                                 conn,
                                 rsp_ptr);

//    conn->get_wios().dispatch(
//        boost::bind(&google::protobuf::Service::CallMethod,
//                    service, md, controller.get(), req_ptr.get(), rsp_ptr.get(), done));
    //todo:If user implemented method forget to call done->Run(), should we set a timer on it to close the connection?
    service->CallMethod(md, controller.get(), req_ptr.get(), rsp_ptr.get(), done);
    return true;
}

void server::rpc_svc_mgr::process_response(controller_ptr controller, srv_conn_ptr conn, message_ptr response) {
    rpc_data proto_data;
    std::string content;
    response->SerializeToString(&content);
    proto_data.set_data_type(rpc_data::RESPONSE);
    proto_data.set_call_id(controller->impl_->get_call_id());
    //todo:as of now, when client compress, we compress, same with encryption,
    //client specified action should be supported in the future
    proto_data.set_compress(controller->isCompress());
    proto_data.set_encrypt(controller->isEncrypt());
    controller->impl_->set_msg_type(gprpc::rpc_controller_impl::RESPONSE);
    controller->impl_->set_process_end(gprpc::rpc_controller_impl::RPC_SERVER);
    std::string out;
    controller->impl_->process_data(content, out);

    proto_data.set_failed(controller->Failed());
    proto_data.set_reason(controller->ErrorText());
    proto_data.set_error_code(controller->ErrorCode());
    proto_data.set_content(out);
    std::string rst;
    proto_data.SerializeToString(&rst);
    conn->set_buffer(rst.c_str(), rst.size());
    conn->write_rsp();
}



