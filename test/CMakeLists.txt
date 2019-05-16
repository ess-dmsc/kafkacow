file(GLOB SRC_FILES
        ../src/*.cpp
        ../src/*.h)

list(FILTER SRC_FILES EXCLUDE REGEX ".*main.cpp$")
list(FILTER SRC_FILES EXCLUDE REGEX ".*Consumer.cpp$")

add_executable(UnitTests
        ${SRC_FILES}
        RequestHandlerTest.cpp
        FlatbuffersTranslatorTest.cpp
        JSONPrintingTest.cpp
        UnitTests.cpp
../src/KafkaW/FakeConsumer.cpp
../src/KafkaW/FakeConsumer.h)

target_link_libraries(UnitTests
        CONAN_PKG::gtest
        CONAN_PKG::cli11
        CONAN_PKG::librdkafka
        CONAN_PKG::boost_filesystem
        CONAN_PKG::boost_system
        CONAN_PKG::boost_dll
        CONAN_PKG::flatbuffers
        CONAN_PKG::spdlog
        CONAN_PKG::jsonformoderncpp
        CONAN_PKG::libgit2)
