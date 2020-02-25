file(GLOB SRC_FILES
        ../src/*.cpp
        ../src/*.h
        ../src/Kafka/*.cpp
        ../src/Kafka/*.h
        ../src/imgui/*.cpp
)

list(FILTER SRC_FILES EXCLUDE REGEX ".*main.cpp$")

add_executable(UnitTests
        ${SRC_FILES}
        RequestHandlerTest.cpp
        FlatbuffersTranslatorTest.cpp
        JSONPrintingTest.cpp
        UnitTests.cpp
        MessageTest.cpp)

target_link_libraries(UnitTests
        ${CONAN_PACKAGES}
        CONAN_PKG::gtest)
