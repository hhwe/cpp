include_directories(${PROJECT_SOURCE_DIR}/include)
set(APP_SRC mystl_test.cpp)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
add_executable(mystl_test ${APP_SRC})