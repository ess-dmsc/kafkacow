cmake_minimum_required(VERSION 3.6)
project(kafkacow)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall" )
if (NOT WIN32)
 set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -pedantic" )
endif()

set(SRC_CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH ${SRC_CMAKE_MODULE_PATH})
list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})

# Use gold linker if available on system (it is faster than default GNU linker)
if (UNIX AND NOT APPLE)
    execute_process(COMMAND ${CMAKE_C_COMPILER} -fuse-ld=gold -Wl,--version ERROR_QUIET OUTPUT_VARIABLE ld_version)
    if ("${ld_version}" MATCHES "GNU gold")
        message(STATUS "Using gold linker")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=gold -Wl,--disable-new-dtags")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fuse-ld=gold -Wl,--disable-new-dtags")
    endif()
endif()

file(GLOB listOfSchemas "${PROJECT_SOURCE_DIR}/streaming-data-types/schemas/*.fbs" )
list(REMOVE_DUPLICATES listOfSchemas)
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/schemas/")
foreach(schema ${listOfSchemas})
    configure_file(${schema} "${CMAKE_CURRENT_BINARY_DIR}/schemas/" COPYONLY)
endforeach()

if(NOT CONAN_DISABLE)
    include(${SRC_CMAKE_MODULE_PATH}/conan.cmake)
    conan_cmake_run(CONANFILE conanfile.txt
            PROFILE default
            BASIC_SETUP CMAKE_TARGETS SKIP_RPATH
            BUILD_TYPE "None"
            BUILD outdated
            KEEP_RPATHS)
endif()

set(HTML_COVERAGE_REPORT OFF CACHE BOOL "Generate test coverage report as html, find in <BUILD_DIR>/coverage/index.html")
if(CMAKE_COMPILER_IS_GNUCXX AND COV OR HTML_COVERAGE_REPORT)
  include(CodeCoverage)
  append_coverage_compiler_flags()
  set(COVERAGE_EXCLUDE test)  # Exclude tests from test coverage
  if (HTML_COVERAGE_REPORT)
    setup_target_for_coverage_gcovr_html(NAME coverage
                                         EXECUTABLE UnitTests
                                         EXCLUDE ${COVERAGE_EXCLUDE}
                                         DEPENDENCIES UnitTests)
  else()
    setup_target_for_coverage_gcovr_xml(NAME coverage
                                        EXECUTABLE UnitTests
                                        EXCLUDE ${COVERAGE_EXCLUDE}
                                        DEPENDENCIES UnitTests)
  endif()
endif()

set(CONAN_PACKAGES
    CONAN_PKG::cli11
    CONAN_PKG::librdkafka
    CONAN_PKG::flatbuffers
    CONAN_PKG::spdlog
    CONAN_PKG::nlohmann_json
    CONAN_PKG::streaming-data-types
    CONAN_PKG::date)

add_subdirectory(src)
add_subdirectory(test)

# add a target to generate API documentation with Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND AND BUILD_DOCS)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target(doc
            ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen" VERBATIM
            )
endif()
