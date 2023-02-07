cmake_minimum_required(VERSION 3.22)

set(PROJECT_NAME Sip-server)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 23)
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)

file(GLOB_RECURSE SIP_SRC ${SRC_DIR}/*.cpp ${SRC_DIR}/*.h ${SRC_DIR}/*.hpp)

include_directories(${SRC_DIR})
add_executable(${PROJECT_NAME} main.cpp ${SIP_SRC})
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME sip-server)

if (UNIX)
    target_link_libraries(${PROJECT_NAME} -pthread)
else ()
    message(FATAL_ERROR "Неподдерживаемая платформа.")
endif ()
