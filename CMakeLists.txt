# gprpc global cmake file
# This currently builds C++ code.
#
# Copyright © 2017 Patrick Wu(Wu chunhuan)/wuchunhuan@gmail.com.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required(VERSION 2.8)
#set(CMAKE_VERBOSE_MAKEFILE ON)

SET(PROTO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/proto)
FILE(GLOB PROTO_INPUT ${PROTO_DIR}/*.proto)
SET(PROTOC "/usr/local/bin/protoc")
SET(PROTOC_C_OUT_FLAG --cpp_out)
SET(PROTO_GEN_DIR ${PROTO_DIR})
MESSAGE(STATUS "PROTOC = ${PROTOC}")

FOREACH(PROTO_FILE ${PROTO_INPUT})
    GET_FILENAME_COMPONENT(PROTO_NAME ${PROTO_FILE} NAME_WE)
    SET(CUR_PROTO_GEN
            ${PROTO_GEN_DIR}/${PROTO_NAME}.pb.h
            ${PROTO_GEN_DIR}/${PROTO_NAME}.pb.cc
            )
    SET(PROTO_GEN
            ${PROTO_GEN}
            ${CUR_PROTO_GEN}
            )
    MESSAGE(STATUS "CUR_PROTO_GEN = ${CUR_PROTO_GEN}")

    ADD_CUSTOM_COMMAND(
            OUTPUT ${CUR_PROTO_GEN}
            COMMAND ${PROTOC} ${PROTO_FILE} ${PROTOC_C_OUT_FLAG} ${PROTO_GEN_DIR}
            -I${PROTO_DIR}
            DEPENDS ${PROTOC} ${PROTO_FILE}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endforeach(PROTO_FILE ${PROTO_INPUT})

include_directories(src/common
        src/cli
        src/srv
        src/proto
        ../common
        include)
add_compile_options(-O2 -Wall -fPIC -fpermissive -std=c++11)
file(GLOB_RECURSE cli_source_files "src/cli/*")
file(GLOB_RECURSE srv_source_files "src/srv/*")
file(GLOB_RECURSE dbg_source_files "src/common/rpc_debug.cpp")
file(GLOB_RECURSE common_rpc_files "src/common/rpc_controller*.cpp")
file(GLOB_RECURSE common_files "src/common/connection.cpp")
file(GLOB_RECURSE header_files "include/*")


add_library(comm_objs OBJECT ${PROTO_GEN} ${common_rpc_files} ${common_files})
add_library(cli_objs OBJECT ${cli_source_files})
add_library(srv_objs OBJECT ${srv_source_files})
add_dependencies(cli_objs comm_objs)
add_dependencies(srv_objs comm_objs)

add_library(gprpccli SHARED $<TARGET_OBJECTS:cli_objs> $<TARGET_OBJECTS:comm_objs> ${dbg_source_files})
add_library(gprpccli_s STATIC $<TARGET_OBJECTS:cli_objs> $<TARGET_OBJECTS:comm_objs> ${dbg_source_files})
set_target_properties(gprpccli_s PROPERTIES OUTPUT_NAME "gprpccli")
set_target_properties(gprpccli PROPERTIES CLEAN_DIRECT_OUTPUT 1)
set_target_properties(gprpccli_s PROPERTIES CLEAN_DIRECT_OUTPUT 1)
add_library(gprpcsrv SHARED $<TARGET_OBJECTS:srv_objs> $<TARGET_OBJECTS:comm_objs> ${dbg_source_files})
add_library(gprpcsrv_s STATIC $<TARGET_OBJECTS:srv_objs> $<TARGET_OBJECTS:comm_objs> ${dbg_source_files})
set_target_properties(gprpcsrv_s PROPERTIES OUTPUT_NAME "gprpcsrv")
set_target_properties(gprpcsrv PROPERTIES CLEAN_DIRECT_OUTPUT 1)
set_target_properties(gprpcsrv_s PROPERTIES CLEAN_DIRECT_OUTPUT 1)

add_library(gprpccli_dbg SHARED $<TARGET_OBJECTS:cli_objs> $<TARGET_OBJECTS:comm_objs> ${dbg_source_files})
add_library(gprpccli_dbg_s STATIC $<TARGET_OBJECTS:cli_objs> $<TARGET_OBJECTS:comm_objs> ${dbg_source_files})
set_target_properties(gprpccli_dbg_s PROPERTIES OUTPUT_NAME "gprpccli_dbg")
set_target_properties(gprpccli_dbg PROPERTIES CLEAN_DIRECT_OUTPUT 1)
set_target_properties(gprpccli_dbg_s PROPERTIES CLEAN_DIRECT_OUTPUT 1)
add_library(gprpcsrv_dbg SHARED $<TARGET_OBJECTS:srv_objs> $<TARGET_OBJECTS:comm_objs> ${dbg_source_files})
add_library(gprpcsrv_dbg_s STATIC $<TARGET_OBJECTS:srv_objs> $<TARGET_OBJECTS:comm_objs> ${dbg_source_files})
set_target_properties(gprpcsrv_dbg_s PROPERTIES OUTPUT_NAME "gprpcsrv_dbg")
set_target_properties(gprpcsrv_dbg PROPERTIES CLEAN_DIRECT_OUTPUT 1)
set_target_properties(gprpcsrv_dbg_s PROPERTIES CLEAN_DIRECT_OUTPUT 1)

#target_compile_definitions(rpccli PRIVATE BOOST_ASIO_ENABLE_HANDLER_TRACKING)
#target_compile_definitions(rpccli_s PRIVATE BOOST_ASIO_ENABLE_HANDLER_TRACKING)
#target_compile_definitions(rpcsrv PRIVATE BOOST_ASIO_ENABLE_HANDLER_TRACKING)
#target_compile_definitions(rpcsrv_s PRIVATE BOOST_ASIO_ENABLE_HANDLER_TRACKING)
target_compile_definitions(gprpccli_dbg PRIVATE _RPC_DEBUG_)
target_compile_definitions(gprpccli_dbg_s PRIVATE _RPC_DEBUG_)
target_compile_definitions(gprpcsrv_dbg PRIVATE _RPC_DEBUG_)
target_compile_definitions(gprpcsrv_dbg_s PRIVATE _RPC_DEBUG_)



set(CMAKE_INSTALL_PREFIX /usr/local)
install(TARGETS gprpccli gprpccli_s gprpcsrv gprpcsrv_s gprpccli_dbg gprpccli_dbg_s gprpcsrv_dbg gprpcsrv_dbg_s
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
install(DIRECTORY include/gprpc
        DESTINATION include/)

#add_subdirectory(src/example)
