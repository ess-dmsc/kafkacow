cmake_minimum_required(VERSION 3.6)
project(kafkaCow)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
if (NOT WIN32)
 set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -pedantic" )
endif()

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${PROJECT_SOURCE_DIR}/cmake")

file(GLOB listOfSchemas "${PROJECT_SOURCE_DIR}/streaming-data-types/schemas/*.fbs" )
list(REMOVE_DUPLICATES listOfSchemas)
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/schemas/")
foreach(schema ${listOfSchemas})
    configure_file(${schema} "${CMAKE_CURRENT_BINARY_DIR}/schemas/" COPYONLY)
endforeach()

configure_file("${PROJECT_SOURCE_DIR}/AppImageResources/icon.png" "${CMAKE_CURRENT_BINARY_DIR}/icon.png" COPYONLY)
configure_file("${PROJECT_SOURCE_DIR}/AppImageResources/kafkacow.desktop" "${CMAKE_CURRENT_BINARY_DIR}/kafkacow.desktop" COPYONLY)

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