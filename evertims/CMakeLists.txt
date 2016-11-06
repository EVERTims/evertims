cmake_minimum_required(VERSION 2.6)

set(PROJECT ims)
project(${PROJECT})

set(EVERTIMS_SOURCES
    src/socket.cc
    src/main.cc
    src/solver.cc
    src/reverbEstimate.cc
    src/writer.cc
    src/reader.cc
    src/OSC/OSC-client.c
    src/OSC/OSC-timetag.c
    )


add_executable(${PROJECT} ${EVERTIMS_SOURCES})

add_definitions("-D__${CMAKE_SYSTEM_NAME}")

include(FindThreads)
set(CMAKE_THREAD_PREFER_PTHREAD ON)
find_package(Threads)

find_package(evert REQUIRED)

target_include_directories(${PROJECT}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/OSC
    PUBLIC ${evert_INCLUDE_DIRS}
    )

target_link_libraries(${PROJECT}
    PUBLIC ${OPENGL_LIBRARIES}
    PUBLIC ${evert_LIBRARIES}
    Threads::Threads
    )


install(
    TARGETS ${PROJECT}
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin)
