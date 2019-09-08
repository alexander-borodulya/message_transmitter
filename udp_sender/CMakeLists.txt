cmake_minimum_required(VERSION 2.8)

project(udp_sender)
set(SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/server.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/server.h
    ${CMAKE_SOURCE_DIR}/common/common.cpp
    ${CMAKE_SOURCE_DIR}/common/common.h
    ${CMAKE_SOURCE_DIR}/common/log.cpp
    ${CMAKE_SOURCE_DIR}/common/log.h
    ${CMAKE_SOURCE_DIR}/common/message_format.h
    ${CMAKE_SOURCE_DIR}/common/udp_socket.cpp
    ${CMAKE_SOURCE_DIR}/common/udp_socket.h
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR})
