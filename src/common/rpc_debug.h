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
#ifndef MM_CACHE_CLUSTER_RPC_DEBUG_H
#define MM_CACHE_CLUSTER_RPC_DEBUG_H
#include <iostream>
#include <string>
using namespace std;

#define d_log rpc_log("DEBUG")
#define i_log rpc_log("INFO")
#define w_log rpc_log("WARN")
#define e_log rpc_log("ERROR")
#define f_log rpc_log("FATAL")

class rpc_log{
public:
    rpc_log(const std::string& level);
    ~rpc_log();
    rpc_log& operator<<(bool val);
    rpc_log& operator<<(short val);
    rpc_log& operator<<(unsigned short val);
    rpc_log& operator<<(int val);
    rpc_log& operator<<(unsigned int val);
    rpc_log& operator<<(long val);
    rpc_log& operator<<(unsigned long val);
    rpc_log& operator<<(float val);
    rpc_log& operator<<(double val);
    rpc_log& operator<<(long double val);
    rpc_log& operator<<(char val);
    rpc_log& operator<<(signed char val);
    rpc_log& operator<<(unsigned char val);
    rpc_log& operator<<(const char* val);
    rpc_log& operator<<(const signed char* val);
    rpc_log& operator<<(const unsigned char* val);
    rpc_log& operator<<(void* val);
    rpc_log& operator<<(std::streambuf* val);
    rpc_log& operator<<(const std::string& val);

private:
    std::string get_prefix(std::string level);
};

#endif //MM_CACHE_CLUSTER_RPC_DEBUG_H
