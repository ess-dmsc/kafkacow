cmake_minimum_required(VERSION 3.10)
project(kafkaCow)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${PROJECT_SOURCE_DIR}/cmake")

if(NOT CONAN_DISABLE)
    include(${CMAKE_MODULE_PATH}/conan.cmake)
    conan_cmake_run(CONANFILE conanfile.txt
            PROFILE default
            BASIC_SETUP CMAKE_TARGETS
            BUILD_TYPE "None"
            BUILD outdated)
endif()

add_executable(kafkaCow main.cpp ConsumerPrepare.cpp ConsumerPrepare.h RequestHandler.cpp RequestHandler.h ConnectKafka.cpp ConnectKafka.h)
target_link_libraries(kafkaCow CONAN_PKG::cli11 CONAN_PKG::librdkafka)