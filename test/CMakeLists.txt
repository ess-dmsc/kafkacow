file(GLOB SRC_FILES
        ../src/*.cpp
        ../src/*.h
        ../src/Kafka/*.cpp
        ../src/Kafka/*.h
)

list(FILTER SRC_FILES EXCLUDE REGEX ".*main.cpp$")

add_executable(UnitTests
        ${SRC_FILES}
        RequestHandlerTest.cpp
        FlatbuffersTranslatorTest.cpp
        JSONPrintingTest.cpp
        UnitTests.cpp
        MessageTest.cpp)

target_link_libraries(UnitTests PRIVATE
        ${CONAN_PACKAGES}
        CONAN_PKG::gtest)

# Link stdc++fs or c++experimental to get std::experimental::filesystem when necessary
target_link_libraries(UnitTests PRIVATE $<$<AND:$<CXX_COMPILER_ID:GNU>,$<VERSION_LESS:$<CXX_COMPILER_VERSION>,9.0>>:stdc++fs>)
target_link_libraries(UnitTests PRIVATE $<$<AND:$<CXX_COMPILER_ID:AppleClang>,$<VERSION_LESS:$<CXX_COMPILER_VERSION>,11.0>>:c++experimental>)
