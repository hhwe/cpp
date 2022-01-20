file(GLOB INCLUDE_FILES "*.h")

install(FILES
    ${INCLUDE_FILES}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/json)




    include_directories(${PROJECT_SOURCE_DIR}/include)
    set(APP_SRC mystl_test.cpp)
    set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
    add_executable(mystl_test ${APP_SRC})