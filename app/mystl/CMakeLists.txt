project(mystl)

file(GLOB INCLUDE_FILES "include/*.h")
file(GLOB SRC_FILES "src/*.cc")
file(GLOB TEST_FILES "test/*.cc")

include_directories(${PROJECT_SOURCE_DIR}/include/)
include_directories(${PROJECT_SOURCE_DIR}/include/allocator)
include_directories(${PROJECT_SOURCE_DIR}/include/iterator)
include_directories(${PROJECT_SOURCE_DIR}/include/algorithm)
include_directories(${PROJECT_SOURCE_DIR}/include/container)
message("include_directories" ${PROJECT_SOURCE_DIR}/include)

add_executable(mystl_test ${TEST_FILES})
# add_executable(mystl ${SRC_FILES})
