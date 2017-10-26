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
#ifndef MM_CACHE_CLUSTER_RPC_CONTROLLER_H
#define MM_CACHE_CLUSTER_RPC_CONTROLLER_H

#include <gprpc/common.h>
//#include "rpc_controller_imp.h"

namespace gprpc {
    namespace client {
        class cli_conn;
        class rpc_channel_impl;
    }
    namespace server {
        class rpc_svc_mgr;
    }
    class rpc_controller_impl;
    typedef boost::shared_ptr<rpc_controller_impl> rpc_controller_impl_ptr;
}

namespace gprpc {
    class rpc_controller : public RpcController {
        friend class gprpc::client::rpc_channel_impl;
        friend class gprpc::server::rpc_svc_mgr;
        friend class gprpc::client::cli_conn;
    public:
        rpc_controller();

        void Reset();

        bool Failed() const;

        string ErrorText() const;

        void StartCancel();

        void SetFailed(const string& reason);

        int32_t ErrorCode() const;

        void SetErrorCode(int32_t err_code);

//        void SetFailed(int32_t err_code);

        bool IsCanceled() const;

        void NotifyOnCancel(Closure* callback);

        bool isCompress() const;

        void setCompress(bool compress);

        bool isEncrypt() const;

        void setEncrypt(bool encrypt);

        std::string getRemoteAddress() const;

        /**
         * Sets the timeout for DNS resolving.
         * If timeout is not set, DNS resolving will not return until resolving is done or some error happened, which means
         * corresponding synchronous resolving call will block or asynchronize resolving call will remain pending.
         * @param ms time out in ms milliseconds
         */
        void set_resolve_timeout(uint32_t ms);

        void set_connect_timeout(uint32_t ms);

        void set_read_timeout(uint32_t ms);

        void set_write_timeout(uint32_t ms);

        /**
         * Sets the timeout for the whole rpc call(including dns resolving, connecting,
         * reading data, writing data, intermediate processing). If timeout is not set, synchronous rpc call will block
         * until call is done or some error happened, or asynchronous rpc call will remain pending.
         * @param ms time out in ms milliseconds
         */
        void set_total_timeout(uint32_t ms);

        uint32_t get_resolve_timeout();

        uint32_t get_connect_timeout();

        uint32_t get_read_timeout();

        uint32_t get_write_timeout();

        uint32_t get_total_timeout();

        string error_text_detail() const;

        void set_error_detail(const string& detail);

    private:
        rpc_controller_impl_ptr impl_;
    };
    typedef boost::shared_ptr<rpc_controller> controller_ptr;
}

#endif //MM_CACHE_CLUSTER_RPC_CONTROLLER_H
