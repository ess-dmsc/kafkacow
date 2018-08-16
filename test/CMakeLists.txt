add_executable(UnitTests RequestHandlerTest.cpp)

target_link_libraries(UnitTests
        CONAN_PKG::gtest)