cmake_minimum_required(VERSION 2.6)

set (PROJECT evert)
project (${PROJECT})

set (EVERT_SOURCES
    src/elBSP.cc
    src/elOrientedPoint.cc
    src/elVector.cc
    src/elListener.cc
    src/elPathSolution.cc
    src/elRoom.cc
    src/material.cc
    src/elSource.cc
    src/elPolygon.cc
    src/elBeam.cc
    )

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
link_libraries(${OPENGL_LIBRARIES})

add_library (${PROJECT} SHARED ${EVERT_SOURCES})

add_definitions ("-D__${CMAKE_SYSTEM_NAME}")

file (WRITE ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT}Config.cmake
"\
set (evert_INCLUDE_DIRS ${CMAKE_INSTALL_PREFIX}/include/evert)
set (evert_LIBRARIES ${CMAKE_INSTALL_PREFIX}/lib/libevert${CMAKE_SHARED_LIBRARY_SUFFIX})")

# target_compile_options(evert PRIVATE -stdlib=libstdc++)

install(
    TARGETS ${PROJECT}
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/ DESTINATION include/${PROJECT}
    FILES_MATCHING PATTERN "*.h")

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT}Config.cmake
    DESTINATION lib/cmake/${PROJECT})

