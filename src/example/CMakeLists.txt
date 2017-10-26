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
SET(PROTO_DIR ${CMAKE_CURRENT_SOURCE_DIR})
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



include_directories(/usr/local/include
        /usr/local/ssl/include
        ./
        )


link_directories(/usr/local/lib
        /usr/local/ssl/lib)

add_compile_options(-g -Wall -fpermissive -std=c++11)


add_executable(async_client async_client.cpp ${PROTO_GEN})
add_executable(sync_client sync_client.cpp ${PROTO_GEN})
add_executable(ping_server server.cpp ${PROTO_GEN})


target_link_libraries(async_client
#        libgprpccli.a
        libgprpccli_dbg.a
        libprotobuf.a
        libboost_filesystem.a
        libboost_system.a
        libboost_thread.a
        pthread)

target_link_libraries(sync_client
#        libgprpccli.a
        libgprpccli_dbg.a
        libprotobuf.a
        libboost_filesystem.a
        libboost_system.a
        libboost_thread.a
        pthread)

target_link_libraries(ping_server
#        libgprpcsrv.a
        libgprpcsrv_dbg.a
        libprotobuf.a
        libboost_filesystem.a
        libboost_system.a
        libboost_thread.a
        pthread)
