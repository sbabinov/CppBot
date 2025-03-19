cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

project(Api_test)

find_package(OpenSSL REQUIRED)
if (OPENSSL_FOUND)
    include_directories(${OPENSSL_INCLUDE_DIRS})
    message(STATUS "OpenSSL found: ${OPENSSL_VERSION}")
else()
    message(FATAL_ERROR "OpenSSL not found!")
endif()

add_executable(main main.cpp)

target_link_libraries(main PRIVATE OpenSSL::SSL OpenSSL::Crypto)
