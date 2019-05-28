file(GLOB SRC_FILES
        ../src/*.cpp
        ../src/*.h
        ../src/KafkaW/*.cpp
        ../src/KafkaW/*.h
)

list(FILTER SRC_FILES EXCLUDE REGEX ".*main.cpp$")

add_executable(UnitTests
        ${SRC_FILES}
        RequestHandlerTest.cpp
        FlatbuffersTranslatorTest.cpp
        JSONPrintingTest.cpp
        UnitTests.cpp)

target_link_libraries(UnitTests
        ${CONAN_PACKAGES}
        CONAN_PKG::gtest)
