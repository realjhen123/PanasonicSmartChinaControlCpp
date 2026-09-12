cmake_minimum_required(VERSION 3.14)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(PanasonicSmartChinaControl VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
    set(VCPKG_TARGET_TRIPLET "x64-mingw-static" CACHE STRING "")
endif()

add_library(jsoncpp_lib STATIC
    third_party/jsoncpp/jsoncpp.cpp)

target_include_directories(jsoncpp_lib PUBLIC third_party/jsoncpp)

find_package(ZLIB REQUIRED)
find_package(CURL QUIET)
find_package(OpenSSL QUIET)

add_executable(PanasonicSmartChinaControl src/pscc.cpp)
target_include_directories(PanasonicSmartChinaControl PUBLIC third_party/)
target_link_libraries(PanasonicSmartChinaControl PRIVATE CURL::libcurl ZLIB::ZLIB OpenSSL::SSL OpenSSL::Crypto jsoncpp_lib)