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
#ifndef MM_CACHE_CLUSTER_CLOSURE_H
#define MM_CACHE_CLUSTER_CLOSURE_H

#include <gprpc/common.h>
#include <gprpc/rpc_controller.h>
//#include <rpc.pb.h>

template <typename Class>
class method_closure0 : public Closure {
public:
    typedef void (Class::*MethodType)();

    method_closure0(Class* object, MethodType method, bool self_deleting)
        : object_(object), method_(method), self_deleting_(self_deleting) {}
    ~method_closure0() {}

    void Run() {
        bool needs_delete = self_deleting_;
        (object_->*method_)();
        if (needs_delete) delete this;
    }

private:
    Class* object_;
    MethodType method_;
    bool self_deleting_;
};

template <typename Class>
inline Closure* new_callback(Class* object, void (Class::*method)()) {
    return new method_closure0<Class>(object, method, true);
}

template <typename Class, typename Arg1, typename Arg2, typename Arg3>
class method_closure3 : public Closure {
public:
    typedef void (Class::*MethodType)(Arg1 arg1, Arg2 arg2, Arg3 arg3);

    method_closure3(Class* object, MethodType method, bool self_deleting,
                    Arg1 arg1, Arg2 arg2, Arg3 arg3)
        : object_(object), method_(method), self_deleting_(self_deleting),
          arg1_(arg1), arg2_(arg2), arg3_(arg3) {}
    ~method_closure3() {}

    void Run() {
        bool needs_delete = self_deleting_;
        (object_->*method_)(arg1_, arg2_, arg3_);
        if (needs_delete) delete this;
    }

private:
    Class* object_;
    MethodType method_;
    bool self_deleting_;
    Arg1 arg1_;
    Arg2 arg2_;
    Arg3 arg3_;
};

template <typename Class, typename Arg1, typename Arg2, typename Arg3>
inline Closure* new_callback(Class* object, void (Class::*method)(Arg1, Arg2, Arg3),
                             Arg1 arg1, Arg2 arg2, Arg3 arg3) {
    return new method_closure3<Class, Arg1, Arg2, Arg3>(
        object, method, true, arg1, arg2, arg3);
}

#define CLI_RPC_TYPE_CHECK(ARG1, ARG2, ARG3) \
    dynamic_cast<rpc::rpc_controller*>(ARG1); \
    dynamic_cast<::google::protobuf::Message*>(ARG2); \
    dynamic_cast<::google::protobuf::Message*>(ARG3)

template <typename Arg1, typename Arg2, typename Arg3>
bool cli_rpc_type_check(Arg1 arg1, Arg2 arg2, Arg3 arg3) {
    return !((dynamic_cast<gprpc::rpc_controller *>(arg1) == NULL)
             || (dynamic_cast<::google::protobuf::Message *>(arg2) == NULL)
             || (dynamic_cast<::google::protobuf::Message *>(arg3) == NULL));
}

template <typename Class, typename Arg1, typename Arg2, typename Arg3>
class cli_rpc_method_closure3 : public Closure {
public:
    typedef void (Class::*MethodType)(Arg1 arg1, Arg2 arg2, Arg3 arg3);

    cli_rpc_method_closure3(Class* object, MethodType method, bool self_deleting,
                            Arg1 arg1, Arg2 arg2, Arg3 arg3)
        : object_(object), method_(method), self_deleting_(self_deleting),
          arg1_(arg1), arg2_(arg2), arg3_(arg3) {
//        CLI_RPC_TYPE_CHECK(arg1, arg2, arg3);
    }
    ~cli_rpc_method_closure3() {}

    void Run() {
        bool needs_delete = self_deleting_;
        (object_->*method_)(arg1_, arg2_, arg3_);
        if (needs_delete) delete this;
    }

private:
    Class* object_;
    MethodType method_;
    bool self_deleting_;
    Arg1 arg1_;
    Arg2 arg2_;
    Arg3 arg3_;
};

template <typename Arg1, typename Arg2, typename Arg3>
class cli_rpc_function_closure3 : public Closure {
public:
    typedef void (*FunctionType)(Arg1 arg1, Arg2 arg2, Arg3 arg3);

    cli_rpc_function_closure3(FunctionType function, bool self_deleting,
                              Arg1 arg1, Arg2 arg2, Arg3 arg3)
        : function_(function), self_deleting_(self_deleting),
          arg1_(arg1), arg2_(arg2), arg3_(arg3) {
//        CLI_RPC_TYPE_CHECK(arg1, arg2, arg3);
    }
    ~cli_rpc_function_closure3() {}

    void Run() {
        bool needs_delete = self_deleting_;
        (*function_)(arg1_, arg2_, arg3_);
        if (needs_delete) delete this;
    }

private:
    FunctionType function_;
    bool self_deleting_;
    Arg1 arg1_;
    Arg2 arg2_;
    Arg3 arg3_;
};

template <typename Class, typename Arg1, typename Arg2, typename Arg3>
inline Closure* new_cli_rpc_callback(Class* object, void (Class::*method)(Arg1, Arg2, Arg3),
                                     Arg1 arg1, Arg2 arg2, Arg3 arg3) {
//    CLI_RPC_TYPE_CHECK(arg1, arg2, arg3);
    if (!cli_rpc_type_check(arg1, arg2, arg3)) {
        return NULL;
    }
    return new cli_rpc_method_closure3<Class, Arg1, Arg2, Arg3>(
        object, method, true, arg1, arg2, arg3);
}

template <typename Arg1, typename Arg2, typename Arg3>
inline Closure* new_cli_rpc_callback(void (*function)(Arg1, Arg2, Arg3),
                                     Arg1 arg1, Arg2 arg2, Arg3 arg3) {
//    CLI_RPC_TYPE_CHECK(arg1, arg2, arg3);
    if (!cli_rpc_type_check(arg1, arg2, arg3)) {
        return NULL;
    }
    return new cli_rpc_function_closure3<Arg1, Arg2, Arg3>(
        function, true, arg1, arg2, arg3);
}
#endif //MM_CACHE_CLUSTER_CLOSURE_H
