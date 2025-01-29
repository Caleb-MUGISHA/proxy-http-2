cmake_minimum_required(VERSION 3.15)
project(EnvoyStyleProxy LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Dependency management using Conan
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

find_package(OpenSSL REQUIRED)
find_package(Boost 1.70 REQUIRED system thread)
find_package(nghttp2 REQUIRED)

add_library(proxy_core STATIC
  src/core/listener.cc
  src/core/connection_handler.cc
  src/core/http2_codec.cc
)

target_include_directories(proxy_core PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${Boost_INCLUDE_DIRS}
  ${NGHTTP2_INCLUDE_DIRS}
)

target_link_libraries(proxy_core
  PRIVATE Boost::system Boost::thread
  PRIVATE OpenSSL::SSL
  PRIVATE nghttp2
)

add_executable(proxy_service
  src/main.cc
  src/config/yaml_parser.cc
)

target_link_libraries(proxy_service
  PRIVATE proxy_core
  PRIVATE ${CONAN_LIBS}
)
