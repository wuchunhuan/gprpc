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
#include <sstream>
#include "rpc_debug.h"


std::string rpc_log::get_prefix(std::string level) {
    char string1[128] = {0};
    time_t Today;
    tm* Time;
    time(&Today);
    Time = localtime(&Today);

    strftime(string1, 128, "%Y-%m-%d %H:%M:%S",Time);
    std::stringstream prefix;
    prefix << string1 << " [" << level << "]: ";
    return prefix.str();
}

rpc_log::rpc_log(const std::string &level) {
#ifdef _RPC_DEBUG_
    clog << get_prefix(level);
#endif
}

rpc_log::~rpc_log() {
#ifdef _RPC_DEBUG_
    clog << std::endl;
#endif
}

rpc_log &rpc_log::operator<<(bool val) {
#ifdef _RPC_DEBUG_
    clog << val;
#endif
    return *this;
}

rpc_log &rpc_log::operator<<(short val) {
#ifdef _RPC_DEBUG_
    clog << val;
#endif
    return *this;
}

rpc_log &rpc_log::operator<<(unsigned short val) {
#ifdef _RPC_DEBUG_
    clog << val;
#endif
    return *this;
}

rpc_log &rpc_log::operator<<(int val) {
#ifdef _RPC_DEBUG_
    clog << val;
#endif
    return *this;
}

rpc_log &rpc_log::operator<<(unsigned int val) {
#ifdef _RPC_DEBUG_
    clog << val;
#endif
    return *this;
}

rpc_log &rpc_log::operator<<(long val) {
#ifdef _RPC_DEBUG_
    clog << val;
#endif
    return *this;
}

rpc_log &rpc_log::operator<<(unsigned long val) {
#ifdef _RPC_DEBUG_
    clog << val;
#endif
    return *this;
}

rpc_log &rpc_log::operator<<(float val) {
#ifdef _RPC_DEBUG_
    clog << val;
#endif
    return *this;
}

rpc_log &rpc_log::operator<<(double val) {
#ifdef _RPC_DEBUG_
    clog << val;
#endif
    return *this;
}

rpc_log &rpc_log::operator<<(long double val) {
#ifdef _RPC_DEBUG_
    clog << val;
#endif
    return *this;
}

rpc_log &rpc_log::operator<<(char val) {
#ifdef _RPC_DEBUG_
    clog << val;
#endif
    return *this;
}

rpc_log &rpc_log::operator<<(signed char val) {
#ifdef _RPC_DEBUG_
    clog << val;
#endif
    return *this;
}

rpc_log &rpc_log::operator<<(unsigned char val) {
#ifdef _RPC_DEBUG_
    clog << val;
#endif
    return *this;
}

rpc_log &rpc_log::operator<<(const char* val) {
#ifdef _RPC_DEBUG_
    clog << val;
#endif
    return *this;
}

rpc_log &rpc_log::operator<<(const signed char* val) {
#ifdef _RPC_DEBUG_
    clog << val;
#endif
    return *this;
}

rpc_log &rpc_log::operator<<(const unsigned char* val) {
#ifdef _RPC_DEBUG_
    clog << val;
#endif
    return *this;
}

rpc_log &rpc_log::operator<<(void* val) {
#ifdef _RPC_DEBUG_
    clog << val;
#endif
    return *this;
}

rpc_log &rpc_log::operator<<(std::streambuf* val) {
#ifdef _RPC_DEBUG_
    clog << val;
#endif
    return *this;
}

rpc_log &rpc_log::operator<<(const std::string& val) {
#ifdef _RPC_DEBUG_
    clog << val;
#endif
    return *this;
}
