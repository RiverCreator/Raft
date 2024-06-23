set(INCLUDE_SOURCES
    ${PROJECT_SOURCE_DIR}/include
)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)
message(${PROJECT_SOURCE_DIR})
include_directories(${INCLUDE_SOURCES})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)
add_subdirectory(proto)
add_subdirectory(src)