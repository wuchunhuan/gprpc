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
#ifndef MM_CACHE_CLUSTER_RPC_CONTROLLER_IMP_H
#define MM_CACHE_CLUSTER_RPC_CONTROLLER_IMP_H


#include <cstdint>
#include <string>
#include <map>


namespace gprpc {
    class rpc_controller_impl {
    public:
        enum process_end {
            RPC_CLIENT = 0,
            RPC_SERVER = 1,
            NULL_END = 2
        };
        enum msg_type {
            REQUEST = 0,
            RESPONSE = 1,
            NULL_TYPE = 2
        };
        rpc_controller_impl();

        void reset();

        bool failed() const;

        std::string error_text() const;

        std::string error_text_detail() const;

        void start_cancel();

        void set_failed(const std::string& reason);

        void set_error_detail(const std::string& detail);

        int32_t error_code() const;

        void set_error_code(int32_t err_code);

//        void SetFailed(int32_t err_code, const string& reason);
        void set_failed(int32_t err_code);

        bool is_canceled() const;

        uint32_t get_call_id() const;

        void set_call_id(uint32_t call_id);

        const std::string &get_call_tag() const;

        void set_call_tag(const std::string &call_tag);

        bool is_compress() const;

        void set_compress(bool compress);

        bool is_encrypt() const;

        void set_encrypt(bool encrypt);

        void process_data(const std::string& in, std::string& out);

        void set_process_end(process_end end);

        void set_msg_type(msg_type type);

        std::string get_remote_address() const;

        void set_remote_address(const std::string address);

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

    private:
        //todo: no implemented yet
        void compress(const std::string& in, std::string& out);
        void decompress(const std::string& in, std::string& out);
        void encrypt(const std::string& in, std::string& out);
        void decrypt(const std::string& in, std::string& out);
    private:
        process_end process_end_;
        msg_type msg_type_;
        bool failed_;
        int32_t err_code_;
        std::string failed_reason_;
        std::string failed_detail_;
        std::string remote_address_;

        uint32_t call_id_;
        std::string call_tag_;
        bool compress_;
        bool encrypt_;

        uint32_t resolve_timeout_;
        uint32_t connect_timeout_;
        uint32_t read_timeout_;
        uint32_t write_timeout_;
        uint32_t total_timeout_;
    public:
        static std::map<int32_t, std::string> init_err_info();
        static const std::map<int32_t, std::string> err_info_;
    };
}


#endif //MM_CACHE_CLUSTER_RPC_CONTROLLER_IMP_H
