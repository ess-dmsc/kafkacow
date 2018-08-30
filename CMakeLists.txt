cmake_minimum_required(VERSION 3.6)
project(kafkaCow)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
if (NOT WIN32)
 set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -pedantic" )
endif()

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${PROJECT_SOURCE_DIR}/cmake")

file(GLOB listOfSchemas "${PROJECT_SOURCE_DIR}/streaming-data-types/schemas/*.fbs" )
message(${listOfSchemas})
list(REMOVE_DUPLICATES listOfSchemas)
foreach(schema ${listOfSchemas})
    configure_file(${schema} "${CMAKE_CURRENT_BINARY_DIR}/schemas" COPYONLY)
endforeach()

if(NOT CONAN_DISABLE)
    include(${CMAKE_MODULE_PATH}/conan.cmake)
    conan_cmake_run(CONANFILE conanfile.txt
            PROFILE default
            BASIC_SETUP CMAKE_TARGETS SKIP_RPATH
            BUILD_TYPE "None"
            BUILD outdated)
endif()
if(CMAKE_COMPILER_IS_GNUCXX AND COV)
    include(CodeCoverage)
    setup_target_for_coverage(coverage UnitTests coverage)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_COVERAGE}")
endif()
add_subdirectory(src)
add_subdirectory(test)


