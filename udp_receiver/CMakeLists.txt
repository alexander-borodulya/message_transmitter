cmake_minimum_required(VERSION 2.8)

project(udp_receiver)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
SET(CMAKE_CXX_FLAGS -pthread)

set(SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/bst_api.h
    ${CMAKE_CURRENT_SOURCE_DIR}/bst_impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/bst_impl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/client.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/client.h
    ${CMAKE_CURRENT_SOURCE_DIR}/container.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/container.h
    ${CMAKE_CURRENT_SOURCE_DIR}/test_container_functional.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_container_functional.h
    ${CMAKE_CURRENT_SOURCE_DIR}/test_threads_model.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_threads_model.h
    ${CMAKE_SOURCE_DIR}/common/common.cpp
    ${CMAKE_SOURCE_DIR}/common/common.h
    ${CMAKE_SOURCE_DIR}/common/log.cpp
    ${CMAKE_SOURCE_DIR}/common/log.h
    ${CMAKE_SOURCE_DIR}/common/message_format.h
    ${CMAKE_SOURCE_DIR}/common/tcp_server.cpp
    ${CMAKE_SOURCE_DIR}/common/tcp_server.h
    ${CMAKE_SOURCE_DIR}/common/tcp_socket.cpp
    ${CMAKE_SOURCE_DIR}/common/tcp_socket.h
    ${CMAKE_SOURCE_DIR}/common/udp_socket.cpp
    ${CMAKE_SOURCE_DIR}/common/udp_socket.h
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR})
