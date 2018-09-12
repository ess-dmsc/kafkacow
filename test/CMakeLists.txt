file(GLOB SRC_FILES
        ../src/*.cpp
        ../src/*.h)

list(FILTER SRC_FILES EXCLUDE REGEX ".*main.cpp$")
list(FILTER SRC_FILES EXCLUDE REGEX ".*ConnectKafka.cpp$")

add_executable(UnitTests RequestHandlerTest.cpp ${SRC_FILES})

target_link_libraries(UnitTests
        CONAN_PKG::gtest
        CONAN_PKG::cli11
        CONAN_PKG::librdkafka
        CONAN_PKG::boost_filesystem
        CONAN_PKG::FlatBuffers
        CONAN_PKG::spdlog
        CONAN_PKG::yaml-cpp)

